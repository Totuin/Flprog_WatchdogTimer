#!/usr/bin/env bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/.." && pwd)"
BIN="$(mktemp)"
trap 'rm -f "$BIN"' EXIT
c++ -std=c++11 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer \
  -I"$HERE/mocks" -I"$ROOT/src" \
  "$ROOT/src/flprogWatchdogTimer.cpp" "$HERE/test_watchdog.cpp" -o "$BIN"
"$BIN"
c++ -std=c++11 -Wall -Wextra -Wpedantic -Werror -I"$HERE/mocks" -I"$ROOT/src" \
  -x c++ -fsyntax-only "$ROOT/examples/FLProgWatchdogTimer/FLProgWatchdogTimer.ino"
echo WATCHDOG_EXAMPLE_SYNTAX_PASS
for CORE in AVR ESP32 ESP8266 RP2040 STM32; do
  c++ -std=c++11 -Wall -Wextra -Wpedantic -Werror -D"RT_HW_CORE_${CORE}" \
    -I"$HERE/mocks" -I"$ROOT/src" -fsyntax-only "$ROOT/src/flprogWatchdogTimer.cpp"
  echo "WATCHDOG_${CORE}_BRANCH_SYNTAX_PASS"
done
