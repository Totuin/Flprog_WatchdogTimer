#pragma once
#include "flprogUtilites.h"

#ifdef RT_HW_CORE_RP2040
#include <hardware/regs/m0plus.h>
#include <hardware/regs/addressmap.h>
#define AIRCR_Register (*((volatile uint32_t *)(PPB_BASE + 0x0ED0C)))
#endif

typedef void (*FLProgrogWatchdogTimerEvent)();

class FLProgrogWatchdogTimer
{
public:
    FLProgrogWatchdogTimer(uint8_t checkPeriod);

    void pool();
    void tick();
    void setCheckPeriod(uint8_t checkPeriod);
    void setCallBack(FLProgrogWatchdogTimerEvent func) { _callBack = func; };
    void setTickPeriod(uint32_t tickPeriod);

private:
    void calculateTickValue();
    void execureRestart();

    FLProgrogWatchdogTimerEvent _callBack = 0;

    uint8_t _checkPeriod = 10;
    uint16_t _checkCounter = 0;
    uint16_t _oldCheckCounter = 0;

    uint32_t _tickCounter = 0;
    uint32_t _tickValue = 1000000;
    uint32_t _tickPeriod = 10;
};

/*
//On Arduino you can just include SPI.h
//as it will pull the three headers below
//#include <SPI.h>
#include <hardware/regs/m0plus.h>
#include <hardware/regs/addressmap.h>
#include <hardware/resets.h>


inline void reboot() {
    //reset USB controller
    reset_block(RESETS_WDSEL_USBCTRL_BITS);
    //you might want to add other perpherials here if you use them

    //reset the CPU
    auto & AIRCR_register = *(volatile uint32_t*)(PPB_BASE + M0PLUS_AIRCR_OFFSET);
    //From datasheet:
    //31:16 VECTKEY: On writes, write 0x05FA to VECTKEY, otherwise the write is ignored.
    //15 ENDIANESS: 0 = Little-endian.
    //14:3 Reserved
    //2 SYSRESETREQ: Writing 1 to this bit causes the SYSRESETREQ signal to the outer system to be asserted to request a reset.
    //1 VECTCLRACTIVE: not relevant here
    AIRCR_register = (0x05FA << M0PLUS_AIRCR_VECTKEY_LSB) | M0PLUS_AIRCR_SYSRESETREQ_BITS;
}
    */