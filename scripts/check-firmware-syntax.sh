#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
: "${CXX:=g++}"
for mode in 1 0; do
  "$CXX" -std=c++17 -fsyntax-only -Wall -Wextra -Werror -DDEMO_MODE="$mode" \
    -Itests/firmware_stubs -Iinclude src/firmware/main.cpp
  echo "PASS: compilacao sintatica host de main.cpp DEMO_MODE=$mode (nao ESP32)"
done
