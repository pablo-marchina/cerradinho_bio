# Relatórios da Sprint 3: Índice

> **Para que serve:** porta de entrada para os relatórios de pesquisa, teste e verificação da Sprint 3.
>
> **Como usar:** ache a seção que você está escrevendo na coluna Alimenta e leia esses relatórios. Relatórios da sprint anterior: [Sprint 1](../Sprint01/README.md); a Sprint 2 não tem índice.
>
> **Lacunas de propósito:** "Não informado" significa que o relatório não traz autor ou issue; chutar criaria um mapeamento errado.

## Índice

| Relatório | Autor | Issue de origem | Status | Alimenta | Implicação |
|---|---|---|---|---|---|
| [Issue #122: walking skeleton MQTT](issue-122-walking-skeleton-mqtt.md) | Arthur Loyola | #122 | Testado na bancada com o ESP32 avulso contra o EMQX Cloud (TLS 8883): `status` e `online` retidos, `cmd` recebido, Last Will em cerca de 47 s e "MQTT ok" no LCD; EMQX 5.8.9 local no Docker testado (Last Will em 7,5 s com keep-alive de 5 s). Falta o teste com o protótipo completo, `evento`, `cmd/resposta` e o WebSocket com mqtt.js. | **3.4.3 Contrato MQTT e dashboard** (Artefato 8) e Art. 10 | O caminho firmware, broker e cliente está provado no broker escolhido (EMQX Cloud), então dashboard e demais fatias já podem usar tópicos reais; o Cloud exige TLS, login e assinar `guaratracking/#`, e o broker local serve só ao desenvolvimento. |
