# Fechamento da Sprint 1 — checklist de entrega

> **Nota (24/10/2026):** este registro descreve a versão `fe2ed7c` da revisão geral. Na entrega, ficou só a classe `IndicatorController`; a captura do lacre por ISR, o ACK manual pela Serial e os snapshots alternados foram adiados para a Sprint 2, porque exigem rodar de novo todos os CTs no Wokwi.


**Base:** `a70d1d6` (`origin/develop` exportada 23/10/2026). As modificações posteriores estão no patch, não na versão pública original.

- [x] Firmware com classe própria de LEDs e duas funções auxiliares em `setup`/`loop` (alteração do código).
- [x] Persistência de fila alterada para snapshots alternados com CRC; teste de falha **de lógica em host**, não queda de energia ESP32.
- [x] Captura de bordas por ISR e debounce temporal propostos para CT18; **não comprovado no Wokwi**.
- [x] Saída Serial classificada como simulação, sem ACK real.
- [x] Auditoria de LGPD, BOM, RF/RN, VPC, personas e critérios de medida; campos não confirmados declarados.
- [ ] `pio run -e demo` e `pio run -e weekly` no ambiente oficial pós-patch.
- [ ] Compilação e simulação do GPS customizado pós-patch.
- [ ] CT18 e CT22 repetidos com evidências brutas e esperados originais — nunca sobrescrever logs antigos.
- [x] Implementar ACK **manual de bancada**, com teste host; [ ] CT09 continua bloqueado até ACK de receptor externo ou aceite formal do recorte de Sprint 1.
- [ ] Confirmar controlador/operador/DPO, provedor/região, bases, retenção e canal LGPD com parceiro.
- [x] Pesquisa adicional de preços públicos para ESP32, jumpers, resistores, LEDs, botões, chaves e potenciômetro, com fontes e ressalvas; [ ] permanecem 8 linhas sem preço, tributos/frete e orçamento homologado.
- [x] Novo card visual de Clarice incorporado, sem emissão de NF-e, sem prazos universais e sem portaria automática; confirmar versão com equipe/parceiro.
- [ ] Publicar Wokwi integrado e revisar se GitLab satisfaz a exigência de GitHub.
- [ ] Confirmar integrantes, docentes, fontes/valores e aprovação de uso de marca.
- [ ] Atualizar RTM com novos testes, evidências e resultados sem presumir aprovação.
- [ ] Fazer revisão cruzada, merge request para `main` e comparar conteúdo do commit de entrega.

**Regra:** patch proposto ≠ CT aprovado. Caso falhe, manter o caso reprovado e documentar o defeito com próxima ação.

**Pesquisa institucional complementar:** aviso público LGPD disponibiliza `lgpd@cerradinho.com.br`, mas a designação para o produto carece de homologação; consultar [inventário](inventario-privacidade.md).

## Ampliação técnica posterior — host integrado e CI

- [x] Exercício em host do arquivo `main.cpp` real com GPIO e NVS falsos (`tests/firmware_integration_tests.cpp`): 20 bordas por ISR sem rede, teste de ACK digitado via parser Serial, falha de gravação e reintegração de snapshot.
- [x] Pipeline GitLab preparada para compilar ESP32 `demo` e `weekly` com PlatformIO 6.2.0, além de *smoke test* Wokwi condicionado à variável protegida `WOKWI_CLI_TOKEN`.
- [ ] O job de Wokwi **não foi executado**; CLI e token indisponíveis neste ambiente, e o YAML é inicial/experimental até executar na plataforma. Não usar como CT18 ou CT22 aprovado.
- [x] Atualização de quatro preços de referência adicionais (LED, botão, chave, potenciômetro); a BOM ainda tem **8 itens sem cotação** e o subtotal segue incompleto.
- [x] `.gitignore` agora permite rastrear a BOM oficial de Sprint 1 sem liberar outras planilhas.

- [x] Receptor serial externo de bancada implementado com SQLite durável e ACK após commit, unitariamente validado; [ ] ensaio RFC2217/Wokwi ainda não executado; não substituir CT09 MQTT/servidor.

- [x] Protocolo reproduzível CT18/CT22/CT09 em `tests/wokwi-ci/PROTOCOLO_REGRESSAO.md`; [ ] evidências novas de execução em Wokwi ainda não coletadas.
