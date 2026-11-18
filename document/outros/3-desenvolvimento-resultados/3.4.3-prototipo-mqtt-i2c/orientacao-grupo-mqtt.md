# GuaráTracking: como rodar e testar o MQTT no seu computador

Este guia é para quem vai gravar o firmware no ESP32 e ver as mensagens no broker (EMQX Cloud). Leva cerca de 20 a 30 minutos na primeira vez.

## 1. O que baixar e instalar

| O quê | Para quê |
|---|---|
| Git | clonar o repositório |
| VS Code + extensão **PlatformIO IDE** | compilar e gravar o firmware (ou o `pio` pela linha de comando) |
| Driver USB da placa (CP210x ou CH340, o que a sua placa usar) | o Windows só cria a porta COM com ele; se a placa já aparece em "Portas (COM e LPT)", não precisa |
| **MQTT Explorer** (https://mqtt-explorer.com) | ver as mensagens do broker e publicar testes |

## 2. O que separar

- ESP32 (DevKit), **cabo USB de dados, curto e bom** e uma porta USB direta do notebook (sem hub). Cabo ruim causa reinício em loop ("Brownout detector was triggered").
- LCD I2C: GND no GND, VCC no **5V/VIN**, SDA no **GPIO21**, SCL no **GPIO23** (montagem alternada, a pinagem padrão do firmware). Os demais pinos da montagem alternada: REDE 16, GNSS RX 36, VOLTAR 32, buzzer 12, AUTORIZAR 15, CICLO 5, AVANÇAR 25, lacre 27, bateria 34, ON/OFF 19. Os pinos 12, 5 e 15 são pinos de boot: não segure CICLO nem AUTORIZAR ao ligar, e nada pode puxar o GPIO12 para 3,3 V. Os ambientes `demo` e `weekly` (Wokwi) continuam nos pinos antigos (SCL 22, REDE 18, GNSS RX 16, VOLTAR 26, buzzer 23, AUTORIZAR 33, CICLO 14).
- Uma rede Wi-Fi **2,4 GHz** com internet e sem login por navegador. O ESP32 **não funciona em 5 GHz**. O hotspot do celular costuma servir, e a rede do Inteli provavelmente não.
- A rede precisa deixar sair a **porta 8883** (MQTT com TLS). Se não conectar com tudo certo, troque de rede.

## 3. Pegar o código

1. Clone o repositório do projeto (g01) e abra a pasta no VS Code.
2. Use a branch de trabalho atual do MQTT (ou a `develop-Art8`/`develop` após o merge), conforme combinado no grupo.

## 4. Criar o seu `include/secrets.h`

1. Copie `include/secrets.example.h` para `include/secrets.h`. **Esse arquivo nunca vai para o Git** (já está no `.gitignore`).
2. Use o **modo 2 (EMQX Cloud)** do exemplo e preencha:
   - nome e senha do **Wi-Fi 2,4 GHz**;
   - endereço do broker, usuário e senha do EMQX, que o responsável pelo MQTT no grupo passa por **mensagem privada**. Nunca coloque no grupo, no repositório ou em print;
   - porta **8883** e `MQTT_TLS 1`.
3. **Cada pessoa precisa de nomes únicos**, senão duas placas se derrubam no mesmo broker. Descomente e ajuste:
   ```
   #define MQTT_CLIENT_ID "guaratracking-seunome"
   #define MQTT_DEVICE_ID "prototipo-001-seunome"
   ```
   O `MQTT_CLIENT_ID` é o "crachá" da conexão. O `MQTT_DEVICE_ID` é o nome que aparece nos tópicos (`guaratracking/prototipo-001-seunome/status`), e evita misturar os dados com os de outras placas.

## 5. Gravar e acompanhar

```
pio run -e bancada -t upload
pio device monitor -b 115200
```

Esperado no monitor: `[wifi] conectado` e `[mqtt] conectado`. No LCD aparece "MQTT ok" por 2 s e depois ele volta à tela da NF. A tela apaga sozinha após uns 15 s sem uso, e isso é normal.

## 6. Ver as mensagens (MQTT Explorer)

1. Nova conexão: **host** do EMQX, **porta 8883**, **TLS ligado**, **usuário e senha** do EMQX.
2. Em **Advanced**, remova as assinaturas `#` e `$SYS/#` (o EMQX recusa) e adicione `guaratracking/#`. Se usar um `MQTT_DEVICE_ID` próprio, pode assinar só `guaratracking/prototipo-001-seunome/#`.
3. Conecte. Devem aparecer `online` e `status`.
4. Para testar o envio ao ESP32: no painel **Publish**, tópico `guaratracking/<seu-device>/cmd`, texto `teste`. No monitor serial aparece `[mqtt] cmd recebido`.
5. Para testar o Last Will: tire a energia da placa por mais de 60 s, e `online` vira `offline`.

## 7. Problemas comuns

| Sintoma | Causa provável |
|---|---|
| A porta COM não aparece | cabo só de carga, ou falta o driver USB |
| Placa reinicia em loop (`Brownout detector`) | cabo ou porta USB fracos; troque o cabo, use a porta direta |
| Não conecta ao Wi-Fi | rede de 5 GHz, senha errada ou rede com login por navegador |
| Wi-Fi conecta, MQTT não | a rede bloqueia a porta 8883, ou usuário e senha do EMQX errados |
| Explorer conecta, mas a árvore fica vazia | ficou a assinatura `#` ou `$SYS/#`; troque por `guaratracking/#` |
| `online` e `offline` alternando sem parar | duas placas com o mesmo `MQTT_CLIENT_ID` |
| `status` não atualiza | a placa caiu; confira o monitor serial |

## 8. Regras do grupo

- **Nunca** commitar `include/secrets.h`, senhas ou prints com credenciais.
- Só um ESP32 por `MQTT_CLIENT_ID` e por `MQTT_DEVICE_ID`.
- O EMQX gratuito tem cota mensal; não deixe placas conectadas à toa.
- Se algo falhar, mande o trecho do monitor serial (sem senhas) e a rede que está usando.
