#include "flprogWatchdogTimer.h"
#include <cassert>
#include <stdint.h>
#include <type_traits>
#include <iostream>

static_assert(!std::is_convertible<int, FLProgrogWatchdogTimer>::value,
              "Constructor must not allow implicit conversion");

static int callbackCount = 0;
static void onRestart() { ++callbackCount; }

static void ticks(FLProgrogWatchdogTimer &watchdog, uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i) watchdog.tick();
}

int main()
{
    {
        // Последовательность ровных тиков: 1 секунда / 100 мс = 10 тиков.
        FLProgrogWatchdogTimer w(1);
        w.setTickPeriod(100000);
        w.setCallBack(onRestart);
        ticks(w, 9);
        assert(callbackCount == 0);
        w.tick();
        assert(callbackCount == 1);
        w.pool();
        w.tick(); // heartbeat consumes tick and resets timeout
        ticks(w, 9);
        assert(callbackCount == 1);
        w.tick();
        assert(callbackCount == 2);
    }
    {
        // Неполное деление: 1 000 000 / 600 000 = 1.66 => два тика.
        FLProgrogWatchdogTimer w(1);
        w.setTickPeriod(600000);
        w.setCallBack(onRestart);
        w.tick();
        assert(callbackCount == 2);
        w.tick();
        assert(callbackCount == 3);
    }
    {
        // Таймер может иметь период больше ожидаемого времени контроля.
        FLProgrogWatchdogTimer w(1);
        w.setTickPeriod(2000000);
        w.setCallBack(onRestart);
        w.tick();
        assert(callbackCount == 4);
    }
    {
        // Нулевой tickPeriod раньше вызывал деление на ноль.
        FLProgrogWatchdogTimer w(1);
        w.setTickPeriod(0);
        w.setTickPeriod(500000);
        w.setTickPeriod(0); // сохраняет предыдущий допустимый период
        w.setCallBack(onRestart);
        w.tick();
        assert(callbackCount == 4);
        w.tick();
        assert(callbackCount == 5);
    }
    {
        // При быстром loop() не должно быть ложного тайм-аута из-за wrap.
        FLProgrogWatchdogTimer w(1);
        w.setTickPeriod(1000000);
        w.setCallBack(onRestart);
        for (int i = 0; i < 65536; ++i) w.pool();
        w.tick();
        assert(callbackCount == 5);
        w.tick();
        assert(callbackCount == 6);
    }
    {
        // Нулевой checkPeriod игнорируется и не создаёт моментальный reset.
        FLProgrogWatchdogTimer w(1);
        w.setTickPeriod(1000000);
        w.setCallBack(onRestart);
        w.setCheckPeriod(0);
        w.tick();
        assert(callbackCount == 7);
    }
    {
        // Выбранный интервал 255 секунд не переполняет uint32_t.
        FLProgrogWatchdogTimer w(255);
        w.setTickPeriod(1000000);
        w.setCallBack(onRestart);
        ticks(w, 254);
        assert(callbackCount == 7);
        w.tick();
        assert(callbackCount == 8);
    }
    std::cout << "WATCHDOG_HOST_TESTS_PASS: timeout, rounding, zero, heartbeat, 255 sec, explicit\n";
}
