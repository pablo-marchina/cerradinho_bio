#pragma once
#include <cstdint>
struct FakeLocation {
 bool isValid()const{return false;}
 bool isUpdated()const{return false;}
 unsigned long age()const{return 5000;}
 double lat()const{return 0.0;}
 double lng()const{return 0.0;}
};
struct FakeSatellite {bool isValid()const{return false;} unsigned value()const{return 0;}};
class TinyGPSPlus {
public:
 FakeLocation location;
 FakeSatellite satellites;
 bool encode(char){return false;}
 unsigned long charsProcessed() const{return 0;}
};
