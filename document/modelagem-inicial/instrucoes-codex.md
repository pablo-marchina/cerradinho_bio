# Instruções para o Codex continuar a implementação

## Pedido pronto para usar

Continue o projeto GuaráTracking no repositório `g01`. Leia primeiro `AGENT.md`,
`README.md`, `document/modelagem-inicial/modelagem-inicial.md` e
`document/modelagem-inicial/validacao.md`. Preserve a modelagem
inicial ESP32 + GPS/GNSS + sinal de lacre + bateria + conectividade. O requisito
confirmado é verificar mudança de lugar entre coletas semanais, sem acelerômetro
ou rastreamento contínuo. Use a simulação Wokwi com PlatformIO no VS Code.

Antes de alterar, examine os arquivos e os resultados de validação. Compile o
firmware e o GPS com os scripts existentes; investigue falhas com evidências.
Mantenha heartbeat serial e conexões UART0 do monitor, inclusive quando o GPS
estiver sem fix. Nunca marque como posição válida uma mensagem GNSS inválida.

Para evoluir do circuito inicial para o restante do projeto, trabalhe na etapa
solicitada pelo usuário, reaproveitando os contratos de telemetria existentes.
Documente as decisões em vez de assumir detalhes de hardware, rede ou TAPI que
ainda não foram definidos. Use dados fictícios. Não publicar documentos internos,
enviar dados para terceiros, instalar serviços pagos ou ativar licenças.

Ao concluir, informe quais arquivos mudou, quais verificações executou e quais
limitações continuam. Uma compilação bem-sucedida não prova execução da simulação
nem desempenho elétrico real.

## Próximas etapas, em ordem sugerida

1. Abrir o Wokwi e executar o roteiro visual: console, GPS válido e inválido,
   mudança de posição, lacre e bateria sintética. Registrar as evidências.
2. Definir se o lacre exige alerta imediato quando houver conexão; o TAPI prevê
   envio fora da rotina semanal e tratamento de retirada autorizada.
3. Escolher e testar o sensor óptico real. Ajustar polaridade e debounce após
   medir o comportamento da montagem, sem tratar a chave como validação física.
4. Avaliar conectividade nas condições externas. Escolher modem/tecnologia só
   depois de comparar cobertura, consumo, custos e antenas.
5. Definir transporte autenticado para um endpoint do próprio projeto e relógio
   UTC. Acrescentar confirmação, reenvio, identificação e deduplicação dos eventos.
   A fila atual preserva somente 16 registros e não confirma entrega.
6. Associar ativo, fornecedor, ciclo e NF na plataforma; manter o dispositivo
   reutilizável. A localização semanal e o prazo de permanência são regras distintas.
7. Dimensionar alimentação e proteção; implementar deep sleep/desligamento de
   periféricos e medir consumo antes de estimar autonomia.

Esses itens são um roteiro de continuidade, não autorização para implantar ou
expandir todas essas etapas em uma tarefa de simples ajuste do circuito.

## Critérios de aceite da modelagem inicial

- Setup reproduzível e configurações incluídas, sem credenciais.
- GPS Custom Chip compilado; firmware compilado para ESP32.
- Diagrama com pinos consistentes e monitor serial explicitamente ligado.
- Mensagem de atividade independente de GPS e internet.
- Primeiro ponto como referência; comparação posterior com tolerância.
- Sem fix: indisponibilidade explícita e última referência preservada.
- Sinal de lacre registrado separadamente da coleta semanal.
- Nível da bateria e conectividade com origem simulada identificada.
- Documentação que distingue configuração, lógica, simulação e hardware físico.
