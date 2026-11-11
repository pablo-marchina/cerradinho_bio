# Relatórios da Sprint 3: Índice

> **Para que serve:** porta de entrada para os relatórios de pesquisa, teste e verificação da Sprint 3.
>
> **Como usar:** ache a seção que você está escrevendo na coluna Alimenta e leia esses relatórios. Relatórios da sprint anterior: [Sprint 1](../Sprint01/README.md); a Sprint 2 não tem índice.
>
> **Lacunas de propósito:** "Não informado" significa que o relatório não traz autor ou issue; chutar criaria um mapeamento errado.

## Índice

| Relatório | Autor | Issue de origem | Status | Alimenta | Implicação |
|---|---|---|---|---|---|
| [Issue #122: walking skeleton MQTT](issue-122-walking-skeleton-mqtt.md) | Arthur Loyola | #122 | Firmware compila nos ambientes `demo` (flash 62,3%), `weekly` e `bancada` com TLS (flash 78,7%), medido no computador; falta testar na placa e no broker, TLS com HiveMQ Cloud e os tópicos `evento` e `cmd/resposta`. | **3.4.3 Contrato MQTT e dashboard** (Artefato 8) e Art. 10 | O caminho firmware, broker e cliente está provado em código, então dashboard e demais fatias já podem usar tópicos reais; o broker local serve ao desenvolvimento e a operação na Cerradinho exige TLS e login (Cloud ou outro serviço, demanda futura). |
