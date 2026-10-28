# GuaráTracking — rastreamento de equipamentos em manutenção externa

**Inteli — Sprint 1, Grupo G01 (2026.2)** · Parceiro do desafio: **CerradinhoBio**.

O GuaráTracking propõe associar um dispositivo reutilizável a cada ciclo de manutenção externa de um equipamento. O fluxo de negócio reúne NF de remessa e de retorno, fornecedor, responsável, prazo esperado, última posição GNSS, eventos de lacre e alertas de atraso e permanência. A solução pretende reduzir consultas dispersas e apoiar PCM, Fiscal e Logística. A pesquisa de negócio e o escopo estão em [documentação](document/documentacao.md).

## Integrantes

- Arthur Dantas Pereira Loyola
- Bruno Fernandes Ramos
- Guilherme Nunes Fernandes e Silva
- Heloísa Noda Kadota
- Luiz Gustavo Campos Cazelatto
- Pablo Marchina
- Rachel Durante Silvestre

**Orientadora:** Camila Naves Arantes

## Estado da Sprint 1 — importante

| Componente | Implementado no protótipo local | Ainda não demonstrado / pendente |
|---|---|---|
| ESP32/Wokwi | `setup()` e `loop()`, classe `IndicatorController`, botões, LEDs, lacre, GPS custom, ADC de bateria | CT18 e CT22 reprovados; ver seção 3.4.1 |
| Eventos e regras | Ciclo, autorização e debounce, status, atraso, permanência, JSON na Serial, fila sem rede na NVS | MQTT, confirmação de servidor e autenticação de dispositivos |
| Plataforma | RF/RN/RTM preliminares documentados | Dashboard, BI/API, notificações, cadastro de usuários |
| Testes | CT01–CT23 e CT35 no Wokwi, com evidências em `assets/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/`; testes host de regras e da classe de LEDs | CT18 e CT22 reprovados; CT09 bloqueado |
| Publicação | Código-fonte versionável e pacote de exportação local | Sincronizar o Wokwi público e confirmar GitLab versus GitHub com a disciplina |

**Não confundir** a rede simulada (`SW_REDE`) com o estado real do Wi-Fi, nem os JSONs da Serial com publicação MQTT. Projeto no Wokwi: https://wokwi.com/projects/478276108359405569.

## Ambiente e compilação

- Placa `esp32dev`, framework Arduino, Espressif32 `6.12.0`, TinyGPSPlus `1.0.3`.
- VS Code com PlatformIO e extensão Wokwi; Python 3.10+ e Node 22+ para scripts de bancada.
- Senhas de Wi-Fi locais devem ficar fora do controle de versão (`include/config.local.h`).

Windows, PowerShell, na raiz do repositório:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\setup.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Environment weekly
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\verify.ps1
```

Quando `platformio` estiver instalado globalmente, as duas compilações equivalem a `pio run -e demo` e `pio run -e weekly`. O projeto configura ferramentas locais sob `.tools/`; não fixe caminhos pessoais de Python ou PlatformIO. A verificação host também executa `tests/transport_ack_tests.cpp` e pode rodar na pipeline GitLab. Em Linux/macOS, com `g++` e Python disponíveis, `bash scripts/verify-host.sh` executa verificações portáveis sem substituir os builds ESP32.

## Execução no Wokwi

1. Compile **ambos os ambientes** e gere o chip GPS (`scripts/setup.ps1` e `scripts/build.ps1`).
2. Abra o diretório raiz no VS Code, selecione `src/firmware/prototipo_wokwi/wokwi.toml` com **Wokwi: Select Config File** e inicie **Wokwi: Start Simulator**.
3. Para `weekly`, selecione `wokwi.weekly.toml` e compile o respectivo ambiente.
4. Para publicar no Wokwi web, prepare os arquivos com `python scripts/export-wokwi.py`, siga o [guia do circuito](src/firmware/prototipo_wokwi/README.md) e confira que a URL final corresponde ao commit e executa os CTs com o mesmo binário/código. A publicação requer ação na conta da equipe.
5. Preserve os logs originais dos testes antigos; anexe novas evidências versionadas após os retestes. Os estímulos CT18/CT22 **não podem ser alterados para forçar aprovação**.

`demo`: localização a cada 30 segundos, 1 s simulando 1 dia nas regras de ciclo. `weekly`: sete dias reais, com o ciclo em escala real. O protótipo não comprova autonomia real, vedação ou cobertura em oficinas.

## Organização

- `src/firmware/main.cpp`, `include/`: firmware e regras portáveis.
- `src/firmware/prototipo_wokwi/`, `chips/`: diagrama integrado, configuração e GPS custom.
- `tests/`, `scripts/`: lógica, injeção de falhas e verificações.
- `document/documentacao.md`, `document/outros/`: documento da disciplina, checklist de aceite e fundamentação.
- `assets/3-desenvolvimento-resultados/`, `assets/3-desenvolvimento-resultados/3.2.1-personas/`, `assets/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/`: templates, personas e evidências históricas.

**Checklist antes da entrega:** [relatório consolidado de evidências](document/outros/reports/Sprint01/RELATORIO_ENTREGA_SPRINT1.md), [validação e limites](document/outros/3-desenvolvimento-resultados/validacao-e-limites-sprint1.md) e [fechamento da sprint](document/outros/reports/Sprint01/FECHAMENTO-S1.md). Consolidar `develop` em `main` após revisão, build e reteste, sem sobrescrever evidências.

## Licença

A definição de licença de redistribuição deste repositório deve ser aprovada pelos titulares do projeto. Nenhuma licença é presumida.

### Ensaios adicionais e CI da Sprint 1

`bash scripts/verify-host.sh` executa os testes unitários das regras e da classe `IndicatorController` em host, e `bash scripts/check-firmware-syntax.sh` compila o `main.cpp` com dublês de Arduino. **É um teste em computador, não simulação oficial Wokwi**.

A pipeline `.gitlab-ci.yml` contém jobs para compilação ESP32 (PlatformIO) e um smoke test condicional via Wokwi CLI. O segundo job só é habilitado se um mantenedor configurar `WOKWI_CLI_TOKEN` como variável **masked/protected** no GitLab; **não inserir o token em arquivo nem no chat**. Requer compilar o chip GNSS custom. Veja `tests/wokwi-ci/smoke.yaml` e [roteiro de aceite](document/outros/reports/Sprint01/FECHAMENTO-S1.md). O cenário só pode ser considerado aprovado após executar no Wokwi e arquivar o log do mesmo commit.
