# Preparação (uma vez, antes dos CTs)

## Material
- Protótipo de 3 botões (ON/OFF, AVANÇAR, VOLTAR) com LCD I2C e buzzer, ligado ao notebook por **cabo USB curto e de dados, em porta direta** (sem hub).
- Celular com hotspot de **2,4 GHz** (para derrubar a rede no CT72) ou um roteador que você possa desligar.
- MQTT Explorer e acesso ao console do EMQX Cloud.
- Um cronômetro (celular).

## 1. Firmware
1. `include/secrets.h` com Wi-Fi 2,4 GHz, host, usuário e senha do EMQX, porta 8883, `MQTT_TLS 1` e **`MQTT_CLIENT_ID` e `MQTT_DEVICE_ID` próprios** (anote o `MQTT_DEVICE_ID`: ele é o `<ID>` de todos os tópicos).
2. Gravar o ambiente `bancada-3botoes`:
   ```bash
   pio run -e bancada-3botoes -t upload
   ```
3. Abrir o monitor serial a 115200:
   ```bash
   pio device monitor -b 115200
   ```

## 2. MQTT Explorer
1. Conexão com o host do deployment, porta 8883, TLS ligado, usuário e senha **MQTT** do deployment (não o e-mail da conta).
2. Em *Advanced*, retire `#` e `$SYS/#` da lista e assine `guaratracking/#`.
3. Para publicar, use o painel *Publish* com o tópico `guaratracking/<ID>/cmd`, QoS 0 e formato *raw* ou *json*. **Sempre com texto no corpo**: sem texto o comando chega vazio.

## 3. Console do EMQX
Abra a lista de clientes (*Monitoring > Clients*) e deixe aberta.

## 4. Antes de cada CT
- Confira na Serial: ciclo, lacre e fila (`fila=0`) no estado esperado.
- Se o LCD estiver apagado, o primeiro toque só acende.

## Cuidados com as evidências
- Mascare **host, usuário e SSID** nos prints. Nunca salve `secrets.h`.
- Salve com os nomes da tabela de cada CT, em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`, em camelCase, como pede o AGENT.md.
- Use `NF` ou `VINCULAR` pela Serial só quando o roteiro mandar.

## Se algo der errado
| Sintoma | O que fazer |
|---|---|
| `Brownout detector was triggered` em loop | Trocar o cabo, porta direta; capacitor de 470 a 1000 µF entre VIN e GND, ou fonte de 5 V e 2 A |
| `[mqtt] falha na conexao, estado=5` | Usuário ou senha errados (use o MQTT do deployment, não o e-mail) |
| `estado=-2` ou sem Wi-Fi | Rede de 5 GHz ou porta 8883 bloqueada: trocar de rede |
| Comando sem resposta | Verifique o tópico `.../cmd` com o `MQTT_DEVICE_ID` certo e se o corpo tem texto |
