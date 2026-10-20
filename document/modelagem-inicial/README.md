# Cerradinho — modelagem inicial do dispositivo IoT

Setup do repositório GuaráTracking para ESP32 com GPS simulado, entrada de lacre, leitura de bateria
simulada e Wi-Fi. Compara a localização atual com a última amostra válida.

Esta etapa entrega circuito de simulação, firmware e instruções de continuidade.
Não inclui PCB, sensor óptico validado, dimensionamento de bateria, modem celular
ou servidor. A conexão Wi-Fi é um recurso da demonstração, não uma decisão de
cobertura para os fornecedores.

Os caminhos e comandos deste guia são relativos à raiz `g01`, não à pasta deste documento.

## Começar no VS Code (Windows x64)

1. Abra **a raiz do repositório `g01`** no VS Code (Arquivo → Abrir Pasta).
2. Instale as extensões recomendadas: **PlatformIO IDE** e **Wokwi Simulator**.
3. Abra um terminal PowerShell na raiz e execute:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\setup.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Environment weekly
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\verify.ps1
```

O setup baixa Wokwi CLI 0.29.0 e WASI SDK 25.0 de suas distribuições oficiais.
Verifica o SHA256 do Wokwi CLI. Reaproveita PlatformIO instalado ou instala uma
cópia local via Python. Os pacotes ESP32 e os downloads ficam em `.tools/`.
É preciso internet para a primeira preparação; Python 3.10+ e Node.js 22+ devem
estar disponíveis. Se o PlatformIO já estiver instalado, os scripts reutilizam
seu Python, mesmo sem `python.exe` no PATH. O setup não muda PATH ou política
permanente de execução.

Para instalar também as duas extensões pelo comando, use `setup.ps1 -InstallExtensions`.
Essa opção instala extensões no perfil do VS Code; não ativa uma licença Wokwi.

4. Use a licença Wokwi já disponível no perfil do VS Code. Se não houver licença
   válida, a ativação é uma etapa manual da conta em **Wokwi: Request a new License**.
   A extensão possui condições de gratuidade e planos pagos. Não há licença nem
   token incluído neste projeto: [extensão oficial](https://marketplace.visualstudio.com/items?itemName=Wokwi.wokwi-vscode)
   e [termos](https://wokwi.com/legal/terms).
5. Execute `F1` → **Wokwi: Start Simulator**. Mantenha a aba visível.

Após o primeiro setup, `Ctrl+Shift+B` executa a tarefa de compilar o ESP32 e o GPS.
Salvar código não compila o firmware. O Wokwi usa os arquivos binários indicados
em `wokwi.toml`.

Em **Terminal → Run Task**, a tarefa **Cerradinho: compilar semanal e GPS** prepara
o intervalo real. **Cerradinho: validar tudo** compila ambos os ambientes e executa
os testes e o linter oficial. O linter consulta o catálogo Wokwi e precisa de rede;
não precisa de token de CI. O resultado da sessão está em
[resultado-validacao.md](resultado-validacao.md).

## O que aparece

O monitor é configurado para abrir sempre, com as ligações UART0 explícitas.
A cada dois segundos aparece uma linha semelhante a:

```text
[vivo] tempo=2 s gps_chars=... fix=valido lacre=integro bateria_sim=78% wifi=on
```

Ao receber o primeiro fix, o dispositivo cria uma referência de localização.
No modo padrão, **30 segundos representam uma semana**. A cada intervalo:

- lê uma posição válida recente, esperando até dez segundos;
- calcula a distância em relação à última amostra válida;
- marca `moved=true` quando a distância supera a tolerância inicial de 100 m;
- mostra JSON na serial e tenta guardar o registro na fila NVS;
- se não houver fix, informa indisponibilidade e preserva a referência válida.

Os 100 m são uma hipótese de demonstração, não um requisito confirmado do TAPI.
O primeiro ponto informa `moved=null`, porque ainda não há comparação possível.
Coordenadas zero são válidas quando existe fix; não são usadas para representar
falha. Eventos de mudança do lacre são registrados separadamente do intervalo.

## Interagir com o circuito

- **GPS:** clique no chip. Altere `latitude`, `longitude` ou `fixValid`.
  Caso sua versão da extensão não exponha os controles, altere `attrs` em
  `diagram.json` e reinicie. A comparação entre sessões só vale se a memória da
  instância tiver sido preservada; prefira controles durante a mesma execução.
- **Lacre:** chave à esquerda = íntegro (GPIO27 em LOW); à direita = violado
  (entrada aberta com pull-up, HIGH). A chave representa somente o sinal digital
  que futuramente poderá vir do sensor infravermelho.
- **Bateria:** gire o potenciômetro. O valor é uma porcentagem sintética de 0 a
  100, não tensão real nem estado de carga de uma célula.
- **Rede:** o ESP32 usa `Wokwi-GUEST`, sem senha. Não há receptor extra no circuito.
  Para simular falta de conexão, crie `include/config.local.h` com um SSID
  inexistente e recompile. Há um modelo em `include/config.local.example.h`.

O firmware **não transmite os JSONs para um servidor**. Wi-Fi conectado não
equivale a entrega confirmada. A fila armazena até **16 registros**; quando cheia,
preserva os antigos e imprime os novos apenas na serial, com aviso. É uma fila
inicial para inspeção, sem confirmação, remoção ou reenvio implementados.

## Intervalo semanal real

`include/config.h` concentra pinos e regras. O ambiente `weekly` usa sete dias
reais e o ambiente `demo`, 30 s. Para o primeiro:

```powershell
.\scripts\build.ps1 -Environment weekly
```

Selecione `wokwi.weekly.toml` em **Wokwi: Select Config File**. Para voltar à demo,
selecione `wokwi.toml`. Ambos usam o mesmo diagrama. O ambiente semanal ainda
mantém o diagnóstico e os componentes ativos: não implementa deep sleep nem
valida autonomia. Esse nome indica apenas o intervalo de coleta.

Cada inicialização dispara uma coleta imediata. O agendamento semanal ainda não
é persistido entre reinícios; a referência de posição é armazenada em NVS.

## Se o monitor ficar vazio

1. Confirme **SUCCESS** ao compilar o ambiente selecionado.
2. Confira se o Wokwi abriu a configuração correta e se a simulação está rodando.
3. Mantenha a aba do simulador visível.
4. Verifique as conexões `esp:TX0 → $serialMonitor:RX` e
   `esp:RX0 → $serialMonitor:TX`. O id da placa neste projeto é `esp`.
5. Observe `[vivo]`, que independe do GPS. Se `gps_chars=0`, confira GPS TX →
   ESP RX2 e o arquivo `chips/gps.chip.wasm`. Se há caracteres sem fix válido,
   confira `fixValid`, taxa de 9600 baud e checksum do modelo.

Não é necessário abrir o monitor de uma porta COM física do PlatformIO para
acompanhar esta simulação. Use o monitor dentro do Wokwi.

## Registrar a saída do simulador

O modo demo oferece a porta RFC2217 local 4000 para diagnóstico. Execute em outro
terminal na raiz, usando o Python/pySerial já fornecidos pelo PlatformIO:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\capture-serial.ps1 -Seconds 240
```

