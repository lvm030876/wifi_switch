#include <ESP8266mDNS.h>
#include "Ticker.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
#include <WiFiUdp.h>
#include "eepromapi.h"
#include "relayapi.h"
#include "RCSwitch.h"
#include "IRsend.h"
#include "webgui.h"

#define LED_PIN 2
#define RESET_PIN 0
#define PIR_PIN 5	//датчик руху
#define IR_PIN 4	//RS пульт	4
#define RF_PIN 1	//радіомодуль	1
#define debugSerial Serial1

int lightStatus, pirStatus, alarmStatus;
unsigned long rfCode;
int resetTick = 0;

EepromClass eepromapi;
IOTconfig customVar;
RellayClass rellay(1);
Ticker blinker, sensor, alloff;
ESP8266WebServer HTTP(80);
RCSwitch mySwitch = RCSwitch();
WiFiUDP udp;
IRsend irsend(IR_PIN);

void tickBlink() {
	int state = digitalRead(LED_PIN);
	digitalWrite(LED_PIN, !state);
}

void hold(const unsigned int &ms) {
	unsigned long m = millis();
	while (millis() - m < ms) {
		yield();
	}
}

void AP_mode_default(){
	blinker.attach(3, tickBlink);
	String nameDev = WiFi.macAddress();
	nameDev.replace(":", "");
	nameDev = "SWITCH_" + nameDev;
	char AP_ssid_default[32]; 
	nameDev.toCharArray(AP_ssid_default, nameDev.length() + 1);
	char AP_pass_default[32] = "12345678"; 
	WiFi.mode(WIFI_OFF);
	WiFi.softAP(AP_ssid_default, AP_pass_default);
	digitalWrite(LED_PIN, HIGH);
	debugSerial.println("AP started");
}

void STATION_mode(){
	blinker.attach(0.5, tickBlink);
	WiFi.mode(WIFI_OFF);
	WiFi.mode(WIFI_STA);
	WiFi.hostname("iot-switch");
	if (customVar.dhc == 0xaa55) {
		IPAddress gateway = WiFi.gatewayIP();
		IPAddress subnet = WiFi.subnetMask();
		WiFi.config(customVar.statIp, gateway, subnet);
	}
	WiFi.begin(customVar.STA_ssid, customVar.STA_pass);
	hold(100);
	while (WiFi.status() != WL_CONNECTED){
		delay(250); debugSerial.print(".");
		if(WiFi.status() == WL_NO_SSID_AVAIL){}
		if(WiFi.status() ==  WL_CONNECT_FAILED){}
	}
	blinker.detach();
	digitalWrite(LED_PIN, HIGH);
	debugSerial.println("\n STA started");
}

void scanwifi_json() {
	int n = WiFi.scanNetworks();
	String outStr = "{\"scan\":[";
	// '{"sacn":[]}'
	if (n > 0){
		// '{"sacn":[{"sidd":"lvm1976","rssi":"66666666"},{"sidd":"roll","rssi":"12345678"}]}' 
		for (int i = 0; i < n; i++){
			outStr += "{\"ssid\":\"";
			outStr += WiFi.SSID(i);
			outStr += "\", ";
			outStr += "\"mac\":\"";
			outStr += WiFi.BSSIDstr(i);
			outStr += "\", ";
			outStr += "\"channel\":";
			outStr += WiFi.channel(i);
			outStr += ", ";
			outStr += "\"pass\":";
			outStr += WiFi.encryptionType(i) != ENC_TYPE_NONE ? "true" : "false";
			outStr += ", ";
			outStr += "\"hidden\":";
			outStr += WiFi.isHidden(i)? "true" : "false";
			outStr += ", ";
			outStr += "\"rssi\":";
			outStr += WiFi.RSSI(i);
			outStr += "}";
			if ((i + 1) != n) outStr += ", ";
		}
	}
	outStr += "]}";
	HTTP.send(200,"application/json",outStr);
	WiFi.scanDelete();
}

void smart_res() {
	debugSerial.println("Reset devise");
	WiFi.disconnect(true);
	eepromapi.eeprom_clr();
	hold(5000);
	ESP.eraseConfig();
	ESP.reset();
}

void cmdGo(int a, int b, int c){
	alloff.detach();
	rellay.rellay(a, b, c);
}

