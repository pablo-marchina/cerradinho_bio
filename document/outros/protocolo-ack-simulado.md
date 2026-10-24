# Protocolo de confirmação serial — bancada da Sprint 1

**Escopo:** demonstração local de uma fila com retenção até confirmação, não MQTT/HTTP, não servidor, não autenticação ou segurança. O identificador CRC32 de oito dígitos é somente um correlator de bancada; um ACK real precisaria de identificadores persistentes, autenticação, garantias de ordenação e desduplicação.

## Fluxo executável

1. No Wokwi integrado com firmware desta revisão, manter `SW_REDE` disponível e consultar `PENDENTES`. Os eventos produzidos entram na fila e são gravados em NVS *antes* de transmitir na Serial.
2. Quando aparecer `[TX_SIM] token=XXXXXXXX` seguido do JSON, **não digitar ACK** por pelo menos 3 s e verificar retransmissão do mesmo registro/token. Nenhuma entrada sai da fila por impressão na Serial.
3. Digitar `ACK 00000000` (token incorreto) e `PENDENTES`: esperar `[ACK_SIM] RECUSADO` e contagem inalterada.
4. Digitar `ACK XXXXXXXX` correspondente ao primeiro registro; a fila recua um evento e um novo `[TX_SIM]` identifica o próximo, se houver. Repetir o token antigo: não pode remover o próximo item.
5. Com evento aguardando, desligar `SW_REDE`, aguardar, ligar: a fila não deve ser apagada; observar retransmissão após restabelecimento.
6. Com `SW_REDE` desabilitada, gerar abertura de lacre, saída de ciclo e alertas; provocar fila cheia e validar a preservação prioritária de críticos e o **resumo agregado** quando exceder a capacidade. O resumo preserva quantidade e primeira/última ocorrência, **não payloads individuais**.
7. Desligar e ligar simulação com NVS persistida (quando suportado) e verificar se primeiro item volta; verificar console com `reset_reason` e log de snapshot. Aplicar injeção de escrita parcial nos testes host; queda abrupta real precisa do Wokwi ou hardware.

## Oráculos, riscos e critérios

| Caso | Estímulo | Esperado | Evidência exigida |
|---|---|---|---|
| CT-SIM-ACK-01 | Sem ACK, 3 s | Mesmo token retransmitido, `PENDENTES` constante | Logs Serial novos, data/commit |
| CT-SIM-ACK-02 | Token incorreto | RECUSADO, nenhum evento removido | Log completo |
| CT-SIM-ACK-03 | Token correto | Remove somente cabeça, persiste novo snapshot | Log e verificação após restart |
| CT-SIM-ACK-04 | Rede desabilitada | Sem transmissão durante OFF; reenvio após ON | Log com timestamp/chaves |
| CT-SIM-ACK-05 | Evento critical com fila cheia | Preserva eventos críticos ou agrega excedentes | Log e contagens |
| CT-SIM-ACK-06 | NVS falha | Não declarar evento persistido; possível repetição após reboot | Injeção e logs |

**Estado dos seis novos cenários:** não executados em Wokwi nesta sessão. `tests/transport_ack_tests.cpp` valida fila/token, e `tests/firmware_integration_tests.cpp` exercita o parser de comandos e a fila do firmware real em host com dublês de hardware. Não substituir CT09 por esses testes: CT09 pede confirmação externa real e permanece bloqueado até haver receptor independente ou aceite formal de escopo.

**Limitações identificadas:** fila 8 registros; mensagens com comprimento limitado a 511 caracteres por registro; ao inserir com todos os slots críticos, o resumo preserva contagem e timestamps extremos; ACK perdido pode produzir duplicatas e o backend deverá deduplicar; falta medição de durabilidade NVS sob falha real de energia.

## Receptor externo de bancada opcional (24/10/2026)

Para diferenciar a entrega serial de uma confirmação independente, o repositório possui
`scripts/serial-ack-receiver.py`. O programa lê mensagens da porta Serial RFC2217,
valida o CRC do payload e escreve **o JSON completo** num banco SQLite local antes
de devolver `ACK XXXXXXXX`. Se a gravação falhar, não envia o ACK. Receber o mesmo
payload novamente não duplica a linha do banco e permite confirmar outra vez.

```bash
python -m pip install pyserial
# Com a extensão Wokwi rodando em modo demo e porta RFC2217 localhost:4000:
python scripts/serial-ack-receiver.py --seconds 180
# Dados: build/wokwi-ci/receiver.sqlite3
```

Validação local sem Wokwi: `python tests/serial_ack_receiver_tests.py` verifica gravação,
deduplicação, CRC inválido e persistência do banco após reabertura. **Este é somente
um receptor independente de bancada**: não há broker MQTT, TLS, autorização remota,
identificador criptográfico nem confirmação cloud; CT09 continua pendente de aceite
conforme oráculo original e ambiente definido pela disciplina.
