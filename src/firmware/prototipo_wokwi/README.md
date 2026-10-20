# Protótipo Wokwi: GuaráTracking (Sprint 1)

Simulação no Wokwi do dispositivo de rastreamento de ativos em manutenção externa (parceiro: Cerradinho Bioenergia). Na Sprint 1 não há rede nem broker reais: o envio MQTT é representado por JSON no Monitor Serial.

**Projeto público no Wokwi:** https://wokwi.com/projects/478276108359405569

## Arquivos

| Arquivo | Conteúdo |
|---|---|
| `diagram.json` | Circuito: ESP32 DevKit C v4, 4 LEDs com resistor de 220 Ω, 2 botões, 2 chaves deslizantes e 1 potenciômetro. |
| `sketch.ino` | Firmware. Nesta versão (issue #6) é só um teste de ligação: cada entrada acende um LED e as leituras aparecem no Serial. A lógica do protótipo entra com as issues #13 e #14. |

## Mapa de pinos

| Componente | Pino | Modo | Função no protótipo |
|---|---|---|---|
| LED verde | GPIO 25 | saída | Operando, lacre íntegro (piscando = retirada autorizada em curso) |
| LED amarelo | GPIO 26 | saída | Piscando = atraso; fixo = permanência acima do limiar |
| LED vermelho | GPIO 27 | saída | Violação do lacre |
| LED azul | GPIO 14 | saída | Sem rede, com eventos na fila |
| SW_LACRE (chave) | GPIO 32 | `INPUT_PULLUP` | Lacre fechado/aberto |
| SW_REDE (chave) | GPIO 18 | `INPUT_PULLUP` | Conexão disponível/indisponível (simulada) |
| BTN_AUTORIZAR (botão) | GPIO 33 | `INPUT_PULLUP` | Autoriza a retirada do lacre |
| BTN_CICLO (botão) | GPIO 13 | `INPUT_PULLUP` | Toque curto = novo vínculo; segurar 2 s = encerrar ciclo |
| POT_BATERIA (potenciômetro) | GPIO 34 (ADC1) | analógico | Nível de bateria simulado |

Botões e chaves ligam o pino ao GND e usam o pull-up interno: **LOW = acionado**.

## Como rodar

1. Abra o link público acima (não precisa de login) e clique em ▶.
2. Ou crie um projeto ESP32 novo no Wokwi e cole `diagram.json` e `sketch.ino` nas abas de mesmo nome.

## Evidências

- `assets/wokwi/circuito.png`: circuito com o Monitor Serial.

## Teste de ligação (issue #6)

Executado em 19/out/2026 no projeto público. Evidência: `assets/wokwi/circuito.png`.

| Ação | Esperado | Resultado |
|---|---|---|
| Nada acionado | LEDs apagados; `lacre=1 rede=1 autorizar=1 ciclo=1` | Passou |
| SW_LACRE acionada | LED verde aceso; `lacre=0` | Passou |
| SW_REDE acionada | LED azul aceso; `rede=0` | Passou |
| BTN_AUTORIZAR pressionado | LED amarelo aceso; `autorizar=0` | Passou |
| BTN_CICLO pressionado | LED vermelho aceso; `ciclo=0` | Passou |
| POT_BATERIA de um extremo ao outro | `bateria_adc` de 0 a ~4095 | Passou |