void sensorTik(){
	lightStatus = map(analogRead(A0), 0, 1024, 100, 0);
	pirStatus = digitalRead(PIR_PIN);
	if (digitalRead(RESET_PIN) == 0) {
		if (resetTick++ > 100) smart_res();
	} else resetTick = 0;
	if (mySwitch.available()) {
		rfCode = mySwitch.getReceivedValue();
		if (rfCode == customVar.rfAOn) cmdGo(2, 3, 3);
		if (rfCode == customVar.rfBOn) cmdGo(3, 2, 3);
		if (rfCode == customVar.rfCOn) cmdGo(3, 3, 2);
		if (rfCode == customVar.rfOn) cmdGo(1, 1, 1);
		if (rfCode == customVar.rfOff) cmdGo(0, 0, 0);
		mySwitch.resetAvailable();
	}
}

void switch_web() {
	HTTP.send(200,"text/json","{\"status\":\"ok\"}");
	if (HTTP.arg("switchA") != "") cmdGo(HTTP.arg("switchA").toInt(), 3, 3);
	if (HTTP.arg("switchB") != "") cmdGo(3, HTTP.arg("switchB").toInt(), 3);
	if (HTTP.arg("switchC") != "") cmdGo(3, 3, HTTP.arg("switchC").toInt());
	if (HTTP.arg("pirProt") != "") {
		alloff.detach();
		rellay.protect(HTTP.arg("pirProt").toInt());
	}
}

void pirDo(){
	alarmStatus = 1;
	Status swStat = rellay.getStatus();
	if (((alloff.active() || (swStat.a + swStat.b + swStat.c + swStat.pr) == 0)) && (lightStatus <= customVar.pirLight) && (customVar.pirTime != 0)) {
		alloff.detach();
		if (customVar.pirGroup == 0) rellay.rellay(1, 0, 0);
		if (customVar.pirGroup == 1) rellay.rellay(0, 1, 0);
		if (customVar.pirGroup == 2) rellay.rellay(0, 0, 1);
		alloff.once(customVar.pirTime, [](){rellay.rellay(0, 0, 0);});
	}
}

void switch_xml() {
	Status swStat = rellay.getStatus();
	String relayXml = "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\n<switch>\n\t<mac>"
		+ WiFi.macAddress()
		+ "</mac>\n\t<switchA>" + swStat.a 
		+ "</switchA>\n\t<switchB>" + swStat.b
		+ "</switchB>\n\t<switchC>" + swStat.c
		+ "</switchC>\n\t<pirProt>" + swStat.pr
		+ "</pirProt>\n\t<pirStatus>" + pirStatus			// статус датчику руха
		+ "</pirStatus>\n\t<alarmStatus>" + alarmStatus
		+ "</alarmStatus>\n\t<lightValue>" + lightStatus	// відносний рівень освітлення
		+ "</lightValue>\n\t<RSSI>" + WiFi.RSSI()
		+ "</RSSI>\n</switch>";
	HTTP.send(200,"text/xml",relayXml);
}

void switch_json() {
	Status swStat = rellay.getStatus();
	String relayJson = "{\"switch\": {\"mac\":\""
		+ WiFi.macAddress()
		+ "\",\"switchA\":" + swStat.a 
		+ ",\"switchB\":" + swStat.b
		+ ",\"switchC\":" + swStat.c
		+ ",\"pirProt\":" + swStat.pr
		+ ",\"pirStatus\":" + pirStatus			// статус датчику руха
		+ ",\"alarmStatus\":" + alarmStatus
		+ ",\"lightValue\":" + lightStatus	// відносний рівень освітлення
		+ ",\"RSSI\":" + WiFi.RSSI()
		+ "}}";
	HTTP.send(200,"text/json",relayJson);
}

void rf_json() {
	String rfJson = "{\"rf\": {\"rfNew\":"
		+ String(rfCode, DEC)
		+ ",\"rfAOn\":" + customVar.rfAOn
		+ ",\"rfBOn\":" + customVar.rfBOn
		+ ",\"rfCOn\":" + customVar.rfCOn
		+ ",\"rfOn\":" + customVar.rfOn
		+ ",\"rfOff\":" + customVar.rfOff
		+ "}}";	
	HTTP.send(200,"application/json",rfJson);
}

