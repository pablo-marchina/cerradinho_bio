#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Classe propria do prototipo V2: telas de status do LCD 16x2. Monta o texto
// das duas linhas a partir do estado calculado pelas regras de negocio, sem
// acessar o hardware (o main.cpp escreve no LCD), para poder ser testada em host.
// Nao faz esperas bloqueantes.

enum class Screen : uint8_t { Invoice, Deadline, Location, Battery, Seal, Connection };
constexpr uint8_t ScreenCount = 6;

enum class SealView : uint8_t { Intact, Violated, Authorized };

struct StatusView {
  const char *invoice;  // NF da remessa; vazio = nao informada
  bool cycleActive;
  uint32_t daysOut;
  uint32_t expectedDays;
  bool havePosition;      // ha posicao valida, nova ou restaurada da NVS
  bool positionAgeKnown;  // falso para o ponto restaurado apos reinicio
  uint32_t positionAgeS;
  int batteryPct;
  bool batteryLow;
  bool batterySensorFailed;
  SealView seal;
  bool sealOpen;
  bool linkUp;      // com MQTT: Wi-Fi e broker conectados; sem MQTT: a chave de sinal
  uint32_t queued;  // eventos guardados na fila sem conexao
};

class ScreenController final {
 public:
  static constexpr uint8_t Columns = 16;
  using Line = char[Columns + 1];

  // Avancar e voltar percorrem as seis telas em ordem circular; toda troca fecha a mensagem.
  void next() {
    index_ = (index_ + 1) % ScreenCount;
    messageMs_ = 0;
  }
  // Volta uma tela, em ordem circular, e fecha a mensagem.
  void previous() {
    index_ = (index_ + ScreenCount - 1) % ScreenCount;
    messageMs_ = 0;
  }
  // Abre uma tela especifica e fecha a mensagem.
  void show(Screen screen) {
    index_ = static_cast<uint8_t>(screen);
    messageMs_ = 0;
  }
  Screen current() const { return static_cast<Screen>(index_); }

  // Mensagem curta por cima da tela atual (resultado de um botao), por durationMs.
  void flash(const char *top, const char *bottom, uint32_t now, uint32_t durationMs) {
    copy(messageTop_, top);
    copy(messageBottom_, bottom);
    messageAt_ = now;
    messageMs_ = durationMs;
  }

  // Monta as duas linhas do LCD: a mensagem ativa ou a tela atual.
  void render(const StatusView &view, uint32_t now, Line top, Line bottom) const {
    if (messageMs_ > 0 && now - messageAt_ < messageMs_) {
      copy(top, messageTop_);
      copy(bottom, messageBottom_);
      return;
    }
    switch (current()) {
      case Screen::Invoice: renderInvoice(view, top, bottom); break;
      case Screen::Deadline: renderDeadline(view, top, bottom); break;
      case Screen::Location: renderLocation(view, top, bottom); break;
      case Screen::Battery: renderBattery(view, top, bottom); break;
      case Screen::Seal: renderSeal(view, top, bottom); break;
      case Screen::Connection: renderConnection(view, top, bottom); break;
    }
  }

  // Contagem enquanto ciclo ou autorizacao estao pressionados (exigem 3 s).
  static void renderHold(const char *action, uint32_t remainingMs, Line top, Line bottom) {
    copy(top, action);
    format(bottom, "segure mais %lu s",
             (unsigned long)((remainingMs + 999) / 1000));
  }

 private:
  uint8_t index_ = 0;
  Line messageTop_ = {};
  Line messageBottom_ = {};
  uint32_t messageAt_ = 0;
  uint32_t messageMs_ = 0;

  // Escreve na linha; o que passar de 16 colunas e cortado.
  static void format(Line target, const char *pattern, ...)
      __attribute__((format(printf, 2, 3))) {
    va_list args;
    va_start(args, pattern);
    vsnprintf(target, Columns + 1, pattern, args);
    va_end(args);
  }

  static void copy(Line target, const char *text) { format(target, "%s", text ? text : ""); }

  // Tela da NF da remessa.
  static void renderInvoice(const StatusView &view, Line top, Line bottom) {
    copy(top, "NF da remessa");
    copy(bottom, view.invoice && view.invoice[0] ? view.invoice : "nao informada");
  }

  // "dia 12/30" em vez de "35/30" no atraso: a segunda linha diz quanto falta ou excedeu.
  static void renderDeadline(const StatusView &view, Line top, Line bottom) {
    if (!view.cycleActive) {
      copy(top, "Prazo");
      copy(bottom, "sem ciclo ativo");
      return;
    }
    char text[64];
    snprintf(text, sizeof(text), "Prazo: dia %lu/%lu", (unsigned long)view.daysOut,
             (unsigned long)view.expectedDays);
    if (strlen(text) > Columns) {
      snprintf(text, sizeof(text), "Dia %lu/%lu", (unsigned long)view.daysOut,
               (unsigned long)view.expectedDays);
    }
    copy(top, text);
    if (view.daysOut > view.expectedDays) {
      const unsigned long late = view.daysOut - view.expectedDays;
      format(bottom, "ATRASO: %lu dia%s", late, late == 1 ? "" : "s");
    } else if (view.daysOut == view.expectedDays) {
      copy(bottom, "vence hoje");
    } else {
      const unsigned long left = view.expectedDays - view.daysOut;
      format(bottom, left == 1 ? "Falta %lu dia" : "Faltam %lu dias", left);
    }
  }

  // Ultima posicao valida e ha quanto tempo foi obtida, sem coordenadas no visor.
  static void renderLocation(const StatusView &view, Line top, Line bottom) {
    copy(top, "Ultima posicao");
    if (!view.havePosition) {
      copy(bottom, "sem fix ainda");
    } else if (!view.positionAgeKnown) {
      copy(bottom, "salva, sem idade");
    } else {
      const unsigned long age = view.positionAgeS;
      if (age < 60) format(bottom, "ha %lu s", age);
      else if (age < 3600) format(bottom, "ha %lu min", age / 60);
      else if (age < 86400) format(bottom, "ha %lu h", age / 3600);
      else format(bottom, "ha %lu dias", age / 86400);
    }
  }

  // Tela de bateria: porcentagem e estado, ou falha do sensor.
  static void renderBattery(const StatusView &view, Line top, Line bottom) {
    if (view.batterySensorFailed) {
      copy(top, "Bateria");
      copy(bottom, "sensor com falha");
      return;
    }
    format(top, "Bateria: %d%%", view.batteryPct);
    copy(bottom, view.batteryLow ? "BAIXA" : "normal");
  }

  // Tela da conexao: estado do MQTT e eventos guardados na fila (RN09).
  static void renderConnection(const StatusView &view, Line top, Line bottom) {
    copy(top, view.linkUp ? "MQTT: conectado" : "MQTT: sem sinal");
    format(bottom, "Fila: %lu evento%s", (unsigned long)view.queued, view.queued == 1 ? "" : "s");
  }

  // Tela do lacre: violado, autorizado ou integro.
  static void renderSeal(const StatusView &view, Line top, Line bottom) {
    switch (view.seal) {
      case SealView::Violated:
        copy(top, "Lacre VIOLADO");
        copy(bottom, "aberto sem autor");
        break;
      case SealView::Authorized:
        copy(top, "Lacre AUTORIZADO");
        copy(bottom, view.sealOpen ? "aberto p/ reparo" : "pode ser aberto");
        break;
      case SealView::Intact:
        copy(top, "Lacre INTEGRO");
        copy(bottom, "fechado");
        break;
    }
  }
};
