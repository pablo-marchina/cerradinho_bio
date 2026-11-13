# Diagrama de 3 botões, sem bateria (USB)

Igual ao `../prototipo_3botoes` (ESP32 DevKitC de 38 pinos, duas protoboards de 830 furos, mesmos pinos e ambiente `bancada-3botoes`), mas **sem o potenciômetro que simulava a bateria**. A placa é alimentada pelo cabo USB do computador.

## O que muda

- Sem potenciômetro e sem os fios dele.
- **GPIO34 fica ligado ao 3V3** (jumper vermelho até o trilho de cima, nível de bateria 100%). Sem essa ligação o pino ficaria solto, e o firmware leria valores aleatórios e poderia emitir `BATERIA_BAIXA` ou `SENSOR_BATERIA_FALHA`. Se preferir o pino solto, retire só esse jumper. O GPIO34 é só de entrada, e 3,3 V é o máximo que ele aceita.
- O resto do mapa de pinos é o do `prototipo_3botoes`: ON/OFF 19, AVANÇAR 25, VOLTAR 4, buzzer 13, LCD SDA 21 e SCL 22, GNSS TX no VP (GPIO36) e RX no pino 17. A lista completa está em `LIGACOES.md`.

## Limitações

- Sem lacre, rede, autorização e ciclo físicos, o firmware atual não gera esses eventos: eles virão do dashboard por MQTT `cmd` (ainda não implementado).
- Sem bateria, o firmware nunca verá o nível cair. Os alertas de bateria baixa não podem ser testados neste protótipo.
- Os trilhos reais são divididos ao meio (interrupção na coluna 30): cada trilho usado tem uma ponte curta (furos 23 e 28).
