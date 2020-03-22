#ifndef config_h
#define config_h
#include "Arduino.h"
#include <ESP8266mDNS.h>
#include "Ticker.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
#include "IRremoteESP8266.h"
#include "IRsend.h"
#include "WiFiManager.h"
#include "RCSwitch.h"
#include "eepromclass.h"
#include "relay.h"
#include "index.h"
#include "ipstat.h"
#include "setauto.h"
#include "rfstat.h"
#include "irstat.h"


// #define PROGMEM   ICACHE_RODATA_ATTR
#define LED_PIN 2
#define RESET_PIN 0
#define PIR_PIN 5	//датчик руху
#define IR_PIN 4	//RS пульт	4
#define SEN_PIN 1	//радіомодуль	1
#define debugSerial Serial1

void startServer();
void tickBlink();
void smart_res();
void switch_xml();
void switch_json();
void pir_xml();
void pir_json();
void rf_xml();
// void rf_json();
void reset_alarm();
void switch_web();
void mem_set();
void pirDo();
void sensorTik();
void ir_web();
// void ICACHE_RAM_ATTR reset_WIFI_cfg();

// volatile boolean reset_cfg = false;

int lightStatus, pirStatus, alarmStatus;
unsigned long rfCode;
int resetTick = 0;  // 

const char* upgradeIndex = R"=====(<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>)=====";

#endif