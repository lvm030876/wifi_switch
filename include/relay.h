#ifndef RELAY_H
#define RELAY_H
#include "Arduino.h"

#define SWITCH_A 12
#define SWITCH_B 13
#define SWITCH_C 14

struct Status {
    int a, b, c, pr;
};

class RellayClass {
private:
    Status _status;
public:
    RellayClass(int a);
    void rellay(int a, int b, int c);
    void protect(int pr);
    Status getStatus();
};
#endif