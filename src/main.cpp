#include "config.h"

EepromClass eepromapi;
RellayClass rellay(1);
Ticker blinker, sensor, alloff;
ESP8266WebServer HTTP(80);
RCSwitch mySwitch = RCSwitch();
IRsend irsend(IR_PIN);

void setup() {
	debugSerial.begin(115200);
	debugSerial.println("\n start");
	pinMode(PIR_PIN, INPUT);
	pinMode(LED_PIN, OUTPUT);
	blinker.attach(0.5, tickBlink);
	pinMode(RESET_PIN, INPUT);
	// attachInterrupt(RESET_PIN, reset_WIFI_cfg, FALLING);
	mySwitch.enableReceive(SEN_PIN);
	irsend.begin();
	eepromapi.eeprom_init();
	sensor.attach(0.1, sensorTik);
	WiFiManager wifiManager;
	wifiManager.setTimeout(180);
	wifiManager.setAPCallback([](WiFiManager *myWiFiManager){
		blinker.detach();
		blinker.attach(0.5, tickBlink);
	});
	if (!wifiManager.autoConnect("iot_home")) {
		debugSerial.println("failed to connect and hit timeout");
		ESP.reset();
	}
	IOTconfig customVar = eepromapi.eeprom_get();
	if (customVar.dhc == 0xaa55) {
		IPAddress gateway = WiFi.gatewayIP();
		IPAddress subnet = WiFi.subnetMask();
		WiFi.config(customVar.statIp, gateway, subnet);
	}
	if (MDNS.begin("iot_home", WiFi.localIP())) {
		debugSerial.println("MDNS responder started");
	}
	blinker.detach();
	digitalWrite(LED_PIN, HIGH);
	startServer();
}

void loop() {
	HTTP.handleClient();
	MDNS.update();
}

void sensorTik(){
	lightStatus = map(analogRead(A0), 0, 1024, 100, 0);
	pirStatus = digitalRead(PIR_PIN);
	if (pirStatus == 1) pirDo();
	// if (reset_cfg) smart_res();
	if (digitalRead(RESET_PIN) == 0) {
		if (resetTick++ > 100) smart_res();
	} else resetTick = 0;
	if (mySwitch.available()) {
		rfCode = mySwitch.getReceivedValue();
		if (rfCode > 0) {
			IOTconfig customVar = eepromapi.eeprom_get();
			if (rfCode == customVar.rfAOn) {
				alloff.detach();
				rellay.rellay(1, 3, 3);
			}
			if (rfCode == customVar.rfBOn) {
				alloff.detach();
				rellay.rellay(3, 1, 3);
			}
			if (rfCode == customVar.rfCOn) {
				alloff.detach();
				rellay.rellay(3, 3, 1);
			}
			if (rfCode == customVar.rfOn) {
				alloff.detach();
				rellay.rellay(1, 1, 1);
			}
			if (rfCode == customVar.rfOff) {
				alloff.detach();
				rellay.rellay(0, 0, 0);
			}
		}
		mySwitch.resetAvailable();
	}
}

void pirDo(){
	alarmStatus = 1;
	boolean yes = false;
	Status swStat = rellay.getStatus();
	IOTconfig customVar = eepromapi.eeprom_get();
	if (alloff.active()) yes = true;
	else if (((swStat.a + swStat.b + swStat.c + swStat.pr) == 0) && (lightStatus <= customVar.pirLight) && (customVar.pirTime != 0)) yes = true;
	if (yes) {
		alloff.detach();
		if (customVar.pirGroup == 0) rellay.rellay(1, 0, 0);
		if (customVar.pirGroup == 1) rellay.rellay(0, 1, 0);
		if (customVar.pirGroup == 2) rellay.rellay(0, 0, 1);
		alloff.once(customVar.pirTime, [](){rellay.rellay(0, 0, 0);});
	}
}

void tickBlink() {
	int state = digitalRead(LED_PIN);
	digitalWrite(LED_PIN, !state);
}

void smart_res() {
	debugSerial.println("Reset devise");
	WiFi.disconnect(true);
	eepromapi.eeprom_clr();
	delay(5000);
	ESP.reset();
}

// void ICACHE_RAM_ATTR reset_WIFI_cfg() {
// 	reset_cfg = true;	
// }

