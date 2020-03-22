#ifndef EEPROMCLASS_H
#define EEPROMCLASS_H
#include "EEPROM.h"
#include <ESP8266WiFi.h>

struct IOTconfig{
  int dhc;
  IPAddress statIp;
  int pirTime;
  int pirLight;
  int pirGroup;
  unsigned long rfAOn;
  unsigned long rfBOn;
  unsigned long rfCOn;
  unsigned long rfOn;
  unsigned long rfOff;
  int start;
};

class EepromClass {
  public:
    void eeprom_init();
    void eeprom_clr();
    void eeprom_set(IOTconfig &obj);
    IOTconfig eeprom_get();
  private:
    IOTconfig _customVar;
    int _len = sizeof(_customVar);
    void eeprom_save();
    void eeprom_load();
};

#endif