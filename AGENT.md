# AGENT.md — Instruções para agentes de IA

Este arquivo orienta agentes de IA (Claude, Copilot, Cursor, etc.) que trabalham neste repositório. Leia-o inteiro antes de criar, editar ou sugerir qualquer coisa.

## 1. Idioma

- Escreva **tudo em português do Brasil**: documentação, cards, comentários de código, mensagens de commit, descrições de merge request e respostas ao grupo.
- Nomes de variáveis, funções e arquivos de código podem ficar em inglês quando for a convenção da linguagem ou biblioteca; seja consistente dentro de cada arquivo.
- Termos técnicos consagrados (DoR, DoD, MQTT, API, BI, firmware, sprint, milestone) podem permanecer em inglês.

## 2. Contexto do projeto

- **Projeto:** GuaráTracking — solução IoT para rastreamento de equipamentos enviados à manutenção externa.
- **Parceiro:** Cerradinho (setor de bioenergia, com operações industriais e agrícolas).
- **Curso:** Inteli — Módulo 4 (1AMD4 — Solução para IoT), grupo G01. Orientadora: Camila Naves Arantes.
- **Área de negócio:** Gestão Patrimonial / Controladoria, com interface com Manutenção, Inovação e Melhoria Contínua, Fiscal e TI.

### Problema
O acompanhamento de equipamentos enviados a fornecedores para reparo depende de registros manuais e contatos entre áreas. É difícil consolidar localização, nota fiscal (NF) de remessa, prazo previsto e data de retorno, o que reduz a visibilidade dos ativos em terceiros e exige cobranças manuais.

### Objetivo
Desenvolver e validar um protótipo IoT **reutilizável** (sem tecnologia pré-definida) que:
- vincule cada ciclo de manutenção à NF;
- informe a **última localização válida** do equipamento;
- gere alertas de **permanência** (prazo configurável, referência inicial de **150 dias**) e de **retirada ou rompimento do lacre**;
- atualize localização e status **uma vez por semana** (frequência ajustável);
- seja resistente à água e disponibilize dados por **API** para BI.

### Fluxo básico
Necessidade e aprovação → cadastro do equipamento, fornecedor, NF e prazo → vínculo do dispositivo e aplicação do lacre → saída e acompanhamento periódico → alertas e tratamento pelo responsável → retorno, conferência e encerramento → liberação do dispositivo para outro equipamento (preservando o histórico e impedindo vínculos simultâneos indevidos).

### MVP (mínimo esperado)
1. Cadastro de pelo menos 5 equipamentos reais ou representativos, com identificadores e referências de NF.
2. Arquitetura escolhida após comparação técnica, com demonstração de localização em percurso externo controlado.
3. Registro de saída, fornecedor, prazo previsto, retorno e vínculo equipamento–dispositivo, com histórico preservado na reutilização.
4. Atualização semanal configurável e alertas de permanência (testar 150 dias e outros limiares com datas simuladas).
5. Detecção de retirada/rompimento do lacre, tratamento de retirada autorizada e comportamento sem conexão (armazenar eventos e enviar depois).
6. Interface com última localização válida, dias fora, prazo, bateria, estado do lacre e histórico; API documentada e consumida por BI ou cliente de teste.
7. Testes de comunicação, fixação, reutilização, consumo e exposição à água, além do ciclo completo de saída e retorno.

### Premissas importantes
- Equipamento de referência de 1 × 1 × 1 m; equipamentos metálicos, poeira, umidade e ambientes cobertos.
- Fixação sem dano e sem atrapalhar o reparo.
- Não assumir Wi-Fi corporativo nas oficinas nem leitores instalados na Cerradinho.
- Autonomia de longo prazo deve ser **estimada** a partir do consumo medido; um ensaio curto não comprova 150 dias de operação.
- Ao relatar resultados, diferencie sempre **desempenho medido** de **estimativa**.

