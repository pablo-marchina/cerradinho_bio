# Relatórios da Sprint 3: Índice

> **Para que serve:** porta de entrada para os relatórios de pesquisa, teste e verificação da Sprint 3.
>
> **Como usar:** ache a seção que você está escrevendo na coluna Alimenta e leia esses relatórios. Relatórios da sprint anterior: [Sprint 1](../Sprint01/README.md); a Sprint 2 não tem índice.
>
> **Lacunas de propósito:** "Não informado" significa que o relatório não traz autor ou issue; chutar criaria um mapeamento errado.

## Índice

| Relatório | Autor | Issue de origem | Status | Alimenta | Implicação |
|---|---|---|---|---|---|
| [Issue #122: walking skeleton MQTT](issue-122-walking-skeleton-mqtt.md) | Arthur Loyola | #122 | Testado na bancada com o ESP32 avulso contra o EMQX Cloud (TLS 8883): `status` e `online` retidos, `cmd` recebido, Last Will em cerca de 47 s e "MQTT ok" no LCD; EMQX 5.8.9 local no Docker testado (Last Will em 7,5 s com keep-alive de 5 s). Foi um ensaio de bancada sem fotos oficiais: os CT70 a CT75 e os casos de falha ficam descritos no report para gravar com o protótipo completo. Falta também `evento`, `cmd/resposta` e o WebSocket com mqtt.js. | **3.4.3 Contrato MQTT e dashboard** (Artefato 8) e Art. 10 | O caminho firmware, broker e cliente está provado no broker escolhido (EMQX Cloud), então dashboard e demais fatias já podem usar tópicos reais; o Cloud exige TLS, login e assinar `guaratracking/#`, e o broker local serve só ao desenvolvimento. |
| [Issue #141: validação do MQTT no protótipo físico](issue-141-validacao-mqtt-prototipo.md) | Luiz Cazelatto | #141 | Ensaio de 12/11 no protótipo completo (montagem alternada) contra o EMQX Cloud: CT70 aprovado (`online` e `status` retidos, "MQTT ok" no LCD); CT71 aprovado com e sem ciclo (`VIOLACAO_LACRE` com o mesmo `uptime_ms` 146531 na Serial e no Explorer); `cmd` recebido, mas vazio; CT73 com Last Will em cerca de 40 s. Faltam vídeo do LCD e do buzzer, foto do "MQTT ok" e print do console; brownout em loop no boot do rádio, três vezes no dia. | **3.4.3 Protótipo do Projeto com MQTT e I2C** (Artefato 8): CT70, CT71 e CT73 | O alerta de violação chega ao broker no mesmo segundo da abertura do lacre, então o dashboard já pode usar `evento` e `status` reais; a alimentação pelo USB não sustenta o pico do rádio com o GNSS, e isso precisa ser resolvido antes de gravar os CTs oficiais. |
