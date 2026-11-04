#include <cassert>
#include <cstdio>
#include <cstring>
#include "screen_controller.h"

static ScreenController::Line top, bottom;

static StatusView baseView() {
  return StatusView{"", true, 0, 5, false, false, 0, 80, false, false, SealView::Intact, false};
}

static void expect(const ScreenController &screens, const StatusView &view, uint32_t now,
                   const char *line1, const char *line2) {
  screens.render(view, now, top, bottom);
  if (std::strcmp(top, line1) != 0 || std::strcmp(bottom, line2) != 0) {
    std::printf("esperado [%s|%s], obtido [%s|%s]\n", line1, line2, top, bottom);
    assert(false);
  }
}

int main() {
  ScreenController screens;
  StatusView view = baseView();

  // Navegacao circular: NF, prazo, localizacao, bateria, lacre e de volta a NF.
  assert(screens.current() == Screen::Invoice);
  screens.show(Screen::Battery);  // potenciometro girado abre a tela de bateria
  assert(screens.current() == Screen::Battery);
  screens.show(Screen::Invoice);
  for (uint8_t i = 0; i < ScreenCount; ++i) screens.next();
  assert(screens.current() == Screen::Invoice);
  screens.previous();
  assert(screens.current() == Screen::Seal);
  screens.next();

  expect(screens, view, 0, "NF da remessa", "nao informada");
  view.invoice = "NF-000123";
  expect(screens, view, 0, "NF da remessa", "NF-000123");

  screens.next();
  view.daysOut = 2;
  expect(screens, view, 0, "Prazo: dia 2/5", "Faltam 3 dias");
  view.daysOut = 4;
  expect(screens, view, 0, "Prazo: dia 4/5", "Falta 1 dia");
  view.daysOut = 5;
  expect(screens, view, 0, "Prazo: dia 5/5", "vence hoje");
  view.daysOut = 6;
  expect(screens, view, 0, "Prazo: dia 6/5", "ATRASO: 1 dia");
  view.daysOut = 152; view.expectedDays = 150;
  expect(screens, view, 0, "Dia 152/150", "ATRASO: 2 dias");
  view.cycleActive = false;
  expect(screens, view, 0, "Prazo", "sem ciclo ativo");

  screens.next();
  expect(screens, view, 0, "Ultima posicao", "sem fix ainda");
  view.havePosition = true;
  expect(screens, view, 0, "Ultima posicao", "salva, sem idade");
  view.positionAgeKnown = true; view.positionAgeS = 42;
  expect(screens, view, 0, "Ultima posicao", "ha 42 s");
  view.positionAgeS = 7200;
  expect(screens, view, 0, "Ultima posicao", "ha 2 h");
  view.positionAgeS = 3 * 86400;
  expect(screens, view, 0, "Ultima posicao", "ha 3 dias");

  screens.next();
  expect(screens, view, 0, "Bateria: 80%", "normal");
  view.batteryPct = 15; view.batteryLow = true;
  expect(screens, view, 0, "Bateria: 15%", "BAIXA");
  view.batterySensorFailed = true;
  expect(screens, view, 0, "Bateria", "sensor com falha");

  screens.next();
  expect(screens, view, 0, "Lacre INTEGRO", "fechado");
  view.seal = SealView::Authorized;
  expect(screens, view, 0, "Lacre AUTORIZADO", "pode ser aberto");
  view.sealOpen = true;
  expect(screens, view, 0, "Lacre AUTORIZADO", "aberto p/ reparo");
  view.seal = SealView::Violated;
  expect(screens, view, 0, "Lacre VIOLADO", "aberto sem autor");

  // Mensagem de resultado cobre a tela por um tempo e some depois, ou ao navegar.
  screens.flash("Ciclo iniciado", "aguardando NF", 1000, 2000);
  expect(screens, view, 2999, "Ciclo iniciado", "aguardando NF");
  expect(screens, view, 3000, "Lacre VIOLADO", "aberto sem autor");
  screens.flash("Autorizacao ok", "vale 3 dias", 4000, 2000);
  screens.next();
  expect(screens, baseView(), 4100, "NF da remessa", "nao informada");

  // Contagem do pressionamento de 3 s, arredondada para cima.
  ScreenController::renderHold("Iniciar ciclo", 3000, top, bottom);
  assert(!std::strcmp(top, "Iniciar ciclo") && !std::strcmp(bottom, "segure mais 3 s"));
  ScreenController::renderHold("Iniciar ciclo", 1, top, bottom);
  assert(!std::strcmp(bottom, "segure mais 1 s"));

  // Nenhuma linha passa de 16 colunas.
  view.invoice = "1234567890123456789";
  screens.render(view, 0, top, bottom);
  assert(std::strlen(top) <= ScreenController::Columns && std::strlen(bottom) <= ScreenController::Columns);

  std::puts("PASS: classe Telas, navegacao circular, NF, prazo/atraso, posicao, bateria, lacre e 3 s");
}