void pir_json() {
	String json = "{\"pir\": {\"pirTime\":" 
		+ String(customVar.pirTime) 
		+ ",\"pirLight\":" 
		+ String(customVar.pirLight) 
		+ ",\"pirGroup\":" 
		+ String(customVar.pirGroup) 
		+ "}}";	
	HTTP.send(200,"text/json",json);
}

void ir_web() {
	HTTP.send(200,"text/json","{\"status\":\"ok\"}");
	uint16_t irSignal[250];
	int khz = HTTP.arg("khz").toInt();
	if (khz == 0) khz = 36;
	int rep = HTTP.arg("rep").toInt();
	if (rep == 0) rep = 1;
	String irDate = HTTP.arg("date");
	if (irDate != "") {
		int irDate_len = irDate.length() + 1; 
		char char_array[irDate_len];
		irDate.toCharArray(char_array, irDate_len);

		char* pch = strtok(char_array, ",");
		int i = 0;
		while (pch != NULL) {
			irSignal[i] = atoi (pch);
			pch = strtok (NULL, ",");
			i++;
		}
		while (rep){
			irsend.sendRaw(irSignal, i, khz);
			rep--;	
		}
	}
}

void reset_alarm() {
	HTTP.send(200,"text/json","{\"status\":\"ok\"}");
	alarmStatus = 0;
}

void mem_set() {
	int t;
	boolean flag = false;
	boolean rst = false;
	HTTP.send(200,"application/json","{\"status\":\"ok\"}");
	if (HTTP.arg("ipStat") != "") {
		flag = true;
		IPAddress ip;
		ip.fromString(HTTP.arg("ipStat"));
		IPAddress gateway = WiFi.gatewayIP();
		IPAddress subnet = WiFi.subnetMask();
		hold(1000);
		WiFi.config(ip, gateway, subnet);
		customVar.statIp = ip;
		customVar.dhc = 0xaa55;
		rst = true;
	}
	if (HTTP.arg("ssid") != "") {
		flag = true;
		String str = HTTP.arg("ssid");
    	str.toCharArray(customVar.STA_ssid, str.length() + 1);
		customVar.wifimode = 1;
		rst = true;
	}
	if (HTTP.arg("pass") != "") {
		flag = true;
		String str = HTTP.arg("pass");
    	str.toCharArray(customVar.STA_pass, str.length() + 1);
		customVar.wifimode = 1;
		rst = true;
	}
	if (HTTP.arg("pirTime") != "") {
		t = HTTP.arg("pirTime").toInt();
		if ((t >= 0) && (t <= 30*60)) {
			flag = true;
			customVar.pirTime = t;
			}
	}
	if (HTTP.arg("pirLight") != "") {
		t = HTTP.arg("pirLight").toInt();
		if ((t >= 0) && (t <= 100)) {
			flag = true;
			customVar.pirLight = t;
		}
	}
	if (HTTP.arg("pirGroup") != "") {
		t = HTTP.arg("pirGroup").toInt();
		if ((t <= 2) && (t >= 0)) {
			flag = true;
			customVar.pirGroup = t;
		} 
	}
	if (HTTP.arg("rfAOn") != "") {
		t =  HTTP.arg("rfAOn").toInt();
		customVar.rfAOn = (t < 0)? 0:(t > 0)? t: rfCode;
		flag = true;
	}
	if (HTTP.arg("rfBOn") != "") {
		t =  HTTP.arg("rfBOn").toInt();
		customVar.rfBOn = (t < 0)? 0:(t > 0)? t: rfCode;
		flag = true;
	}
	if (HTTP.arg("rfCOn") != "") {
		t =  HTTP.arg("rfCOn").toInt();
		customVar.rfCOn = (t < 0)? 0:(t > 0)? t: rfCode;
		flag = true;
	}
	if (HTTP.arg("rfOn") != "") {
		t =  HTTP.arg("rfOn").toInt();
		customVar.rfOn = (t < 0)? 0:(t > 0)? t: rfCode;
		flag = true;
	}
	if (HTTP.arg("rfOff") != "") {
		t =  HTTP.arg("rfOff").toInt();
		customVar.rfOff = (t < 0)? 0:(t > 0)? t: rfCode;
		flag = true;
	}
	if (flag) eepromapi.eeprom_set(customVar);
	if (rst) {
		hold(5000);
		ESP.restart();
		hold(500);
	}
}

