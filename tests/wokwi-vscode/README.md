# Rodada manual no Wokwi for VS Code

Execução dos CTs do V2 que não couberam no Wokwi CLI, cujo plano gratuito limita cada conexão a 5 min e tem cota mensal de minutos de CI. Os mesmos cenários de [`tests/wokwi-ci/`](../wokwi-ci/) são feitos à mão no VS Code. A Serial é gravada pela porta RFC2217 local 4000, e os logs são conferidos pelas mesmas funções do `run_cts.py`.

| Arquivo | Uso |
|---|---|
| `capturar.py` | Grava um log por cenário em `build/wokwi-ct/` e envia sozinho os comandos `REMESSA` (CT44) e `NF` (CT14–CT16). |
| `preparar.py` | Gera `CT12/`, `CT43/` e `CT45/` (diagrama e `wokwi.toml`) a partir do diagrama de produção. |
| `bateria-ct12.chip.c` | Chip ligado ao GPIO34 no lugar do potenciômetro. A partir de 6 s, fica 3,5 s em cada nível: 17 → 22 → 17 → 31 → 17 → 0 → 50 → 100%. |
| `gps-ct45.chip.c` | GPS com estímulos no tempo: sem fix até 60 s e rede indisponível (GPIO18 em nível alto) até 110 s. Substitui o GPS e a chave de rede. |

Os chips existem porque os estímulos manuais não foram reproduzíveis:

- **CT12/CT13:** o potenciômetro arrastado não parava em 22% e passava várias vezes abaixo de 20%.
- **CT45:** a rede voltava antes do fix, e o `GNSS_RECUPERADO` não passava pela fila.

No Wokwi, a entrada analógica do ESP32 chega a 100% perto de 5,08 V, valor medido nesta rodada. Por isso, o chip de bateria usa 5,1 V como 100%.

## Preparação

Depois de compilar o firmware `demo`:

```powershell
& $python tests/wokwi-vscode/preparar.py
$env:WASI_SDK_PATH = Join-Path $PWD '.tools/wasi-sdk'
& ./.tools/wokwi-cli.exe chip compile tests/wokwi-vscode/bateria-ct12.chip.c -o build/bateria-ct12.chip.wasm
& ./.tools/wokwi-cli.exe chip compile tests/wokwi-vscode/gps-ct45.chip.c -o build/gps-ct45.chip.wasm
```

## Execução

1. Inicie `capturar.py` com os nomes dos cenários, antes de iniciar a simulação.
2. No VS Code, rode **Wokwi: Select Config File** e escolha a config do cenário:
   - `src/firmware/prototipo_wokwi/wokwi.toml` para os cenários manuais;
   - `tests/wokwi-vscode/CTxx/wokwi.toml` para CT12, CT43 e CT45.
3. Confira a config no circuito antes de seguir:
   - **CT12:** aparece o bloco "Bateria do CT12" acima do potenciômetro;
   - **CT43:** o GPS mostra `fixValid` = 0;
   - **CT45:** o GPS tem cinco pinos, entre eles `NET`.
4. Faça os passos do cenário, sem mexer nas chaves trocadas por chip.
5. Pare a simulação. A gravação passa ao próximo nome quando a porta fecha ou depois de 180 s sem dados.
6. Confira os logs com:

   ```powershell
   & $python tests/wokwi-ci/run_cts.py --checar <cenário>
   ```

Na rodada de 03/11/2026, a conexão RFC2217 às vezes continuou aberta depois de um reinício da simulação. Quando isso acontece, o log junta as tentativas, e o resultado vale só a partir do último boot (`ets Jul`). Foi assim no log do CT11.
