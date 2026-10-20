# Validação da modelagem inicial

## Verificações automatizadas

Execute o setup e compile os ambientes `demo` e `weekly`, depois:

```powershell
.\scripts\verify.ps1
```

Os testes compilam e executam o mesmo código matemático e de geração NMEA usado
pelo firmware/modelo, em WebAssembly com WASI. Verificam coordenadas fora da faixa,
NaN, coordenada zero válida, distância nula, distância conhecida, antimeridiano,
antípodas, deslocamentos abaixo/acima da tolerância, arredondamento de minutos,
checksum, indicador de fix e rejeição de coordenada inválida no gerador.

O verificador do projeto confere ligações seriais, pinagem compartilhada,
controles/atributos do chip e existência/cabeçalhos dos artefatos compilados.
Também confere todas as alimentações, console expandido, baud rates, recomendações
das extensões e os ambientes Arduino/ESP32/TinyGPSPlus. Aceita JSONC produzido
pelo PlatformIO no VS Code. `verify.ps1` termina executando o verificador oficial
`wokwi-cli lint --warnings-as-errors`, que precisa de rede para atualizar o catálogo.
Esses testes não executam o ESP32 nem comprovam entrega via rede ou consumo.

## Roteiro no Wokwi

| Caso | Ação | Resultado esperado |
|---|---|---|
| Console | Iniciar demo e aguardar 2 s | `[boot]` e `[vivo]`, mesmo sem GPS |
| Primeiro fix | Usar coordenadas padrão e fix=1 | `location_sample`, `baseline_created=true`, `moved=null` em memória limpa |
| Parado | Manter posição por mais de 30 s | `moved=false`, distância próxima de zero |
| Mudou de local | Alterar latitude de -23.5500 para -23.5400 e esperar coleta | Distância maior que 1 km, `moved=true` |
| Próxima referência | Manter essa nova coordenada e aguardar outra coleta | `moved=false` |
| GPS indisponível | fix=0; esperar próxima coleta e timeout de 10 s | `location_unavailable`, `moved=null`; referência preservada |
| GPS recuperado | fix=1 e manter/alterar posição | Compara com a última amostra válida |
| Lacre | Mover chave à direita | `seal_changed`, `seal=violated` sem esperar 30 s |
| Lacre fechado | Voltar à esquerda | Novo evento com `seal=intact`, sem apagar o histórico |
| Bateria | Girar potenciômetro | `battery_sim_pct` e heartbeat mudam |
| Offline | Usar SSID inexistente e recompilar | Diagnóstico e registro local continuam sem Wi-Fi |
| Fila cheia | Produzir mais de 16 registros | Avisa limite, preserva antigos; não afirma envio |

Se a versão da extensão não disponibilizar os controles do Custom Chip, documente
essa limitação e use atributos no diagrama para testar entradas fixas. Não diga
que testou deslocamento entre coletas se houve reinicialização com perda de flash.

Nesta sessão, a extensão 4.0.1 disponibilizou os três controles durante a execução.
Para guardar evidência textual, use `scripts/capture-serial.ps1` conforme o README.
O log identifica cada reinício por `[boot]`; compare posições apenas dentro da
mesma execução. A captura não verifica o resultado automaticamente. Preserve o
log e registre os `uptime_ms` dos eventos no relatório.

O debounce implementado é de 60 ms. Alternar a chave lentamente comprova os
eventos funcionais; não caracteriza a rejeição de pulsos de duração inferior a
60 ms. Esse ensaio temporal e o comportamento do sensor físico são verificações
separadas. O diagrama atual desativa o bounce artificial da chave.

## Persistência e tempo

A referência e os registros usam NVS. A preservação da flash entre execuções do
simulador depende do ambiente; este projeto não configura persistência de flash
externa. No hardware físico, também é necessário testar falta de energia durante
a gravação. O código não formata nem limpa a fila automaticamente.

Sem relógio UTC sincronizado, os registros usam uptime identificado. Não usar
esse contador para calcular dias de permanência ou data real da última posição.

## Resultado desta preparação

Consulte `document/modelagem-inicial/resultado-validacao.md` para os comandos executados e resultados
observados na preparação. Esse relatório deve ser atualizado com evidências reais,
sem substituir limitações por hipóteses de funcionamento.