void startServer() {
    HTTP.on("/", HTTP_GET, [](){HTTP.send(200, "text/html", homeIndex);});
    HTTP.on("/style.css", HTTP_GET, [](){HTTP.send(200, "text/css", style);});
	HTTP.on("/switch", HTTP_GET, switch_web);
	HTTP.on("/resalarm", HTTP_GET, reset_alarm);
	HTTP.on("/switch.xml", HTTP_GET, switch_xml);
	HTTP.on("/switch.json", HTTP_GET, switch_json);
    HTTP.on("/setauto.htm", HTTP_GET, [](){HTTP.send(200, "text/html", pirIndex);});
	HTTP.on("/pir.json", HTTP_GET, pir_json);
	HTTP.on("/rf.json", HTTP_GET, rf_json);
    HTTP.on("/rf.htm", HTTP_GET, [](){HTTP.send(200, "text/html", rfIndex);});
    HTTP.on("/ip.htm", HTTP_GET, [](){HTTP.send(200, "text/html", ipIndex);});
	HTTP.on("/scanwifi.json", HTTP_GET, scanwifi_json);
    HTTP.on("/ir.htm", HTTP_GET, [](){HTTP.send(200, "text/html", irIndex);});
	HTTP.on("/irsend", HTTP_GET, ir_web);
    HTTP.on("/help", HTTP_GET, [](){HTTP.send(200, "text/html", helpIndex);});
	HTTP.on("/mem", HTTP_GET, mem_set);
	HTTP.on("/default", HTTP_GET, [](){
		HTTP.send(200, "text/plain", "OK");
		hold(5000);
		smart_res();
		});
	HTTP.on("/reboot", HTTP_GET, [](){
		HTTP.send(200, "text/plain", "OK");
		hold(5000);
		ESP.restart();
		hold(500);
		});
    HTTP.on("/upgrade", HTTP_GET, [](){HTTP.send(200, "text/html", upgradeIndex);});
    HTTP.on("/update", HTTP_POST, [](){
		HTTP.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
		hold(5000);
		ESP.restart();
		hold(500);
    },[](){
		HTTPUpload& upload = HTTP.upload();
		if(upload.status == UPLOAD_FILE_START){
			debugSerial.setDebugOutput(true);
			WiFiUDP::stopAll();
			debugSerial.printf("Update: %s\n", upload.filename.c_str());
			uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
			if(!Update.begin(maxSketchSpace)){
				Update.printError(debugSerial);
			}
		} else if(upload.status == UPLOAD_FILE_WRITE){
			if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
				Update.printError(debugSerial);
			}
		} else if(upload.status == UPLOAD_FILE_END){
			if(Update.end(true)){
				debugSerial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
			} else {
				Update.printError(debugSerial);
			}
			debugSerial.setDebugOutput(false);
		}
		yield();
    });
	HTTP.begin();
}

void setup() {
	debugSerial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
	debugSerial.println("\n start");
	pinMode(PIR_PIN, INPUT_PULLUP);
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);
	pinMode(RESET_PIN, INPUT);
	eepromapi.eeprom_init();
	mySwitch.enableReceive(RF_PIN);
	irsend.begin();
	sensor.attach(0.1, sensorTik);
	customVar = eepromapi.eeprom_get();
	(customVar.wifimode == 0)? AP_mode_default(): STATION_mode();
	if (MDNS.begin("iot-switch", WiFi.localIP())) {
		debugSerial.println("\n MDNS responder started, name is http://iot-switch.local/");
	}
	udp.begin(8266);
	startServer();
	MDNS.addService("http", "tcp", 80);
}

void loop() {
	HTTP.handleClient();
	MDNS.update();
	if (pirStatus == 1) pirDo();
	int packetSize = udp.parsePacket();
	if(packetSize) {
		char packetBuffer[10];
		udp.read(packetBuffer, 10);
		String udpRead = packetBuffer;
		if (udpRead.indexOf("Who") != -1){
			char IP[] = "xxx.xxx.xxx.xxx";
			WiFi.localIP().toString().toCharArray(IP, 16);
			IPAddress broadcastIp = WiFi.localIP();
			broadcastIp[3] = 255;
			udp.beginPacket(broadcastIp, 8266);
			udp.write("switch: ");
			udp.write(IP);
			udp.endPacket();
		}
	}
}