Inicie **Wokwi: Start Simulator** em até 30 segundos, se ainda estiver parado.
A captura acrescenta a saída a `build/simulation-serial.log`, sem enviar comandos
ao firmware. O tempo de captura é de relógio; o tempo mostrado pelo ESP32 é de
simulação. Mantenha o simulador visível. Para capturar a inicialização completa,
inicie a captura antes da simulação. Não pare o Wokwi antes de encerrar a captura.

Se aparecer conexão recusada, pare o simulador e execute novamente **Wokwi: Start
Simulator** para recarregar `wokwi.toml`; o botão de reinício sozinho pode usar a
configuração anterior. Verifique também se a porta 4000 está livre. A configuração
semanal não abre essa porta. A captura é opcional e não depende de token de CI.

Os arquivos `.vscode/*.json` gerados pelo PlatformIO podem conter comentários.
O verificador aceita esse formato JSONC; não é necessário remover os comentários
ou editar `c_cpp_properties.json`/`launch.json` manualmente.

## Arquivos para continuar

- [Modelagem e limites](modelagem-inicial.md): blocos, pinagem e decisões.
- [Instruções para o Codex](instrucoes-codex.md): tarefa de continuidade.
- [Validação](validacao.md): testes automáticos e roteiro visual.
- [AGENT.md](../../AGENT.md): contexto permanente para alterações neste projeto.

## Referências técnicas

- [Wokwi + PlatformIO](https://docs.wokwi.com/vscode/platformio)
- [Configuração, firmware e chips customizados](https://docs.wokwi.com/vscode/project-config)
- [Compilação de Custom Chips](https://docs.wokwi.com/guides/custom-chips-to-wasm)
- [Controles e atributos do GPS simulado](https://docs.wokwi.com/chips-api/attributes)
- [Monitor serial](https://docs.wokwi.com/guides/serial-monitor)
- [Linter oficial do diagrama](https://docs.wokwi.com/wokwi-ci/cli-usage#linting-diagrams)
- [NVS/Preferences do ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/preferences.html)
