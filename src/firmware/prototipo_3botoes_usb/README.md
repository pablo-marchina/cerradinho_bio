# Diagrama de 3 botões, alimentado pelo USB (sem pilha)

É o **mesmo circuito** do `../prototipo_3botoes` (ESP32 DevKitC de 38 pinos, duas protoboards de 830 furos, mesmos pinos, mesmas pontes nos trilhos e mesmo ambiente `bancada-3botoes`). A diferença é só a fonte de energia: a placa é alimentada pelo **cabo USB da própria ESP32, ligado ao computador**, sem pilha nem fonte externa.

O **potenciômetro continua simulando o nível da bateria**, no GPIO34 (extremos em 3V3 e GND, cursor no GPIO34), como no `prototipo_3botoes`. Girar o potenciômetro muda a porcentagem que o firmware lê, e dá para testar o alerta de bateria baixa.

## Pinos

ON/OFF 19, AVANÇAR 25, VOLTAR 4, buzzer 13, LCD SDA 21 e SCL 22, potenciômetro no GPIO34, GNSS TX no VP (GPIO36) e RX no pino 17. A lista completa, com a posição de cada pino e cada jumper, está em `LIGACOES.md`.

## Limitações

- Sem lacre, rede, autorização e ciclo físicos, o firmware não gera esses eventos pelos botões: eles virão do dashboard por MQTT `cmd`.
- Os trilhos reais são divididos ao meio (interrupção na coluna 30): cada trilho usado tem uma ponte curta (furos 23 e 28).
