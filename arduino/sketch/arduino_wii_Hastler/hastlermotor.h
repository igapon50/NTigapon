#ifndef _hastlermotor_h
#define _hastlermotor_h

#define MAX_MOTER_VAL 510
#define STOP_MOTER_VAL 255
#define MIN_MOTER_VAL 0

void hastler_motor_init();
int hastler_moter_front(const int val = STOP_MOTER_VAL);
int hastler_moter_back(const int val = STOP_MOTER_VAL);

#endif