void startServer() {
    HTTP.on("/", HTTP_GET, [](){HTTP.send(200, "text/html", homeIndex);});
    HTTP.on("/style.css", HTTP_GET, [](){HTTP.send(200, "text/css", style);});
	HTTP.on("/switch", HTTP_GET, switch_web);
	HTTP.on("/resalarm", HTTP_GET, reset_alarm);
	HTTP.on("/switch.xml", HTTP_GET, switch_xml);
	HTTP.on("/switch.json", HTTP_GET, switch_json);
    HTTP.on("/setauto.htm", HTTP_GET, [](){HTTP.send(200, "text/html", pirIndex);});
	HTTP.on("/pir.xml", HTTP_GET, pir_xml);
	HTTP.on("/pir.json", HTTP_GET, pir_json);
	HTTP.on("/rf.xml", HTTP_GET, rf_xml);
    HTTP.on("/rf.htm", HTTP_GET, [](){HTTP.send(200, "text/html", rfIndex);});
    HTTP.on("/ip.htm", HTTP_GET, [](){HTTP.send(200, "text/html", ipIndex);});
	HTTP.on("/mem", HTTP_GET, mem_set);
	HTTP.on("/default", HTTP_GET, smart_res);
    HTTP.on("/upgrade", HTTP_GET, [](){HTTP.send(200, "text/html", upgradeIndex);});
    HTTP.on("/ir.htm", HTTP_GET, [](){HTTP.send(200, "text/html", irIndex);});
	HTTP.on("/irsend", HTTP_GET, ir_web);
    HTTP.on("/update", HTTP_POST, [](){
		HTTP.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
		delay(5000);
		ESP.restart();
		delay(500);
    },[](){
		HTTPUpload& upload = HTTP.upload();
		if(upload.status == UPLOAD_FILE_START){
			debugSerial.setDebugOutput(true);
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

void rf_xml() {
	IOTconfig customVar = eepromapi.eeprom_get();
	String relayXml = "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\n<rf>\n\t<rfCode>"
		+ String(rfCode, DEC)
		+ "</rfCode>\n\t<rfAOn>" + customVar.rfAOn 
		+ "</rfAOn>\n\t<rfBOn>" + customVar.rfBOn
		+ "</rfBOn>\n\t<rfCOn>" + customVar.rfCOn
		+ "</rfCOn>\n\t<rfOn>" + customVar.rfOn
		+ "</rfOn>\n\t<rfOff>" + customVar.rfOff
		+ "</rfOff>\n</rf>";
	HTTP.send(200,"text/xml",relayXml);
}

void reset_alarm() {
	HTTP.send(200,"text/json","{\"status\":\"ok\"}");
	alarmStatus = 0;
}

void switch_web() {
	HTTP.send(200,"text/json","{\"status\":\"ok\"}");
	if (HTTP.arg("switchA") != "") {
		alloff.detach();
		rellay.rellay(HTTP.arg("switchA").toInt(), 3, 3);
		}
	if (HTTP.arg("switchB") != "") {
		alloff.detach(); 
		rellay.rellay(3, HTTP.arg("switchB").toInt(), 3);
		}
	if (HTTP.arg("switchC") != "") {
		alloff.detach(); 
		rellay.rellay(3, 3, HTTP.arg("switchC").toInt());
		}
	if (HTTP.arg("pirProt") != "") {
		alloff.detach();  
		rellay.protect(HTTP.arg("pirProt").toInt());
	}
}

void mem_set() {
	int t;
	boolean flag = false;
	HTTP.send(200,"text/json","{\"status\":\"ok\"}");
	IOTconfig customVar = eepromapi.eeprom_get();
	if (HTTP.arg("ipStat") != "") {
		flag = true;
		IPAddress ip;
		ip.fromString(HTTP.arg("ipStat"));
		IPAddress gateway = WiFi.gatewayIP();
		IPAddress subnet = WiFi.subnetMask();
		delay(1000);
		WiFi.config(ip, gateway, subnet);
		customVar.statIp = ip;
		customVar.dhc = 0xaa55;
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
}

void pir_xml() {
	IOTconfig swStat = eepromapi.eeprom_get();
	String xml = "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\n<pir>\n\t<pirTime>"
		+ String(swStat.pirTime)
		+ "</pirTime>\n\t<pirLight>" 
		+ String(swStat.pirLight)
		+ "</pirLight>\n\t<pirGroup>" 
		+ String(swStat.pirGroup)
		+ "</pirGroup>\n</pir>";	
	HTTP.send(200,"text/xml",xml);
}

void pir_json() {
	IOTconfig swStat = eepromapi.eeprom_get();
	String json = "{\"pir\": {\"pirTime\":\"" 
		+ String(swStat.pirTime) 
		+ "\",\"pirLight\":" 
		+ String(swStat.pirLight) 
		+ ",\"pirGroup\":" 
		+ String(swStat.pirGroup) 
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