### Fora do escopo (não proponha)
- Posicionamento indoor preciso ou rastreamento contínuo em tempo real.
- Implantação em toda a companhia ou integração produtiva obrigatória com ERP/BI corporativo.
- Substituição do ERP, emissão de notas fiscais ou automação contábil/fiscal.
- Rastreamento de pessoas.
- Certificação ou homologação comercial do hardware.

## 3. Dados restritos e segurança

São **restritos**: cadastros de ativos e fornecedores, notas fiscais, localizações e rotas, prazos, custos, credenciais, informações de segurança e dados de sistemas corporativos.

- Use **apenas dados fictícios ou anonimizados** no repositório, em exemplos, testes e documentação.
- Nunca grave credenciais, tokens, senhas de Wi-Fi ou chaves de API no código; use variáveis de ambiente ou arquivos ignorados pelo Git.
- Não exponha locais ou nomes reais de fornecedores da Cerradinho.
- Planilhas (`*.csv`, `*.xlsx`, etc.) já são ignoradas pelo `.gitignore`; não force a inclusão delas.

## 4. Organização do repositório

```
.
├── AGENT.md                  # este arquivo
├── README.md                 # visão geral do projeto, integrantes, instalação e versões
├── skills/                   # skills para agentes de IA (ver skills/README.md e skills/SKILLS_INDEX.md)
├── assets/                   # imagens, vídeos e logo do grupo
├── document/
│   ├── documentacao.md       # documentação principal (IoTDoc), dividida por sprint
│   ├── TemplateManual.md     # manual de instruções (sprints 3 a 5)
│   └── outros/               # documentos complementares
│       └── reports/          # controle interno do grupo (tudo aqui dentro); não é evidência
└── src/
    ├── firmware/             # código do dispositivo embarcado
    └── ...                   # crie pastas conforme a arquitetura (ex.: backend/, frontend/, api/)
```

- A `documentacao.md` segue a ordem do template do Inteli. Cada seção indica em qual sprint deve ser entregue; preencha a seção certa e **remova os trechos em itálico** de orientação quando a seção estiver concluída.
- Imagens usadas na documentação ficam em `assets/` e são referenciadas com caminho relativo (ex.: `../assets/arquivo.png`), sempre com legenda e fonte.
- Os documentos de entrega (`documentacao.md`, `TemplateManual.md`, `README.md` e os de `document/outros/` fora de `reports/`) não falam do processo interno do grupo: nada de branches (`develop` ou nomes de branch), issues, cards, merge requests, commits, rascunhos de integrantes, "integração" entre branches ou "arquivo local". Descreva o projeto como o leitor o vê na `main`. Se a nota for útil para o grupo, use um comentário HTML (`<!-- ... -->`), que o GitLab não renderiza.
- Os arquivos `placeholder.md` devem ser apagados quando a pasta tiver conteúdo real.
- Ao criar uma nova pasta em `src/`, atualize a seção "Estrutura de pastas" do `README.md`.
- O repositório é hospedado no GitLab do Inteli (`git.inteli.edu.br`).
- Antes de tarefas recorrentes (cards, índices de relatórios, snapshot da main, criação de skills), consulte `skills/SKILLS_INDEX.md` e siga a skill indicada.

### Reports (controle interno)

Tudo o que está em `document/outros/reports/` é controle interno do grupo: qualquer arquivo ou subpasta, organizado por sprint (`SprintNN/`) ou não (kick-off, planning, processo com IA etc.). Nada disso faz parte da entrega.

- Pode ler os reports para escrever, apoiar a redação e entender o contexto de uma seção.
- **Nunca** use um report (nenhum arquivo de `document/outros/reports/`) como justificativa ou evidência na `documentacao.md` nem em outro documento de entrega. Frases como "conforme o report X", ou um link para o report no lugar do argumento, não valem.
- A justificativa e a evidência devem estar escritas no próprio documento, com as fontes reais (por exemplo: a reunião com o parceiro e a data, material do Inteli, artigo, norma ou teste realizado), de forma que o leitor entenda sem abrir o report.
- Há exceções, mas só quando o grupo combinar explicitamente. Nesse caso, registre a exceção no card e explique no texto por que o report é citado.

