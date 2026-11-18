#pragma once
#define WIFI_STA 1
#define WL_CONNECTED 3

class WiFiClient {};

class FakeWiFi {
 public:
  int statusValue = WL_CONNECTED;
  void mode(int) {}
  void begin(const char *, const char *) {}
  int status() const { return statusValue; }
  void reconnect() {}
};

extern FakeWiFi WiFi;

