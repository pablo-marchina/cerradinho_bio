#pragma once
#include <cstdint>
struct FakeWire { void begin(int, int) {} void setClock(uint32_t) {} };
inline FakeWire Wire;