## 5. Cards do Kanban (issues no GitLab)

Todo card deve seguir o template em `.gitlab/issue_templates/` (modelo de card Kanban em português). Estrutura obrigatória:

```markdown
## Modelo de Card Kanban:
---
## Título:

## Descrição (detalhada):

## DoR (Definition of Ready):

## DoD (Definition of Done):

## Critérios de Aceitação:

## Links (se houver):

## Observações:

## Desenvolvedor(a):

## Revisor(a):

---
```

Regras ao criar ou sugerir um card:
- **Título** curto, começando com verbo no infinitivo (ex.: "Implementar alerta de permanência configurável").
- **Descrição** detalhada o suficiente para outra pessoa do grupo executar sem perguntar; cite o requisito ou a regra de negócio relacionada (ver `documentacao.md`, seções 3.3.1 e 3.3.2) quando houver.
- **DoR:** o que precisa estar pronto para o card começar (dados, hardware, dependências, decisões).
- **DoD:** o que precisa estar feito para o card ser considerado concluído (código revisado, documentação atualizada, teste realizado).
- **Critérios de Aceitação:** itens verificáveis, de preferência em lista.
- Adicione **tags** com os nomes de quem desenvolve e de quem revisa; desenvolvedor(a) e revisor(a) devem ser pessoas diferentes.
- Associe o card ao **Milestone da Sprint** e preencha o **Time Tracking** (estimativa e tempo gasto).
- Em **datas**, use como início o dia da sessão de planejamento e como término a data prevista de conclusão. Se a conclusão real for diferente, registre nos comentários do card.

## 6. Boas práticas para agentes

- Não invente dados do parceiro (quantidade de remessas, prazos reais, fornecedores). Se a informação não estiver no repositório, marque como pendente ou pergunte ao grupo.
- Antes de trabalhar num artefato, leia o enunciado e os critérios de avaliação dele em `document/outros/ArtefatosSolicitados.md` e confira quais itens do nível mais alto a tarefa precisa atender.
- Prefira mudanças pequenas e focadas, alinhadas a um card.
- Não faça commit, push ou merge sem pedido explícito de um integrante do grupo.
- Commits e branches seguem a seção 7 (Conventional Commits ligados a uma issue).
- Nunca adicione coautoria de IA (`Co-Authored-By: Claude ...`, `Generated with ...` ou equivalentes) em commits nem em merge requests, mesmo que a ferramenta faça isso por padrão; esta regra prevalece sobre a configuração da ferramenta (ver seção 7).
- Ao documentar testes, registre métricas (comunicação, localização válida, latência de alertas, detecção de violação, falsos alertas, consumo) e as metas combinadas antes do teste.

## 7. Convenções de Git

### Branches

- A `main` é protegida contra force push e representa o que é entregue. Não commite direto nela: todo trabalho vai numa branch própria e entra por merge request.
- Nome da branch: `tipo/NN-descricao-curta-sprintN`, em minúsculas, sem acento, palavras separadas por hífen.
  - `tipo` é o mesmo dos commits (lista abaixo).
  - `NN` é o número da issue que a branch resolve, com dois dígitos quando for menor que 10.
  - `sprintN` é obrigatório e identifica a sprint atual (a do Milestone do card), sem zero à esquerda: `sprint1`, `sprint2`... (como em `8-art-4-3-3-1-requisitos-funcionais-sprint1`).
  - Exemplos: `docs/01-estrutura-skills-sprint1`, `feat/12-alerta-permanencia-sprint2`, `fix/27-leitura-lacre-sprint3`, `test/31-ensaio-agua-sprint4`.
