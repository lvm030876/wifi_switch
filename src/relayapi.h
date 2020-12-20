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

RellayClass::RellayClass(int a) {
    pinMode(SWITCH_A, OUTPUT);
    pinMode(SWITCH_B, OUTPUT);
    pinMode(SWITCH_C, OUTPUT);
    rellay(0, 0, 0);
    protect(0);
};

void RellayClass::rellay(int a, int b, int c) {
    _status.a = (a < 2)? a: (a == 2)?(_status.a?0:1):_status.a;
    _status.b = (b < 2)? b: (b == 2)?(_status.b?0:1):_status.b;
    _status.c = (c < 2)? c: (c == 2)?(_status.c?0:1):_status.c;
    digitalWrite(SWITCH_A,_status.a);
    digitalWrite(SWITCH_B,_status.b);
    digitalWrite(SWITCH_C,_status.c);
};

void RellayClass::protect(int pr) {
    _status.pr = (pr < 2)? pr: (_status.pr?0:1);
};

Status RellayClass::getStatus(){
    return _status;
};