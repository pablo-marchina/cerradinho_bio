# Guarátracking — Frontend v1.0

Painel operacional de equipamentos remetidos à manutenção externa. A identidade segue o guia Guarátracking v2. A aplicação oferece dois modos separados: demonstração com dados sintéticos e integração MQTT por WebSocket com o protótipo.

Não há backend, banco de dados, emissão de NF ou autenticação real. O modo demonstração continua sem comunicação externa. No modo MQTT, telemetria e comandos vivem apenas na sessão do navegador; nenhum segredo deve ser incluído no bundle nem versionado.

## Executar

Node.js 22.16+ e npm. No pacote de entrega, copie somente o conteúdo de `arquivos-por-mr/141/` para a raiz do repositório, preservando os caminhos: a pasta final será `src/frontend/`. Não copie a pasta `141` nem extraia o ZIP inteiro na raiz. Se já houver alterações no frontend, revise o diff antes de substituir.

```bash
cd src/frontend
npm install
npm run dev
```

Abra http://localhost:5173 (ou a URL exibida). O `postinstall` gera `public/mockServiceWorker.js`; caso os scripts estejam desabilitados, execute `npx msw init public --save`. MSW precisa de localhost ou HTTPS. A aplicação inicia o worker antes de renderizar, sem API externa.

## Integração MQTT local

O modo MQTT assina `status`, `evento`, `online` e `cmd/resposta` e publica em `cmd`, sempre sob `guaratracking/{device_id}/`. Ele aceita o formato compacto atual do firmware, inclusive `online`/`offline` em texto, e também compreende o formato expandido previsto no contrato. A versão atual do firmware ainda não publica coordenadas no MQTT; a posição enviada pelo simulador exercita o formato proposto para a próxima integração.

1. Copie `.env.mqtt.example` para `.env.local` e mantenha os valores locais sem credenciais.
2. Inicie o EMQX com MQTT TCP em `1883` e WebSocket em `8083`, caminho `/mqtt`.
3. Em um terminal, execute o simulador fiel aos tópicos do ESP32:

   ```powershell
   npm run simulate:mqtt
   ```

4. Em outro terminal, execute:

   ```powershell
   npm run dev
   ```

O simulador publica status, violação e uma posição fictícia no formato proposto, além de responder aos mesmos comandos planos implementados pelo firmware. Para usar a placa física, encerre o simulador, defina `VITE_MQTT_SOURCE=esp32` e configure o ESP32 com o mesmo `device_id` e o endereço MQTT TCP do notebook. O dashboard continua usando WebSocket. Até o firmware publicar a última posição válida, essa informação ficará indisponível no modo físico.

Variáveis disponíveis: `VITE_MQTT_URL`, `VITE_MQTT_DEVICE_ID`, `VITE_MQTT_SOURCE` (`simulator` ou `esp32`), `VITE_MQTT_USERNAME` e `VITE_MQTT_PASSWORD`. Usuário e senha em variáveis `VITE_*` ficam visíveis no navegador; portanto, este caminho é adequado apenas ao broker local de demonstração. Um ambiente externo autenticado precisa de backend ou credencial temporária de escopo mínimo.

```bash
npm run test:domain       # Node 22: não precisa de npm install
npm test                  # Vitest, após instalar
npm run build             # tsc --noEmit + Vite
npx playwright install chromium
npm run test:e2e          # desktop e mobile, após instalar
npm run preview           # serve o build estático
```

O `package-lock.json` foi gerado e versionado após instalação validada; use `npm ci` para reproduzir o ambiente. O build e os testes locais estão descritos abaixo. O deploy estático no Vercel é opcional: Root Directory `src/frontend`, framework Vite, Install `npm install`, Build `npm run build`, Output `dist`. Nenhum deploy foi realizado.

## Telas

| Rota | Conteúdo |
|---|---|
| `/` | KPIs derivados do cenário, alerta crítico, tabela prioritária, eventos |
| `/equipamentos` | Busca, filtros, ordenação, paginação, cadastro fictício |
| `/equipamentos/:id` | Ciclo, responsável, NF, dias fora, atraso/permanência, telemetria, comandos, laboratório e histórico |
| `/alertas` | Tipos, situação, fornecedor, busca e trilha de tratamento justificada |
| `/mapa` | Últimas coordenadas válidas, marcadores MapLibre, zoom/seleção, filtro GNSS, mapa base opcional |
| `/ciclos` | Consulta e operações de abertura, retorno, NF, encerramento e arquivo de ciclos |
| `/dispositivos` | Seletor TAG/ativo, telemetria, comandos e laboratório de cenários |
| `/relatorios` | Recharts por fornecedor/faixa de permanência, contagem de alertas e CSV fictício |

## Demonstração sugerida

