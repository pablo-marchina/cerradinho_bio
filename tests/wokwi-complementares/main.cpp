// Instrumentação de ensaio da Sprint 1: lógica de produção incluída sem mudanças.
#include <TinyGPS++.h>
#include <WiFi.h>
#include <Preferences.h>
#define setup tracker_setup
#define loop tracker_loop
#include "../../src/firmware/main.cpp"
#undef setup
#undef loop

namespace {
String probeLine;
void inspectNvs(const char *label) {
  Serial.printf("[ensaio] NVS_BEGIN %s uptime_ms=%lu\r\n", label, millis());
  for (unsigned i = 0; i < 16; ++i) {
    const String key = "q" + String(i);
    Serial.printf("[ensaio] NVS %s=%s\r\n", key.c_str(),
                  storage.getString(key.c_str(), "<ausente>").c_str());
  }
  for (unsigned i = 0; i < 8; ++i) {
    const String key = "p" + String(i);
    Serial.printf("[ensaio] NVS %s=%s\r\n", key.c_str(),
                  storage.getString(key.c_str(), "<ausente>").c_str());
  }
  StoredContext ctx{};
  if (restoreContext(ctx)) {
    Serial.printf("[ensaio] NVS ctx ciclo=%lu dias=%lu ativo=%u lacre_aberto=%u clock_ms=%llu\r\n",
      (unsigned long)ctx.cycle.cycle_id, (unsigned long)ctx.cycle.days_out,
      ctx.cycleActive, ctx.sealOpen, (unsigned long long)ctx.clockMs);
  }
  StoredQueueMeta meta{};
  if (storage.getBytes("pmeta", &meta, sizeof(meta)) == sizeof(meta))
    Serial.printf("[ensaio] NVS pmeta count=%u lost=%lu resumo=%lu\r\n",
      meta.count, (unsigned long)meta.lost, (unsigned long)meta.overflowCount);
  double point[2]{};
  if (storage.getBytes("previous", point, sizeof(point)) == sizeof(point))
    Serial.printf("[ensaio] NVS previous=%.6f,%.6f\r\n", point[0], point[1]);
  Serial.println("[ensaio] NVS_END");
}
}

void setup() {
  tracker_setup();
  Serial1.begin(115200, SERIAL_8N1, 32, -1);
  Serial.println("[ensaio] Instrumentacao CT18-CT22 v1; GPIO32 recebe estimulos/sondas; firmware de negocio sem alteracao");
  inspectNvs("boot");
}

void loop() {
  while (Serial1.available()) {
    const char c = Serial1.read();
    if (c == '\n') {
      probeLine.trim();
      if (probeLine.startsWith("?NVS")) inspectNvs(probeLine.c_str());
      else if (probeLine == "?RESET") {
        Serial.println("[ensaio] ESP.restart solicitado; preservar flash/NVS");
        Serial.flush();
        ESP.restart();
      } else Serial.println("[ensaio] " + probeLine);
      probeLine = "";
    } else if (probeLine.length() < 240) probeLine += c;
  }
  tracker_loop();
}
