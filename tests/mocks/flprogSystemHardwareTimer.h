#pragma once
#include <stdint.h>
#define INPUT_PULLUP 2
#define OUTPUT 1
#define LED_BUILTIN 13
#define PB11 21
#define PC13 22
void initFlprogSystemTimer(void (*fn)());
void setFlprogSystemTimerTickPeriod(uint32_t);
uint32_t getFlprogSystemTimerTickPeriod();
void pinMode(int,int);
void digitalWrite(int,bool);
int digitalRead(int);
uint32_t millis();
void delay(unsigned);
struct MockSerial { void begin(int); void print(const char *); void println(unsigned long); };
extern MockSerial Serial;
