#pragma once
// Минимальная заглушка зависимости для автономных проверок логики watchdog.
#include <stdint.h>
#ifdef RT_HW_CORE_ESP32
inline void esp_restart() {}
#endif
#ifdef RT_HW_CORE_ESP8266
struct MockESP8266 { void reset(); };
extern MockESP8266 ESP;
#endif
#ifdef RT_HW_CORE_STM32
inline void NVIC_SystemReset() {}
#endif
