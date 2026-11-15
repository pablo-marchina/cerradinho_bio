# Guarátracking — Frontend v1.0

**Candidato a release acadêmica, frontend-only, em modo demonstração.** Painel operacional de equipamentos remetidos à manutenção externa. A identidade segue o guia Guarátracking v2. O cenário inicial tem seis equipamentos sintéticos e um relógio simulado em 12/11/2026, 10h de Brasília, independente do relógio do navegador.

Não há backend, banco de dados, credenciais MQTT, comunicação com ESP32, emissão de NF, autenticação real ou dados do parceiro. Alterações duram enquanto a aplicação permanece carregada; recarregar a página ou reiniciar o cenário restaura os dados. Comandos e telemetria simulados nunca constituem evidência de integração da seção 3.4.3.

## Executar

Node.js 22.16+ e npm. No pacote de entrega, copie somente o conteúdo de `arquivos-por-mr/141/` para a raiz do repositório, preservando os caminhos: a pasta final será `src/frontend/`. Não copie a pasta `141` nem extraia o ZIP inteiro na raiz. Se já houver alterações no frontend, revise o diff antes de substituir.

```bash
cd src/frontend
npm install
npm run dev
```

Abra http://localhost:5173 (ou a URL exibida). O `postinstall` gera `public/mockServiceWorker.js`; caso os scripts estejam desabilitados, execute `npx msw init public --save`. MSW precisa de localhost ou HTTPS. A aplicação inicia o worker antes de renderizar, sem API externa.

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

- 37 testes Node de domínio aprovados em 14/11/2026; `docs/testes-node.log` registra a rodada original de 27 testes.
- Checagem estrita TypeScript dos módulos de domínio, simulador e fixtures, e transpilação sintática de todos os arquivos TS/TSX aprovadas.
- Instalação npm, typecheck de toda a aplicação, build Vite e 5 testes Vitest foram executados em 14/11/2026. Os 8 roteiros Playwright passaram em desktop e mobile (16 execuções, incluindo o novo roteiro de bateria e alertas). O build passou com aviso de bundle JavaScript acima de 500 kB. Acessibilidade e validação com usuários não foram homologadas.
- A v1.0 significa escopo visual/funcional de demonstração consolidado em código; não significa produto de produção ou integração comprovada. API autenticada, persistência, RBAC real, idempotência distribuída, MQTT, emissão fiscal, testes com usuários, hardware e homologação do parceiro permanecem fora do escopo.
<!-- Nota da preparação original: nenhum original do GitLab havia sido alterado; o pacote foi preparado para aplicação e revisão do grupo. -->

Veja [CHANGELOG](CHANGELOG.md), [cobertura](docs/cobertura.md), [contrato de integração](docs/integracao.md), [requisitos originais](docs/requisitos-originais.md) e [design system](docs/design-system.md).