1. Abra a visão geral: identifique a violação em GT-0042 e o atraso em GT-0017; veja que GT-0060 tem permanência prolongada mas não está atrasado.
2. Dispositivos: selecione GT-0055; aplique “Reconectar e confirmar fila”, escolha “Autorizar retirada”, preencha justificativa e confirme. Aguarde `Executado (simulado)`. Aplique “Abrir lacre (respeita autorização)”. O lacre deve aparecer autorizado. Aplique “Fechar lacre (aguarda reaplicação)” e depois use “Reaplicar lacre” com justificativa. A reaplicação deve ser recusada enquanto não houver fechamento no cenário.
3. Aplique bateria 19%, 21%, 26%, 19%. O evento BATERIA_BAIXA aparece duas vezes; o estado de alerta aberto não é duplicado. Aplique GNSS sem fix; a última posição anterior permanece com a idade e hora. Simule perda MQTT/LWT e fila cheia; a fila é última informação simulada, não leitura ao vivo.
4. Central de comandos: escolha resposta `Forçar recusa`, `Simular ausência de resposta` ou `Simular expiração` e verifique ausência de alteração de negócio. No fluxo normal, autorizar, reaplicar e encerrar exigem TAG conectada; operações de cadastro e registros de negócio são simulações locais. Autorização tem validade configurável, com padrão de 4320 minutos (3 dias), alinhado ao parâmetro de teste da RN06 e ainda pendente de homologação do parceiro.
5. Ciclos: selecione a válvula GT-0008 com retorno físico já confirmado e baixa pendente. Reconecte em Dispositivos; registre NF de retorno fictícia, aplique “Fechar lacre (aguarda reaplicação)”, reaplique o lacre com justificativa e encerre. Para abrir novo ciclo, preencha os dados completos. A posição do ciclo anterior não é copiada. Os eventos permanecem.
6. Alertas: registre ciência, tratamento e resolução com justificativa. A resolução não apaga violação histórica nem altera lacre físico. Para conferir RN14, provoque silêncio por mais de dois intervalos, resolva o alerta, reconecte e avance novamente além de dois intervalos: uma nova ocorrência deve aparecer, sem duplicar alertas ainda abertos.
7. Mapa: selecione um marcador e abra seu detalhe. Mapa base OSM é opcional e necessita de internet; a vista padrão usa coordenadas sobre fundo neutro. Consulte a lista mesmo sem WebGL. Relatórios: filtre fornecedor e exporte CSV.

## Arquitetura

```text
src/
  app/                     # roteamento
  components/layout/       # shell, sidebar, header
  components/ui/           # Button, Card e utils; primitives locais
  components/domain/       # KPIs, tabela, estados, timeline, telemetria e comandos
  features/                # módulos de produto
  services/
    types.ts               # contratos de frontend
    domain.ts              # regras de cálculo, filtragem e datas
    simulator.ts           # transições puras (dados sintéticos)
    api.ts                 # cliente HTTP substituível
    useSnapshot.ts         # TanStack Query e polling de respostas pendentes
  mocks/                   # fixtures e handlers MSW
  styles/                  # tokens e CSS do guia
public/                    # recorte da assinatura oficial, worker gerado
 tests/ e e2e/             # testes de domínio e navegador
 docs/                     # especificações, cobertura e integração
```

Stack efetivamente usada: React, Vite, TypeScript, Tailwind, React Router, TanStack Query e Table, Lucide, CVA/clsx/tailwind-merge, MSW, React Hook Form, Zod, MapLibre e Recharts. Não foi adicionado Zustand: estado global remoto simulado fica em TanStack Query e estado visual em React/URL. Os primitives foram mantidos em código local compatível com tokens shadcn; não foram gerados pela CLI. As fontes vêm de Google Fonts com fallback de sistema. O logo PNG é recorte do guia; trocar pelo arquivo-mestre aprovado. Grafismos Pataxó não foram incluídos. O mapa não infere rotas; uso público do servidor de tiles exige revisão das condições do provedor.

## Validação e limites

- 39 testes Node de domínio aprovados em 16/11/2026; `docs/testes-node.log` registra a rodada original de 27 testes.
- Checagem estrita TypeScript dos módulos de domínio, simulador e fixtures, e transpilação sintática de todos os arquivos TS/TSX aprovadas.
- Instalação npm, typecheck de toda a aplicação, build Vite e 5 testes Vitest foram executados em 16/11/2026. Os 8 roteiros Playwright passaram em desktop e mobile (16 execuções, incluindo o roteiro de bateria e alertas). O build passou com aviso de bundle JavaScript acima de 500 kB. Acessibilidade e validação com usuários não foram homologadas.
- A v1.0 significa escopo visual/funcional de demonstração consolidado em código; não significa produto de produção ou integração comprovada. API autenticada, persistência, RBAC real, idempotência distribuída, MQTT, emissão fiscal, testes com usuários, hardware e homologação do parceiro permanecem fora do escopo.
<!-- Nota da preparação original: nenhum original do GitLab havia sido alterado; o pacote foi preparado para aplicação e revisão do grupo. -->

Veja [CHANGELOG](CHANGELOG.md), [cobertura](docs/cobertura.md), [contrato de integração](docs/integracao.md), [requisitos originais](docs/requisitos-originais.md) e [design system](docs/design-system.md).
