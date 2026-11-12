#include "flprogWatchdogTimer.h"
#include "flprogSystemHardwareTimer.h"

// Исходный пример использовал PB11/PC13 (STM32). Для остальных плат —
// типичные пины кнопки и встроенного светодиода; при необходимости смените их.
#ifdef RT_HW_CORE_STM32
#define BREAK_PIN PB11
#define BLINK_PIN PC13
#else
#define BREAK_PIN 2
#ifdef LED_BUILTIN
#define BLINK_PIN LED_BUILTIN
#else
#define BLINK_PIN 13
#endif
#endif

bool blinkValue = true;
FLProgrogWatchdogTimer watchdogTimer(5); // 5 секунд без pool() => перезапуск

void FLProgTimerHandler();
void WatchdogTimerHandler();

void setup()
{
    Serial.begin(115200);
    pinMode(BLINK_PIN, OUTPUT);
    digitalWrite(BLINK_PIN, blinkValue);
    pinMode(BREAK_PIN, INPUT_PULLUP);

    // Аппаратный таймер вызывает FLProgTimerHandler() с периодом около 10 мкс.
    initFlprogSystemTimer(FLProgTimerHandler);
    setFlprogSystemTimerTickPeriod(10);
    watchdogTimer.setTickPeriod(getFlprogSystemTimerTickPeriod());

    // Callback необязателен. Он исполняется из прерывания: здесь нельзя
    // выполнять Serial.print(), delay() и другие блокирующие операции.
    watchdogTimer.setCallBack(WatchdogTimerHandler);
}

void loop()
{
    // Сообщаем сторожевому таймеру, что основной цикл продолжает работать.
    watchdogTimer.pool();

    Serial.print("StartTime = ");
    Serial.println(millis());

    // Удержание кнопки имитирует зависание основного цикла. Прерывания
    // аппаратного таймера продолжаются, и примерно через 5 с будет перезапуск.
    if (!digitalRead(BREAK_PIN))
    {
        while (true) { }
    }

    delay(1000);
    blinkValue = !blinkValue;
    digitalWrite(BLINK_PIN, blinkValue);
}

void WatchdogTimerHandler()
{
    // Место для БЫСТРОГО действия перед сбросом; пустой обработчик безопасен.
    // Не использовать Serial, delay(), ожидание блокировок и выделение памяти.
}

void FLProgTimerHandler()
{
    watchdogTimer.tick();
}
