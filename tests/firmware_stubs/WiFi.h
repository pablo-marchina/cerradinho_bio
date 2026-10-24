#pragma once
#define WIFI_STA 1
#define WL_CONNECTED 3
class FakeWiFi {
 public:
 void mode(int){}
 void begin(const char*,const char*){}
 int status()const{return 0;}
 void reconnect(){}
};
extern FakeWiFi WiFi;
