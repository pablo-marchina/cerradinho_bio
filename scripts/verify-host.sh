#!/usr/bin/env bash
# Testes portaveis offline: nao substituem Wokwi/ESP32/PlatformIO.
set -euo pipefail
cd "$(dirname "$0")/.."
: "${CXX:=g++}"
: "${CC:=gcc}"
mkdir -p build
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -Iinclude tests/model_tests.c -lm -o build/model_tests_host
./build/model_tests_host
"$CXX" -std=c++17 -O2 -Wall -Wextra -Werror -Iinclude tests/screen_controller_tests.cpp -o build/screen_tests_host
./build/screen_tests_host
"$CXX" -std=c++17 -O2 -Wall -Wextra -Werror -Iinclude -Itests/firmware_stubs tests/firmware_persistence_tests.cpp -o build/firmware_persistence_host
./build/firmware_persistence_host
"$CXX" -std=c++17 -O2 -Wall -Wextra -Werror -Iinclude -Itests/firmware_stubs tests/firmware_seal_isr_tests.cpp -o build/firmware_seal_isr_host
./build/firmware_seal_isr_host
"$CXX" -std=c++17 -O2 -Wall -Wextra -Werror -Iinclude -Itests/firmware_stubs tests/firmware_interface_tests.cpp -o build/firmware_interface_host
./build/firmware_interface_host
"$CXX" -std=c++17 -O2 -Wall -Wextra -Werror -Iinclude -Itests/firmware_stubs tests/firmware_buttons_tests.cpp -o build/firmware_buttons_host
./build/firmware_buttons_host
# Issue #113: exercitar entrada serial VINCULAR por loop/readCommands em demo e weekly.
for mode in 1 0; do
  "$CXX" -std=c++17 -O2 -Wall -Wextra -Werror -DDEMO_MODE="$mode" \
    -Iinclude -Itests/firmware_stubs tests/firmware_link_command_tests.cpp \
    -o "build/firmware_link_command_${mode}_host"
  "./build/firmware_link_command_${mode}_host"
done
python3 -m unittest discover -s tests/audit -p "test_*.py" -v
python3 -m py_compile scripts/export-wokwi.py scripts/audit-docs.py scripts/sprint1_evidence.py scripts/validate-project.py
python3 scripts/export-wokwi.py
printf '%s\n' 'PASS: testes HOST e exportacao Wokwi; nenhuma alegacao de simulacao remota.'
