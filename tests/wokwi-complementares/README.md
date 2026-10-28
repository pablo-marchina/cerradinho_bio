# Ensaios complementares da Sprint 1

Instrumentação isolada para executar CT18–CT22 no Wokwi. `main.cpp` inclui o
firmware de produção e chama suas funções originais. A sonda acrescenta somente
leitura das chaves NVS e `ESP.restart()` real; não altera regras, constantes,
esperados ou eventos de negócio. `estimulos.chip.c` fornece níveis digitais no
lacre/rede/autorização e sentenças NMEA pela UART2. A UART0 continua disponível
para a captura Serial.

O gerador observa o primeiro byte de cada JSON `seal_changed` na UART0 e registra
a diferença em relação à borda digital, usando o mesmo relógio virtual do Wokwi.
As linhas `[ensaio]` são instrumentação, separadas dos JSON do firmware. As sondas
NVS leem os valores armazenados com `Preferences`; não substituem conteúdo. A
instrumentação acrescenta processamento, portanto as latências caracterizam este
ensaio simulado e não o desempenho físico do dispositivo.

`CT18-original` repete os estímulos usando o binário original do projeto, sem a
sonda no loop do ESP32. Na primeira rodada instrumentada, a associação de parte
das latências sem rede ficou inválida; esses valores foram descartados no
resultado, preservando o log bruto. O observador atual fixa a borda ao receber o
primeiro byte da linha. A contraprova original confirma a contagem de eventos,
sem utilizar essas latências inválidas. As mensagens `NOT_FOUND` nas sondas de
chaves vazias representam ausência da chave na NVS.

Preparação (após `scripts/setup.ps1` e compilação do projeto principal):

```powershell
$python = "$env:USERPROFILE/.platformio/penv/Scripts/python.exe"
$pio = "$env:USERPROFILE/.platformio/penv/Scripts/platformio.exe"
& $python tests/wokwi-complementares/preparar.py
$env:WASI_SDK_PATH = Join-Path $PWD '.tools/wasi-sdk'
& ./.tools/wokwi-cli.exe chip compile tests/wokwi-complementares/estimulos.chip.c -o build/estimulos.chip.wasm
& $pio run -d tests/wokwi-complementares -e demo
```

Selecionar `tests/wokwi-complementares/CTxx/wokwi.toml` com **Wokwi: Select Config
File**. Fechar a aba de simulação anterior e iniciar uma nova para carregar os
binários. Capturar a Serial desde o boot pela porta RFC2217 local 4000. Uma rodada
nova deve começar com `q0..q15` ausentes e contexto não restaurado; CT22 usa
`ESP.restart()` dentro da mesma rodada para preservar a flash.

| Caso | Estímulos no tempo virtual do simulador |
|---|---|
| CT18 | Sem fix durante toda a rodada. Pulsos de 20/40 ms em 3/3,2 s; dez pares de níveis estáveis de 200 ms entre 4 e 7,8 s. Repetição em 43/43,2 e 44–47,8 s, com rede indisponível desde 42 s. Final em 60 s. |
| CT19 | Sem fix até 20 s; válido até 55 s; NMEA com fix inválido até 90 s; válido até 120 s; UART sem NMEA de 120 a 160 s. Lacre abre/fecha em 6/6,5, 76/76,5 e 146/146,5 s. Final em 185 s. |
| CT20 | A desde o boot; B com latitude +0,00045° desde 20 s; C com +0,0018° em relação a A desde 50 s. Final em 70 s. |
| CT21 | Uma coleta válida automática e 16 transições do lacre entre 4 e 19 s. Sondas NVS em 21/28 s; rede indisponível em 24 s e disponível em 26 s. Final em 30 s, antes da segunda coleta automática. |
| CT22 | A desde o boot; rede indisponível em 20 s; lacre aberto em 25 s; NMEA interrompido de 35 a 65 s; sonda em 40 s; `ESP.restart()` em 44 s; sondas em 60/98 s; B com latitude +0,00135° desde 65 s; rede disponível em 100 s. Final em 105 s. |

Os diagramas gerados preservam ESP32, potenciômetro, LEDs e resistores do circuito
principal. Somente os três fios de entrada controlados pelo gerador são separados
das chaves/botão visuais; o GPIO32 é exclusivo da sonda. Os binários e o cabeçalho
SDK copiado são gerados localmente e ignorados pelo Git. Evidências e resultados
ficam em `assets/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/` e na seção 3.4.1 da documentação principal.
