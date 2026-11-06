#pragma once
#include <cstdint>
struct FakeWire { void setClock(uint32_t) {} };
inline FakeWire Wire;
