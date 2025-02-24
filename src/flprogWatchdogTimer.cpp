#include "flprogWatchdogTimer.h"

FLProgrogWatchdogTimer::FLProgrogWatchdogTimer(uint8_t checkPeriod)
{
    setCheckPeriod(checkPeriod);
}

void FLProgrogWatchdogTimer::setCheckPeriod(uint8_t checkPeriod)
{
    if (_checkPeriod == checkPeriod)
    {
        return;
    }
    _checkPeriod = checkPeriod;
    calculateTickValue();
}

void FLProgrogWatchdogTimer::setTickPeriod(uint32_t tickPeriod)
{
    if (_tickPeriod == tickPeriod)
    {
        return;
    }
    _tickPeriod = tickPeriod;
    calculateTickValue();
}

void FLProgrogWatchdogTimer::calculateTickValue()
{
    _tickValue = (uint32_t)(_checkPeriod * 1000000) / _tickPeriod;
}

void FLProgrogWatchdogTimer::pool()
{
    _checkCounter++;
}
void FLProgrogWatchdogTimer::tick()
{
    if (_oldCheckCounter != _checkCounter)
    {
        _oldCheckCounter = _checkCounter;
        _tickCounter = 0;
        return;
    }
    if (_tickCounter != _tickValue)
    {
        _tickCounter++;
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
    asm volatile("jmp 0x0000");
#endif
#ifdef RT_HW_CORE_ESP32
    esp_restart();
#endif
#ifdef RT_HW_CORE_ESP8266
    ESP.reset();
#endif
#ifdef RT_HW_CORE_RP2040
    AIRCR_Register = 0x5FA0004;
#endif
#ifdef RT_HW_CORE_STM32
    NVIC_SystemReset();
#endif
}