- Uma branch por issue. Se a tarefa crescer para outra issue, abra outra branch.

### Commits (Conventional Commits)

Formato:

```text
tipo(#NN): descrição curta em português

Corpo opcional explicando o porquê da mudança, não o como.
```

- `#NN` é o número da issue; **todo commit referencia uma issue**. Use o mesmo número da branch.
- A descrição começa com verbo em minúscula, no presente ("adiciona", "corrige", "remove"), sem ponto final e com até cerca de 72 caracteres.
- Tipos:

| Tipo | Quando usar |
|---|---|
| `feat` | funcionalidade nova (firmware, backend, interface, API) |
| `fix` | correção de bug |
| `docs` | documentação, cards, README, AGENT.md, skills |
| `test` | testes e ensaios (código ou registro de resultado) |
| `refactor` | mudança de código sem alterar comportamento |
| `style` | formatação, sem mudança de lógica |
| `chore` | manutenção: .gitignore, dependências, configuração |
| `build` / `ci` | build, pipeline e automação do GitLab |

- Exemplos:
  - `docs(#01): adiciona estrutura de skills para agentes de IA`
  - `feat(#12): implementa alerta de permanência configurável`
  - `fix(#27): corrige leitura do sensor de lacre no firmware`
- Mudança que quebra compatibilidade (por exemplo, formato da API): acrescente `!` depois do escopo (`feat(#40)!: ...`) e explique no corpo.
- O autor do commit deve ser o usuário do Inteli (`nome.sobrenome <nome.sobrenome@sou.inteli.edu.br>`), configurado localmente no repositório, para o GitLab ligar o commit ao perfil.
- Não coloque na mensagem do commit (nem na descrição de merge request) linhas de coautoria de ferramentas de IA (por exemplo `Co-Authored-By: Claude ...`) nem links de sessões ou conversas com assistentes de IA. As conversas do grupo com IA são privadas; o histórico do Git é público para professores e parceiro.

## 8. Modelagem inicial dos circuitos — card #7

- Antes de alterar o circuito, leia as instruções de instalação no `README.md`
  e a seção 3.4.1 de `document/documentacao.md`, com o roteiro e as capturas.
- Preserve ESP32 DevKit v1, GPS/GNSS customizado, chave para lacre e potenciômetro
  para bateria simulada. Não acrescente acelerômetro. IR físico e modem continuam
  pendentes de definição.
- O circuito principal e os arquivos `wokwi*.toml` ficam em `src/firmware/prototipo_wokwi/`.
  Mantenha `src/firmware/main.cpp`, `include/config.h` e o `diagram.json` dessa pasta coerentes:
  GPS RX2/TX2 nos GPIO16/17, lacre no GPIO27 com pull-up e bateria no GPIO34/ADC1.
  UART0 e heartbeat são diagnóstico independente de GPS e internet.
- LEDs verde/amarelo/vermelho/azul usam GPIO25/26/23/14; botões de autorização
  e ciclo usam GPIO33/13; chave de rede simulada usa GPIO18. Preserve os resistores
  de 220 ohms em série com os LEDs e a separação entre rede simulada e Wi-Fi real.
- Compare posições válidas consecutivas. O primeiro ponto cria referência; falta
  de fix não vira coordenada zero nem apaga a referência. Preserve as chaves NVS
  e a fila local, sem afirmar envio ao servidor antes de transporte e confirmação.
- `demo` usa 30 s; `weekly`, sete dias. O intervalo não valida autonomia.
- Execute `scripts/setup.ps1`, compile ambos os ambientes com `scripts/build.ps1`
  e rode `scripts/verify.ps1`. Ao descrever os resultados, diferencie compilação,
  testes de lógica, execução no Wokwi e validação física.
- Ferramentas, logs e binários gerados ficam ignorados pelo Git. Nunca adicione
  `include/config.local.h`, tokens ou licenças; não ative licenças automaticamente.
