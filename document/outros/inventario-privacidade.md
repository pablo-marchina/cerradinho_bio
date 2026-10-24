# Inventário de tratamento de dados — minuta técnica LGPD

**Situação:** instrumento de planejamento, não política promulgada. Antes de qualquer piloto com dados reais, exigir aceite de controlador, jurídico/encarregado e TI. O protótipo de Sprint 1 usa dados sintéticos e apresenta logs na Serial e NVS local.

## Inventário por finalidade (hipótese a validar)

| Ativo/campo | Fonte | Finalidade restrita | Base legal candidata (não aprovada) | Destinatários candidatos | Retenção sugerida para decisão |
|---|---|---|---|---|---|
| Nome e contato corporativo, `usuario_id`, perfil | Cadastro corporativo | Identificação, responsável, trilha de atendimento | Legítimo interesse ou execução contratual, conforme relação | Gestão/PCM/Fiscal (acesso mínimo) | Enquanto necessário ao vínculo + prazo legal definido |
| Empresa fornecedora, OS, código do ativo | Cadastro e sistema corporativo | Vincular equipamento a terceiro | Legítimo interesse/contrato; avaliar se há pessoa física | PCM/Fiscal/Logística | Prazo documental definido pelo Fiscal |
| NF remessa/retorno, datas e prazo fiscal | ERP e fiscal | Conciliação de remessa, retorno e obrigações | Obrigação legal/regulatória, condicionada à avaliação fiscal | Fiscal, auditores autorizados | Prazo fiscal/prescricional validado |
| Coordenadas, timestamp, dispositivo, eventuais metadados de presença | GNSS e firmware | Encontrar **equipamento**, não monitorar trabalhador | Legítimo interesse com teste de balanceamento ou outra base aprovada | PCM e operador responsável | Geolocalização bruta mínima; política aprovada por categoria |
| IP, user-agent, logs, eventos de sessão | Plataforma futura (não implementada) | Segurança e prevenção de acesso indevido | Legítimo interesse/obrigação aplicável | Segurança/TI | Prazo de logs de segurança aprovado |
| Credenciais e identificadores de device | Provisionamento futuro (não implementado) | Autenticação e integridade de telemetria | Interesse legítimo/contrato conforme escopo | TI e operador técnico | Revogar na desativação, segredos excluídos sob política |

A escolha definitiva de base legal depende do papel da CerradinhoBio e da organização executora, finalidade concreta e teste de necessidade. Consentimento não é adotado automaticamente como base universal. Exigir minimização: não registrar nome de motoristas, dados de localização de indivíduos ou nomes pessoais de funcionários em telemetria quando um `device_id` e identificador de função forem suficientes.

## Onde, com quem, segurança

- **Implementado:** protótipo Wokwi e firmware local; impressão em Serial e snapshots de eventos na NVS; **sem banco ou API de produção**. A memória NVS do protótipo **não contém criptografia/autenticação de payload implementada neste firmware**; não usar dados pessoais reais no simulador.
- **Planejado, não contratado:** banco, armazenamento de backups, serviço MQTT, dashboard, alertas e API. Regiões, contratos, transferências internacionais e fornecedores **não definidos**.
- **Cookies:** não há produto web publicado neste repositório. Ao implementar, inventariar essenciais/analíticos, consentimento e preferências quando aplicável; não confundir com cookies do site institucional.
- **Controles antes do piloto:** controle de acesso por perfil, criptografia em trânsito e em repouso conforme arquitetura, segregação de dados, credenciais rotativas, inventário de subprocessadores, gestão de vulnerabilidades, backups/restauração e plano de incidentes testados. Nenhum desses controles de plataforma pode ser declarado implementado na Sprint 1.

## Titulares, responsáveis e canal

A página pública da CerradinhoBio [1] informa o endereço **lgpd@cerradinho.com.br** como canal corporativo. **Não há confirmação de que esse canal seja oficialmente designado para o GuaráTracking**, nem de que a identificação nominal do encarregado publicada em 2021 continue atual. O projeto deve publicar canal, prazo e mecanismo de verificação de identidade **após** confirmação institucional, sem solicitar cópia de documentos pessoais por meios improvisados.

Exercício de direitos conforme LGPD e regulamentos aplicáveis: confirmação de tratamento, acesso, retificação, explicação/portabilidade nos casos cabíveis, informação sobre compartilhamento, oposição, eliminação ou anonimização quando cabível; registrar requisição e decisão, com resposta pela organização responsável e justificativa nos casos de limitação legal.

## Pendências com responsáveis

1. Parceiro/Jurídico: controlador, operador, identificação institucional aplicável, canal de contato, DPO designado.
2. Jurídico/Fiscal: mapa de dados→finalidade→base legal definitiva→período de retenção/arquivamento fiscal.
3. TI/Segurança: infraestrutura, país/região, operadores, contratos e suboperadores, plano de backups e descarte.
4. Produto: avisos, controles de minimização, papéis/consentimentos quando necessário, procedimento do titular.
5. Engenharia/QA: evidências de controles implementados, auditoria e tratamento de incidentes.

[1] CerradinhoBio, *Aviso de privacidade e uso de cookies* (publicado em 2021; consulta 23/10/2026): https://www.cerradinhobio.com.br/politica-de-privacidade/ . A versão institucional não é transposição automática para este produto.
