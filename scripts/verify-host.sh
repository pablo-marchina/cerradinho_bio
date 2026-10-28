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
python3 -m py_compile scripts/export-wokwi.py scripts/audit-docs.py scripts/validate-project.py
python3 scripts/export-wokwi.py
printf '%s\n' 'PASS: testes HOST e exportacao Wokwi; nenhuma alegacao de simulacao remota.'
