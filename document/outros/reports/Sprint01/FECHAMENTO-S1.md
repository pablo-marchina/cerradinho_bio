# Fechamento de Sprint 1 — estado fiel e critérios de aceite

> **Fonte de verdade para esta entrega:** `document/documentacao.md` e `document/outros/evidencias_externas_sprint1.json`, confrontados com `src/firmware/main.cpp`. Histórico de versões e testes anteriores permanece em `assets/wokwi/`. Este documento substitui checklists históricos que marcavam patches experimentais não integrados como concluídos.

## Escopo demonstrado e histórico de integração

- O status de integração deve ser conferido no GitLab no commit avaliado; registros de revisões anteriores não provam o estado atual.
- Firmware integrado: `IndicatorController` usada em `setup()`/`loop()`, regras de ciclo/lacre, fila local NVS, mensagens JSON na Serial e circuito ESP32 de botões/LEDs com GNSS customizado.
- O firmware desta linha possui captura ISR para lacre; snapshots alternados/CRC e ACK de servidor **não** estão implementados. O ensaio host da ISR não substitui o Wokwi.
- O código da branch registra falhas de gravação NVS e captura bordas do lacre; as correções devem ser verificadas em CI no commit integrado. A NVS continua **não transacional**.
- Builds anteriores não dispensam compilar a revisão final em `demo` e `weekly`.

## Critérios da rubrica

| Entregável | Conteúdo preparado | Falta antes de afirmar 100% |
|---|---|---|
| §1, §3.1.1–3.1.5 | Introdução, Porter/concorrência, SWOT, descrição, VPC, matriz de riscos; §3.1.x agora até 300 palavras | Validação de hipóteses com parceiro |
| §3.1.6 LGPD | Texto conciso e anexo com 13 itens, fontes e inventário | Aceite de controlador/operadores, retenção, bases, canal e DPO aplicável |
| §3.1.7 BOM | Planilha oficial com 16 itens e R$ 733,99 em materiais indicativos; anexo com mão de obra | SKUs, estoque, frete, tributos, contratos/planos e orçamento aprovado |
| §3.2 personas | 3 cards e explicação do uso de IA; Clarice corrigida | Validação de pressupostos com pessoas reais |
| §3.3 RTM | 29 RF e 20 RN rastreados; CTs futuros registrados | Manter distinto o que é demonstrado e o que ainda é planejado |
| §3.4.1 Wokwi | Firmware com `setup`, `loop`, classe própria, LEDs, botões e registros históricos | **CT18 e CT22 reprovados**, CT09 sem ACK servidor; publicar projeto integrado e produzir evidências novas |

## Estado de CTs críticos — NÃO ALTERAR RETROATIVAMENTE

- **CT18:** reprovado no ensaio original sem rede: 16/20 transições estáveis registradas. A captura ISR e debounce registraram 20/20 no teste host, **mas isso não prova a correção no Wokwi**. Investigar e repetir o estímulo original antes de afirmar aprovação.
- **CT22:** reprovado no ambiente: reinício não satisfez oráculo de `uptime_ms`. Testar reset controlado e watchdog com logs da mesma versão; a correção do sinalizador de persistência é somente uma proteção parcial.
- **CT09:** bloqueado para confirmação externa; Serial e fila vazia não provam recebimento por servidor. MQTT não é requisito de implementação produtiva na Sprint 1, mas o critério existente exige delimitação/aceite da banca.

## Roteiro para completar aceite

1. Na branch sob revisão, executar `bash scripts/verify-sprint1-local.sh`, `bash scripts/check-firmware-syntax.sh` e `python3 scripts/audit_sprint1.py --strict-local`.
2. CI oficial: `pio run -e demo`, `pio run -e weekly`, construir GPS customizado e simular no Wokwi. Assinar evidência do commit/ambiente/binário.
3. Reexecutar CT18/CT22 com **os mesmos estímulos e esperados originais**, antes de quaisquer novas condições; manter logs históricos imutáveis. CT09 exige receptor com confirmação observável ou acordo escrito de escopo.
4. Sincronizar projeto Wokwi público com o commit apresentado; verificar acesso anônimo e URL. Confirmar aceitação de GitLab institucional diante da rubrica que menciona GitHub.
5. Aprovar dados jurídicos institucionais e compras; só depois marcar evidências externas no manifesto. Executar auditoria `--strict` somente quando ambas tiverem realmente sido satisfeitas.
6. Solicitar MR para `main`; confirmar sucesso do CI no **commit de entrega**, revisar documentos e manter anexos auditáveis.

**Regra de integridade:** passar em teste de host não equivale a CT aprovado no Wokwi; evidência externamente indisponível continua pendente.

## Registro de testes e condições de conclusão

| Caso | Evidência histórica | Resultado atual a demonstrar |
|---|---|---|
| CT18 — 20 transições de lacre sem rede | 16/20, **reprovado** | 20/20 em repetição Wokwi, sem modificar estímulos |
| CT22 — NVS/reset | **Reprovado** no cenário original | Restaurar estado e confirmar reinício com logs |
| CT09 — entrega externa | **Bloqueado** para ACK de servidor | Comprovação externa ou escopo formalmente aceito |
| Testes host ISR/NVS | Exercícios locais em dublês | Aprovação em CI no commit final; não equivale a Wokwi |

### Evidências exigidas para encerrar

- Registrar SHA do commit, ambiente, versão do firmware e logs de cada CT.
- Compilar firmware ESP32 nos modos `demo` e `weekly` e executar o circuito integrado.
- Não alterar resultados históricos nem declarar homologação LGPD antes do aceite.
