# Inteli - Instituto de Tecnologia e Liderança

<p align="center">
<a href="https://www.inteli.edu.br/"><img src="assets/inteli.png" alt="Inteli - Instituto de Tecnologia e Liderança" border="0" width="40%"></a>
</p>

# GuaráTracking — rastreamento de equipamentos em manutenção externa

## G01

## 👨‍🎓 Integrantes

<!-- PENDENTE: adicionar o link do LinkedIn de cada integrante no formato [Nome](https://www.linkedin.com/in/...). -->
- Arthur Dantas Pereira Loyola
- Bruno Fernandes Ramos
- Guilherme Nunes Fernandes e Silva
- Heloísa Noda Kadota
- Luiz Gustavo Campos Cazelatto
- Pablo Marchina
- Rachel Durante Silvestre

## 👩‍🏫 Professores

### Orientadora

- Camila Naves Arantes

### Instrutores

<!-- PENDENTE: listar os instrutores do módulo com o link do LinkedIn. -->

## 📜 Descrição

O GuaráTracking é uma solução IoT desenvolvida com a **CerradinhoBio** para acompanhar equipamentos enviados à manutenção externa. Cada ciclo de manutenção recebe um dispositivo reutilizável, fixado ao equipamento com um lacre e vinculado à nota fiscal de remessa, ao fornecedor, ao responsável e ao prazo previsto. O dispositivo informa periodicamente a última localização GNSS válida e registra eventos de lacre; a plataforma calcula os dias fora, sinaliza atraso e permanência acima do limiar configurado (referência inicial de 150 dias) e preserva o histórico quando o dispositivo é reutilizado em outro equipamento. O projeto prevê uma API para consulta dos dados por ferramentas de BI. A proposta reduz consultas dispersas e cobranças manuais entre PCM, Fiscal e Logística.

O protótipo roda no Wokwi e em um ESP32 físico offline. Na bancada, os testes registraram a perda e a recuperação dos dados do GPS (CT66) e o acionamento do LED e do buzzer na abertura do lacre (CT69). No Wokwi V2, o CT18 registrou uma falsa abertura e o CT22 aguarda reteste. O CT09 depende da confirmação de recebimento pelo servidor. Os ensaios de bancada CT67 e CT68 seguem planejados. Os resultados por ambiente estão nas seções 3.4.1 e 3.4.2 da [documentação](document/documentacao.md). MQTT, interface web e API estão previstos para as próximas sprints.

Projeto no Wokwi: https://wokwi.com/projects/478276108359405569.

## 📁 Estrutura de pastas

Dentre os arquivos e pastas presentes na raiz do projeto, definem-se:

- <b>.gitlab</b>: template dos cards do Kanban (`issue_templates/card-kanban.md`).

- <b>assets</b>: imagens e arquivos de apoio da documentação, com uma pasta por seção da `documentacao.md` (`1-introducao`, `2-metodologia`, `3-desenvolvimento-resultados`, `4-conclusoes-recomendacoes`, `5-referencias`). Dentro da seção, cada subseção tem a sua pasta (ex.: `3-desenvolvimento-resultados/3.1.2-swot/`). Itens comuns, como o logo do Inteli, ficam na raiz de `assets`.

- <b>chips</b>: chip GPS/GNSS customizado do Wokwi (`gps.chip.c`, `gps.chip.json`) e o gerador de sentenças NMEA (`nmea.h`).

- <b>document</b>: documentação do projeto (`documentacao.md`) e manual de instruções (`manual-de-instrucoes.md`). Em `outros/` ficam os documentos complementares, organizados pelas mesmas pastas de seção usadas em `assets`, e as orientações da disciplina (`orientacoes/`).

- <b>include</b>: cabeçalhos do firmware com a configuração e as regras portáveis (ciclo, lacre, fila de eventos, bateria, relógio e indicadores).

- <b>scripts</b>: preparação do ambiente, compilação, verificação e exportação para o Wokwi.

- <b>skills</b>: instruções para agentes de IA usadas pelo grupo.

- <b>src</b>: código-fonte. `firmware/` contém o firmware do ESP32 (`main.cpp`) e `firmware/prototipo_wokwi/` contém o circuito do Wokwi (`diagram.json`, `wokwi.toml`) e um sketch de teste de ligações (`sketch.ino`).

- <b>tests</b>: testes das regras e dos indicadores executados em computador, dublês das bibliotecas do Arduino e cenários do Wokwi.

- <b>README.md</b>: guia e visão geral do projeto (este arquivo).

- <b>AGENT.md</b>: convenções do repositório para pessoas e agentes de IA.

- <b>platformio.ini</b>: ambientes de compilação `demo` e `weekly` para o simulador e `bancada` para o ESP32 físico.

## 🔧 Execução dos projetos

### Ambiente

- Placa `esp32dev`, framework Arduino, Espressif32 `6.12.0`, TinyGPSPlus `1.0.3` e LiquidCrystal_I2C `1.1.4` (LCD do protótipo V2).
- VS Code com as extensões PlatformIO e Wokwi; Python 3.10+ e Node 22+ para os scripts.
- Senhas de Wi-Fi ficam fora do controle de versão, em `include/config.local.h` (modelo em `include/config.local.example.h`).

### Compilação e verificação

No Windows, com PowerShell, na raiz do repositório:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\setup.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Environment weekly
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\verify.ps1
```

Com o PlatformIO instalado globalmente, as compilações equivalem a `pio run -e demo` e `pio run -e weekly`. As ferramentas locais são instaladas em `.tools/`. Em Linux/macOS, com `g++` e Python, `bash scripts/verify-host.sh` executa os testes das regras e da classe `ScreenController` (telas do LCD), e `bash scripts/check-firmware-syntax.sh` compila o `main.cpp` com dublês do Arduino. São testes em computador e não substituem a compilação para ESP32 nem a simulação no Wokwi.

### Simulação no Wokwi

1. Compile os dois ambientes e gere o chip GPS (`scripts/setup.ps1` e `scripts/build.ps1`).
2. Abra a raiz do repositório no VS Code, selecione `src/firmware/prototipo_wokwi/wokwi.toml` com **Wokwi: Select Config File** e inicie **Wokwi: Start Simulator**.
3. Para o ambiente `weekly`, selecione `wokwi.weekly.toml` e compile o ambiente correspondente.
4. Para publicar no Wokwi web, prepare os arquivos com `python scripts/export-wokwi.py` e siga o [guia do circuito](src/firmware/prototipo_wokwi/README.md).

No ambiente `demo`, a localização é coletada a cada 30 segundos e 1 s simula 1 dia nas regras de ciclo. No `weekly`, a coleta ocorre a cada sete dias reais. A autonomia da bateria e a cobertura em oficinas ainda dependem de medições em campo.

### Execução no ESP32 físico

Siga a pinagem do [guia de montagem](document/outros/3-desenvolvimento-resultados/3.4.2-prototipo-fisico-offline/relatorio-montagem-prototipo-fisico.md) e conecte o ESP32 por USB. No terminal do PlatformIO, na raiz do projeto, execute:

```powershell
pio run -e bancada -t upload
pio device monitor -b 115200
```

No ambiente de bancada, 1 segundo representa 1 dia do ciclo. Os botões LACRE e REDE alternam o estado a cada toque. Para ligar ou desligar o protótipo, mantenha ON/OFF pressionado por 3 segundos.

### Verificações locais

As verificações em computador, a auditoria da documentação e a compilação para ESP32 podem ser executadas localmente com `scripts/verify-host.sh`, `scripts/audit-docs.py` e PlatformIO. O cenário `tests/wokwi-ci/smoke.yaml` permanece disponível para testes manuais com o Wokwi CLI.

## 🗃 Histórico de lançamentos

* 0.2.0 - 05/11/2026
    * Sprint 2: jornadas atual e futura e storyboard de Geraldo, oito requisitos não funcionais, matriz de rastreabilidade atualizada, testes no Wokwi V2 e protótipo físico offline com ESP32, GPS, LCD e alerta de violação do lacre.

* 0.1.0 - 24/10/2026
    * Sprint 1: entendimento do negócio (contexto da indústria, Cinco Forças de Porter, SWOT, Value Proposition Canvas, matriz de riscos, política de privacidade e BOM), personas, requisitos funcionais, regras de negócio, matriz de rastreabilidade e protótipo inicial no Wokwi com os casos de teste CT01–CT23 e CT35.

## 📋 Licença

<!-- PENDENTE: aplicar a licença Creative Commons Attribution 4.0 International citando o Inteli e o grupo G01, conforme os critérios da disciplina, após aprovação do grupo. -->
A definição da licença de redistribuição deste repositório deve ser aprovada pelos titulares do projeto. Nenhuma licença é presumida.
