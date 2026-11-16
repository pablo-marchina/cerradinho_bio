#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "WiFi.h"

class PubSubClient {
 public:
  using Callback = void (*)(char *, uint8_t *, unsigned int);

  explicit PubSubClient(WiFiClient &) {}
  bool connectedState = true;
  bool failNextPublish = false;
  std::vector<std::string> published;
  Callback callback = nullptr;

  void setServer(const char *, int) {}
  bool setBufferSize(uint16_t) { return true; }
  PubSubClient &setKeepAlive(uint16_t) { return *this; }
  void setCallback(Callback value) { callback = value; }
  bool connected() const { return connectedState; }
  void disconnect() { connectedState = false; }
  bool loop() { return true; }
  int state() const { return connectedState ? 0 : -1; }
  bool connect(const char *, const char *, const char *, const char *, uint8_t, bool, const char *) {
    connectedState = true;
    return true;
  }
  bool subscribe(const char *) { return true; }
  bool publish(const char *topic, const char *payload, bool) {
    if (failNextPublish) {
      failNextPublish = false;
      return false;
    }
    published.emplace_back(std::string(topic ? topic : "") + " " + (payload ? payload : ""));
    return true;
  }
  bool beginPublish(const char *, unsigned int, bool) { return true; }
  size_t write(const uint8_t *, size_t length) { return length; }
  bool endPublish() { return true; }
};

