# Issue #131: integração física do dashboard com o ESP32

> **Pergunta investigada:** o dashboard em modo MQTT recebe o estado do ESP32 físico e envia comandos correlacionados pelo EMQX Cloud?
> **O que foi feito:** firmware gravado no protótipo de três botões, conexão ao hotspot e ao EMQX Cloud, leitura da bateria por potenciômetro, fluxo de ciclo e NF pelo dashboard, recusa de encerramento, queda de energia com Last Will e reconexão.
> **Resultado:** o caminho dashboard → broker → ESP32 → `cmd/resposta` → dashboard foi validado fisicamente. A telemetria de bateria, o estado retido e a presença `online`/`offline` também chegaram ao dashboard.
> **Em aberto:** abertura física do lacre dentro e fora da janela de autorização; coordenadas GNSS no MQTT; formato estruturado de `online`; persistência do histórico fora da memória do navegador.

Ensaio executado por Rachel em 18/11/2026. O firmware foi compilado no ambiente `bancada-3botoes` da branch `feat/#131/sprint3/integracao-dashboard`, a partir de `e9727ea5` com a alteração local que antecipa a publicação de `status` quando a bateria muda.

## Ambiente

| Item | Valor |
|---|---|
| Placa | ESP32 DevKit, MAC final `83:a0`, porta `COM14` |
| Firmware | PlatformIO, ambiente `bancada-3botoes`, `DEMO_MODE=1` |
| Identidade | `MQTT_DEVICE_ID=prototipo-001-rachel`; cliente `guaratracking-prototipo-001-rachel` |
| Rede | Hotspot Wi-Fi 2,4 GHz da executora |
| Broker | EMQX Cloud, TLS no dispositivo e WSS no dashboard/MQTTX |
| Dashboard | React/Vite em `http://127.0.0.1:5173`, fonte `mqtt`, origem `esp32` |
| Cliente de inspeção | MQTTX Web, assinatura exata de `guaratracking/prototipo-001-rachel/status` |

A senha do broker, a senha do hotspot e o SSID permaneceram somente nos arquivos locais ignorados pelo Git. O host e o nome do usuário MQTT aparecem nas capturas do console para identificar o deployment, mas nenhuma senha aparece nas evidências.

## Resultados

| Verificação | Situação | Observado | Evidência |
|---|---|---|---|
| Identidade e estado físico | APROVADO | O dashboard mostrou `prototipo-001-rachel`, modo “MQTT · ESP32 físico”, lacre íntegro, fila `0/8` e bateria recebida da placa. | [Dashboard](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/issue131DashboardEstadoFisico.png) |
| Potenciômetro → dashboard | APROVADO | O LCD e o dashboard mostraram `32%` no mesmo ensaio; após a correção, mudanças estabilizadas de pelo menos dois pontos marcam o `status` para publicação com intervalo mínimo de 1 s. | [Protótipo e dashboard](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/issue131PrototipoDashboardBateria.png) |
| Limiares da bateria | APROVADO, por observação | A sequência aproximada `19% → 21% → 26% → 19%` foi executada e o alerta/rearme se comportou conforme a regra de 20%/25%. Não foi produzida uma captura isolada dessa sequência. | Registro deste ensaio |
| Broker recebendo `status` | APROVADO | O MQTTX recebeu JSON real com `device_id`, `bateria_pct`, lacre, ciclo, NF, GNSS e fila no tópico `status`. | [MQTTX](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/issue131BrokerStatusMqttx.png) |
| Abrir ciclo | APROVADO | O dashboard publicou o comando e apresentou “Executado pelo ESP32” após a resposta correlacionada. | [Comando](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/issue131CmdAbrirCiclo.png) |
| NF de remessa | APROVADO | A NF foi aceita no ciclo 3 e o dashboard apresentou a confirmação do ESP32. | [Comando](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/issue131CmdNfRemessa.png) |
| Autorizar retirada | PARCIAL | O comando recebeu resposta positiva imediata. A abertura física do lacre dentro da janela acelerada de três segundos não foi executada e permanece planejada. | [Comando](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/issue131CmdAutorizar.png) |
| Encerrar sem NF de retorno | APROVADO, por observação | O firmware recusou o encerramento e o dashboard exibiu a recusa. Não foi produzida captura específica dessa resposta. | Registro deste ensaio |
| NF de retorno e encerramento | APROVADO, por observação | Depois da NF de retorno, o encerramento foi aceito e o dispositivo voltou a ficar sem ciclo ativo. | `status` final no MQTTX e registro deste ensaio |
| Queda de energia e LWT | APROVADO | Ao retirar o USB, o dashboard mostrou “Conexão perdida (LWT)”; depois de religar, voltou a “Conectado ao broker”. | [Offline](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/issue131DashboardOfflineLwt.png) · [Reconexão](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/issue131DashboardOnlineReconexao.png) |
| Último contato após reconexão | APROVADO, com ressalva | Como `online` ainda é texto simples e não leva `reported_at`, o campo ficou sem horário após receber apenas mensagens retidas. Uma nova mudança do potenciômetro publicou `status` ao vivo e preencheu o último contato. | [Dashboard após novo status](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/issue131DashboardEstadoFisico.png) |

## Fluxo comprovado

```text
dashboard --WSS--> EMQX Cloud --TLS--> ESP32
dashboard <--WSS-- EMQX Cloud <--TLS-- ESP32 (`cmd/resposta` e `status`)
```

Os horários apresentados pelo dashboard comprovaram a criação, a publicação no broker e a confirmação pelo ESP32 como etapas distintas. Uma resposta positiva produziu “Executado pelo ESP32”; o encerramento sem NF de retorno produziu recusa e não foi apresentado como sucesso.

## Limites e pendências

- O lacre físico foi deliberadamente mantido como teste planejado. O aceite do comando `autorizar` não comprova `RETIRADA_AUTORIZADA`, expiração da janela, buzzer ou abertura física.
- O firmware publica `gnss_fix`, mas ainda não publica coordenadas no MQTT; o mapa não é evidência de GNSS físico.
- `online`/`offline` continuam como texto simples. O objeto com `state`, `session_id` e `reported_at` permanece evolução futura.
- O modo de demonstração usa 1 s como 1 dia; o prazo de cinco dias estoura em cerca de cinco segundos. Esse comportamento acelerado foi esperado no ensaio.
- O estado MQTT do dashboard permanece na memória do navegador. Persistência, autenticação de usuários, histórico e regras administrativas dependem de backend.
- Não foram executados neste ensaio os casos de JSON malformado/chave repetida, encerramento com lacre aberto nem a compatibilidade com o botão físico de ciclo previstos no roteiro completo do CT84.

