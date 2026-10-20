# Inteli - Instituto de Tecnologia e Liderança 

<p align="center">
<a href= "https://www.inteli.edu.br/"><img src="assets/inteli.png" alt="Inteli - Instituto de Tecnologia e Liderança" border="0" width=40% height=40%></a>
</p>

<br>

# Nome do projeto

## Nome do grupo

## 👨‍🎓 Integrantes: 
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 1</a>
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 2</a>
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 3</a> 
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 4</a> 
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 5</a>
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 6</a> 
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 7</a>

## 👩‍🏫 Professores:
### Orientador(a) 
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 1</a>
### Instrutores
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 2</a>
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 3</a> 
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 4</a> 
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 5</a>
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 6</a> 
- <a href="https://www.linkedin.com/in/victorbarq/">Nome do integrante 7</a>

## 📜 Descrição

Descreva seu projeto (até 600 palavras)

Inclua um link para o vídeo de demonstração do funcionamento de seu projeto (sprint 5)


## 📁 Estrutura de pastas

Dentre os arquivos e pastas presentes na raiz do projeto, definem-se:

- <b>assets</b>: aqui estão os arquivos relacionados a parte gráfica do projeto, ou seja, as imagens e vídeos que os representam (O logo do grupo pode ser adicionado nesta pasta).

- <b>document</b>: aqui estão todos os documentos do projeto, incluindo o manual de instruções (se aplicável). Há também uma pasta denominada <b>outros</b> onde estão presentes outros documentos complementares.

- <b>src</b>: Todo o código fonte criado para o desenvolvimento do projeto, incluindo firmware, notebooks, backend e frontend, se aplicáveis.

- <b>src/firmware</b>: firmware ESP32 da modelagem inicial de localização, lacre e bateria simulada.

- <b>include</b>: configuração centralizada e funções de cálculo de distância.

- <b>chips</b>: fontes do GPS/GNSS customizado para o Wokwi.

- <b>scripts</b> e <b>tests</b>: preparação de ferramentas, compilação e verificações da modelagem.

- <b>document/modelagem-inicial</b>: guia do setup, arquitetura, roteiro de testes e resultados.

- <b>README.md</b>: arquivo que serve como guia e explicação geral sobre o projeto (o mesmo que você está lendo agora).

## 🔧 Instalação

O firmware utiliza **ESP32 DevKit v1** (placa `esp32dev` no PlatformIO),
framework **Arduino**, **Espressif32 6.12.0** e **TinyGPSPlus 1.0.3**.
O circuito Wokwi tem GPS/GNSS customizado, chave para o sinal do lacre,
potenciômetro para bateria simulada, Wi-Fi integrado e monitor serial.

1. Instale o [Visual Studio Code](https://code.visualstudio.com/).
2. Abra a pasta raiz deste repositório (`g01`), onde está o `platformio.ini`.
3. Instale as extensões recomendadas **PlatformIO IDE** e **Wokwi Simulator**.
4. Execute os comandos abaixo em um terminal PowerShell na raiz.
5. Use a licença Wokwi disponível no seu perfil. A ativação, se necessária, é
   manual; nenhuma licença ou token está incluído neste repositório.
6. Após a compilação, execute **F1 → Wokwi: Start Simulator**.

O setup reaproveita ferramentas instaladas e mantém dependências locais em
`.tools/`. Para uma instalação nova, disponibilize Python 3.10+ e Node.js 22+.

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\setup.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Environment weekly
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\verify.ps1
```

O modo `demo` consulta a posição a cada 30 s; `weekly`, a cada sete dias.
O primeiro fix válido cria a referência. Os seguintes são comparados pela distância,
com tolerância inicial de 100 m, hipótese a validar. Sem fix, a referência é
preservada. O lacre gera eventos com debounce, independentemente da coleta.
O heartbeat aparece a cada dois segundos, mesmo sem GPS ou internet.

`include/config.h` concentra pinos e parâmetros; `src/firmware/main.cpp` contém
o firmware. `diagram.json` e `wokwi.toml` abrem a demonstração. Selecione
`wokwi.weekly.toml` em **Wokwi: Select Config File** para o intervalo real.
**Ctrl+Shift+B** compila ESP32 e GPS; **Cerradinho: validar tudo** compila os dois
ambientes e executa os testes e o linter oficial. Salvar código não recompila.

A fila NVS tem 16 registros e preserva os antigos quando cheia. Não há transporte
ao servidor ou confirmação de entrega. A bateria é sintética; o intervalo semanal
não comprova economia de energia. Sensor IR físico, conectividade externa,
alimentação, deep sleep e autonomia permanecem como etapas futuras.

Consulte o [guia completo do setup](document/modelagem-inicial/README.md),
a [modelagem e pinagem](document/modelagem-inicial/modelagem-inicial.md) e os
[resultados de validação](document/modelagem-inicial/resultado-validacao.md).

Posicione aqui um link para o Manual de Instruções. (Link do Template: https://docs.google.com/document/d/1HqNJ_Ex2HnDIw98rVrJIXhpvu9lmujeO/edit) (Sprint 4)


## 🗃 Histórico de lançamentos

* 0.5.0 - XX/XX/2023
    * 
* 0.4.0 - XX/XX/2023
    * 
* 0.3.0 - XX/XX/2023
    * 
* 0.2.0 - XX/XX/2023
    * 
* 0.1.0 - XX/XX/2023
    *

## 📋 Licença/License

