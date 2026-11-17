# Casos de teste da 3.4.3: roteiros de execução

> **Pergunta investigada:** os comandos MQTT e os fluxos físicos associados passam no protótipo de 3 botões, contra o EMQX Cloud, com evidência completa?
> **O que foi feito:** roteiro passo a passo de cada caso, com os comandos exatos, o resultado esperado, o nome de cada evidência e um quadro de resultado.
> **Evidência:** a preencher em cada arquivo; as imagens e os logs vão para `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`.
> **Em aberto:** CT84 e CT85 ainda exigem execução e gravação no protótipo físico; os resultados anteriores permanecem registrados na documentação.
> **Alimenta:** **3.4.3 Protótipo do Projeto com MQTT e I2C** (casos de teste e resultados).
> **Implicação:** sem esta execução os CTs da 3.4.3 ficam só como ensaio de bancada, e o RF009 e o RF010 não podem sair de "Planejado" na RTM (#126).

Comece por [`00-preparacao.md`](00-preparacao.md).

## Ordem de execução

A numeração dos CTs não é a ordem de execução: o CT73 corta a energia e vai por último, o CT71 reaproveita o roteiro do CT75 e os CT83 a CT85 mudam o ciclo e o lacre, por isso exigem o estado inicial indicado no próprio roteiro.

| Ordem | CT | Roteiro | Card | Feito |
|---|---|---|---|---|
| 1 | CT70 | [Conexão e `status` retido](ct70-conexao-e-status.md) | #123 | [ ] |
| 2 | CT74 | [Comandos `avancar` e `voltar`](ct74-comandos-de-tela.md) | #128 | [ ] |
| 3 | CT75 | [Lacre aberto e fechado por comando](ct75-lacre-por-comando.md) | #128 | [ ] |
| 4 | CT71 | [Violação do lacre publicada no `evento`](ct71-violacao-do-lacre.md) | #123 | [ ] |
| 5 | CT76 | [Comandos recusados com motivo](ct76-comandos-recusados.md) | #128 | [ ] |
| 6 | CT78 | [Dispositivo desligado: só `ligar` é aceito](ct78-dispositivo-desligado.md) | #128 | [ ] |
| 7 | CT83 | [NF por comando (remessa e retorno)](ct83-nf-por-comando.md) | #128 | [ ] |
| 8 | CT77 | [Rede simulada desligada por tempo](ct77-rede-desligada-por-tempo.md) | #128 | [ ] |
| 9 | CT72 | [Fila sem rede de verdade (RN09)](ct72-fila-sem-rede.md) | #124 | [ ] |
| 10 | CT73 | [Queda de energia e Last Will](ct73-queda-de-energia.md) | #124 | [ ] |
| 11 | CT84 | [Abrir e encerrar ciclo pelo dashboard](ct84-ciclo-dashboard.md) | #129 | [ ] |
| 12 | CT85 | [Autorizar retirada pelo dashboard](ct85-autorizacao-dashboard.md) | #129 | [ ] |

## Regras do roteiro
- Cada arquivo tem um quadro **Resultado** para preencher na hora e uma lista de evidências com o nome exato do arquivo.
- Ao terminar um CT, atualize a linha dele na seção Resultados da 3.4.3 e retire o aviso "IMAGEM A INCLUIR" das figuras salvas.
- Estes roteiros são controle interno: o texto da `documentacao.md` não os cita.
