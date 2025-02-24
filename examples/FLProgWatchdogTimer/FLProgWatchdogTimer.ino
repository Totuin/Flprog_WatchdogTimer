#include "flprogWatchdogTimer.h"
#include "flprogSystemHardwareTimer.h"
#define BREAK_PIN PB11
#define BLINK_PIN PC13 

bool blinkValue = true;

FLProgrogWatchdogTimer watchdogTimer(5);

void setup() {
  Serial.begin(115200);
  pinMode(BLINK_PIN, OUTPUT);
  digitalWrite(BLINK_PIN, blinkValue);
  pinMode(BREAK_PIN, INPUT_PULLUP);
  initFlprogSystemTimer(FLProgTimerHandler);
  setFlprogSystemTimerTickPeriod(10);
  watchdogTimer.setTickPeriod(getFlprogSystemTimerTickPeriod());
  watchdogTimer.setCallBack(WatchdogTimerHandler); //Необязательно!! передается ссылка на функцию которая будет вызываться перед перезагрузкой

}

void loop() {
  watchdogTimer.pool();
  Serial.print("StartTime = ");
  Serial.println(millis());

 if (!(digitalRead(BREAK_PIN)))
  { while (true)
    {}
  }
  delay(1000);
  blinkValue = !blinkValue;
  digitalWrite(BLINK_PIN, blinkValue);

}

void WatchdogTimerHandler()
{
  Serial.print("EndTime = ");
  Serial.println(millis());
  delay(500);
}

void FLProgTimerHandler()
{
  watchdogTimer.tick();
}