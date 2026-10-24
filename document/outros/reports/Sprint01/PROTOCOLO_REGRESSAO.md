# Protocolo de regressão funcional — evidências pós-patch (CT18 / CT22 / CT09)

> **Nota (24/10/2026):** este registro descreve a versão `fe2ed7c` da revisão geral. Na entrega, ficou só a classe `IndicatorController`; a captura do lacre por ISR, o ACK manual pela Serial e os snapshots alternados foram adiados para a Sprint 2, porque exigem rodar de novo todos os CTs no Wokwi.


**Não executado em Wokwi nesta sessão.** Rodar somente após `pio run -e demo`, compilar o GPS custom e iniciar a simulação integrada. Guardar `commit`, ambiente, firmware `.bin`, hash do sketch, data, entradas e console integral. Não apagar os logs da revisão anterior.

## CT18 — 20 transições com rede simulada indisponível

- Inicializar NVS de um ensaio novo; ativar `SW_REDE` à posição OFF (o switch não corta o Wi-Fi físico).
- Com lacre inicialmente íntegro, gerar **20 alternâncias** entre aberto e fechado; manter cada posição **pelo menos 200 ms**, preservando o intervalo do caso original. Não usar pulsos curtos de bounce como transições válidas.
- Esperado: exatamente 20 `seal_changed` distintos com tempos `edge_at_ms`, 10 aberturas e 10 fechamentos, sem overflow `[lacre] ALERTA` da ISR. Resultado histórico **16/20 sem rede** permanece reprovado até nova evidência integral.
- Com fila limitada a 8, observar agregação dos excedentes críticos e diagnosticar eventos não críticos descartados. **20 mudanças de estado não significam 20 payloads críticos individuais preservados**.
- Guardar console completo, captura do circuito/chave, intervalo, contagem calculada e identificação do firmware.

## CT22 — reset real versus chave ON/OFF lógica

- Criar ciclo ativo, alterar `LIMIAR`, gerar evento crítico e consultar `PENDENTES` antes do reset.
- Anotar `reset_reason`, `uptime_ms`, geração dos slots e quantidade de pendentes. Fazer **reset real do ESP32 simulado**, pela ação apropriada do Wokwi, não pela chave `SW_ENERGIA`, que apenas desliga a lógica do dispositivo.
- Esperado: novo `[boot]`, `uptime_ms` reiniciado e menor que o valor observado antes do reset, `contexto_restaurado=true`, ciclo/limiar preservados, NVS restaurada e fila coerente. Isolar resets adicionais por watchdog.
- Repetir com corrupção controlada **apenas em ambiente de teste**: slot mais recente inválido deve recuperar o anterior; ambos inválidos devem acionar modo degradado sem sobrescrever os dados.
- Guardar logs completos antes/depois e screenshot do circuito. **Não declarar aprovado** somente porque os testes host de snapshot passaram.

## CT09 — confirmação independente do receptor

- Testar primeiro saída sem ACK e token errado: fila **não** pode esvaziar. Para confirmação externa de bancada, iniciar `scripts/serial-ack-receiver.py` no computador (via RFC2217) com banco SQLite vazio.
- Enviar evento com `SW_REDE` ON; verificar a ordem: `[TX_SIM]`, registro persistido no SQLite, depois `[ACK_SIM]`; a fila diminui somente após receber confirmação no firmware.
- Repetir após simular perda de ACK; o SQLite deve conter uma única cópia do mesmo JSON e o receptor pode responder novamente ao mesmo token.
- **Não confundir com MQTT/servidor real.** Se o CT09 exige recebimento no backend do parceiro, o caso continuará bloqueado até existir endpoint independente conforme contrato, ou aceite formal da banca para demonstração de bancada.

## Matriz de evidências obrigatórias

| Caso | Entradas | Oráculo | Evidência | Status atual |
|---|---|---|---|---|
| CT18 | 20 bordas de 200 ms, rede OFF | 20/20 e 0 overruns | Log completo + contagem + figura | Não retestado Wokwi |
| CT22 | reset real após persistir | nova inicialização, contexto/filas preservados | Log antes/depois, razão reset, NVS | Não retestado Wokwi |
| CT09 | rede OFF/ON, ACK ausente/inválido/válido | fila só remove após confirmação externa | Serial + DB + ações do operador | Receptor host pronto; não retestado Wokwi |

**Critério de atualização:** preencher data e hash de commit, anexar logs novos sem reescrever histórico e marcar PASS apenas se todas as verificações do caso forem observadas no mesmo ensaio.
