#!/usr/bin/env bash
# Testes portaveis offline: nao substituem Wokwi/ESP32/PlatformIO.
set -euo pipefail
cd "$(dirname "$0")/.."
: "${CXX:=g++}"
: "${CC:=gcc}"
mkdir -p build
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -Iinclude tests/model_tests.c -lm -o build/model_tests_host
./build/model_tests_host
"$CXX" -std=c++17 -O2 -Wall -Wextra -Werror -Iinclude -Itests/stubs tests/indicator_controller_tests.cpp -o build/indicator_tests_host
./build/indicator_tests_host
"$CXX" -std=c++17 -O2 -Wall -Wextra -Werror -Iinclude tests/queue_snapshot_tests.cpp -o build/queue_snapshot_tests_host
./build/queue_snapshot_tests_host
"$CXX" -std=c++17 -O2 -Wall -Wextra -Werror -Iinclude tests/seal_debounce_tests.cpp -o build/seal_debounce_tests_host
./build/seal_debounce_tests_host
"$CXX" -std=c++17 -O2 -Wall -Wextra -Werror -Iinclude tests/transport_ack_tests.cpp -o build/transport_ack_tests_host
./build/transport_ack_tests_host
"$CXX" -std=c++17 -O1 -Wall -Wextra -Werror -Itests/firmware_stubs -Iinclude \
  tests/firmware_integration_tests.cpp -o build/firmware_integration_tests_host
./build/firmware_integration_tests_host
python3 tests/serial_ack_receiver_tests.py
python3 -m py_compile scripts/serial-ack-receiver.py scripts/export-wokwi.py scripts/audit_sprint1.py scripts/validate-project.py scripts/validate-release.py
python3 scripts/export-wokwi.py
printf '%s\n' 'PASS: testes HOST e exportacao Wokwi; nenhuma alegacao de simulacao remota.'
