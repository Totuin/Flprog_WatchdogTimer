#include "flprogWatchdogTimer.h"

FLProgrogWatchdogTimer::FLProgrogWatchdogTimer(uint8_t checkPeriod)
{
    setCheckPeriod(checkPeriod);
}

void FLProgrogWatchdogTimer::setCheckPeriod(uint8_t checkPeriod)
{
    // Ноль не обозначает отключение watchdog: он ошибочен для интервала ожидания.
    if ((checkPeriod == 0) || (_checkPeriod == checkPeriod))
    {
        return;
    }
    _checkPeriod = checkPeriod;
    calculateTickValue();
}

void FLProgrogWatchdogTimer::setTickPeriod(uint32_t tickPeriod)
{
    // Нулевой период недопустим (деление на ноль при пересчёте порога).
    // При ошибочном аргументе оставляем последнюю корректную настройку.
    if ((tickPeriod == 0) || (_tickPeriod == tickPeriod))
    {
        return;
    }
    _tickPeriod = tickPeriod;
    calculateTickValue();
}

void FLProgrogWatchdogTimer::calculateTickValue()
{
    // checkPeriod измеряется секундами, tickPeriod — микросекундами.
    // Максимум 255 * 1 000 000 умещается в uint32_t без переполнения.
    const uint32_t durationUs = (uint32_t)_checkPeriod * 1000000UL;

    // Округление ВВЕРХ исключает преждевременный перезапуск, когда период
    // аппаратного тика не делит длительность контроля без остатка.
    uint32_t ticks = durationUs / _tickPeriod;
    if ((durationUs % _tickPeriod) != 0)
    {
        ++ticks;
    }
    if (ticks == 0)
    {
        // Аппаратный тик оказался длиннее всего периода ожидания.
        ticks = 1;
    }

    // Параметры рассчитываются в основном коде; не перенастраивать сторож
    // одновременно с работающим ISR, если платформа не обеспечивает защиту.
    _tickValue = ticks;
    _tickCounter = 0;
}

void FLProgrogWatchdogTimer::pool()
{
    // Подтверждаем активность без увеличения переполняющегося счётчика.
    _loopSeen = 1;
}

void FLProgrogWatchdogTimer::tick()
{
    if (_loopSeen != 0)
    {
        _loopSeen = 0;
        _tickCounter = 0;
        return;
    }

    // Порог измеряется количеством прошедших «пустых» аппаратных тиков.
    // В отличие от исходной проверки !=, срабатываем ровно на N-м тике.
    ++_tickCounter;
    if (_tickCounter < _tickValue)
    {
        return;
    }

    _tickCounter = 0;
    execureRestart();
}

void FLProgrogWatchdogTimer::execureRestart()
{
    if (_callBack != 0)
    {
        _callBack();
    }

#ifdef RT_HW_CORE_AVR
    // Исторический способ «перезапуска» AVR — переход к нулевому адресу.
    // Это не полноценный аппаратный reset (периферия может сохранить состояние).
    asm volatile("jmp 0x0000");
#endif
#ifdef RT_HW_CORE_ESP32
    esp_restart();
#endif
#ifdef RT_HW_CORE_ESP8266
    ESP.reset();
#endif
#ifdef RT_HW_CORE_RP2040
    // AIRCR: ключ VECTKEY=0x05FA и запрос SYSRESETREQ в бите 2.
    AIRCR_Register = 0x05FA0004UL;
#endif
#ifdef RT_HW_CORE_STM32
    NVIC_SystemReset();
#endif
}
