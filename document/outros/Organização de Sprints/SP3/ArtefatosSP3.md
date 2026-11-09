## Artefato 8

Documentação: Preencher seção 3.4.3


Instruções: Registre diversas situações de uso de seu sistema como um todo, indicando exemplos de ação do usuário e resposta do sistema, apontando como o ambiente deverá estar configurado para receber a ação e produzir a resposta. Estes registros serão utilizados para testar seu sistema, portanto, descreva várias situações, incluindo não apenas casos de sucesso, mas também de falha nos comportamentos do sistema.
Siga as nomenclaturas e convenções já utilizadas anteriormente, e não se esqueça dos alinhamentos de negócios e experiência do usuário para pensar em situações representativas. Inclua figuras do protótipo físico e de telas do broker (ou dashboard, se houver), além de descrições dos testes realizados para ilustrar o funcionamento do protótipo.


CRITÉRIOS DE QUALIDADE:
SEM EVIDÊNCIAS [0%] 
O grupo não usou ESP32 no protoboard com organização dos fios de conexão; o grupo não montou um protótipo com botões e LEDs; o grupo não montou um protótipo com display usando comunicação I2C; o protótipo não se subscreve em ao menos um tópico MQTT; o protótipo não publica ao menos em um tópico MQTT; o código não possui nomes descritivos de funções e variáveis, procedimentos encapsulados em funções com comentários. Não há casos de teste descritos.


INSUFICIENTE [até 25%]
O grupo usou ESP32 no protoboard com organização dos fios de conexão; o artefato possui ao menos 2 saídas informacionais em LEDs, ao menos 1 entrada de dados booleana por meio de botão (exemplo, menu de opções ou RESET ou confirmações) com debouncing por hardware; o grupo não montou um protótipo com display usando comunicação I2C; o protótipo não se subscreve em ao menos um tópico MQTT; o protótipo não publica ao menos em um tópico MQTT; o código não possui nomes descritivos de funções e variáveis, procedimentos encapsulados em funções com comentários. Os casos de teste não estão descritos de forma objetiva ou o protótipo não consegue passar nos casos de teste descritos.


BÁSICO [até 50%]
O grupo usou ESP32 no protoboard com organização dos fios de conexão; o artefato possui ao menos 2 saídas informacionais em LEDs, ao menos 1 entrada de dados booleana por meio de botão (exemplo, menu de opções ou RESET ou confirmações) com debouncing por hardware; o grupo montou um protótipo com display usando comunicação I2C; o protótipo não se subscreve em ao menos um tópico MQTT; o protótipo não publica ao menos em um tópico MQTT; o código não possui nomes descritivos de funções e variáveis, procedimentos encapsulados em funções com comentários. Os casos de teste não estão descritos de forma objetiva ou o protótipo não consegue passar nos casos de teste descritos.


INTERMEDIÁRIO [até 75%]
O grupo usou ESP32 no protoboard com organização dos fios de conexão; o artefato possui ao menos 2 saídas informacionais em LEDs, ao menos 1 entrada de dados booleana por meio de botão (exemplo, menu de opções ou RESET ou confirmações) com debouncing por hardware; o grupo montou um protótipo com display usando comunicação I2C; o protótipo se subscreve em ao menos um tópico MQTT; o protótipo publica ao menos em um tópico MQTT; o código não possui nomes descritivos de funções e variáveis, procedimentos encapsulados em funções com comentários. Os casos de teste estão descritos de forma objetiva e o protótipo consegue passar nos casos de teste descritos.


AVANÇADO [até 100%]
O grupo usou ESP32 no protoboard com organização dos fios de conexão; o artefato possui ao menos 2 saídas informacionais em LEDs, ao menos 1 entrada de dados booleana por meio de botão (exemplo, menu de opções ou RESET ou confirmações) com debouncing por hardware; o grupo montou um protótipo com display usando comunicação I2C; o protótipo se subscreve em ao menos um tópico MQTT; o protótipo publica ao menos em um tópico MQTT; o código possui nomes descritivos de funções e variáveis, procedimentos encapsulados em funções com comentários. Os casos de teste estão descritos de forma objetiva e o protótipo consegue passar nos casos de teste descritos.


Observação: lembrem-se que critérios de qualidade não são uma checklist! Os professores podem cobrar conhecimento prévio de outras sprints e até de outros módulos.

## Artefato 9

Cada grupo deve elaborar o Manual de Instruções do projeto a partir do template presente no repositório. Neste artefato será entregue a versão inicial do manual, que deve contemplar as partes introdutórias (nome do projeto, parceiro, grupo e histórico) e a seção 1.1, contendo todos os componentes e respectivas quantidades levantados até o momento para a solução. Nas próximas sprints haverá incrementos das demais seções e revisões desta entrega conforme apontamentos dos professores. 

Sigam o template em Markdown disponível já no seu repositório (https://git.inteli.edu.br/templates/primeiro-ano/in04/template_m4/-/blob/main/document/TemplateManual.md)
Escrevam pensando em quem vai usar o dispositivo, e não em quem o desenvolveu.
Serão avaliadas a presença das seções previstas e a atualização do histórico de revisões, a clareza e a objetividade dos textos, a qualidade das ilustrações, tabelas e sua referência no texto, a coerência do manual com a solução desenvolvida e sua adequação a quem vai reproduzir a solução e usar o dispositivo. 

## Artefato 10

Descrição
Documentação: Preencher seções 2 e 3.3.5 da documentação.

Instruções: Para a seção 2, descreva as etapas da metodologia RM-ODP que foram utilizadas para o desenvolvimento, citando o referencial teórico. Você deve apenas enunciar os métodos, sem dizer ainda como ele foi aplicado e quais resultados obtidos.


Para a seção 3.3.5, descreva a arquitetura técnica da solução de forma detalhada (visão de arquitetura). Justifique como a arquitetura suporta os requisitos funcionais e não funcionais. Grave um vídeo descrevendo a arquitetura da solução em blocos e justificando como a arquitetura suporta os requisitos funcionais e não funcionais. Envie o vídeo publicando em Google Drive (ou similar) e compartilhe o link.

CRITÉRIOS DE QUALIDADE


O diagrama de arquitetura e o vídeo devem:
- mostrar microcontroladores, incluindo descrições de sua função no sistema (por exemplo: "Irá processar o sinal dos sensores a cada X minutos")
- mostrar sensores, incluindo descrição de função e especificações técnicas do tipo de informação que será coletada
- mostrar apresentadores de informação (displays), incluindo descrição de que tipo de informação será apresentada (por exemplo, "Mostrar temperatura dos sensores")
- mostrar atuadores, caso existam na solução, incluindo descrições do que irão acionar (por exemplo, "Ligar motor de irrigação durante x minutos")
- mostrar o broker MQTT e o dashboard que é a interface do usuário - mostrar ligações entre os elementos (com fio ou sem fio) - incluindo conexões com sensores e atuadores, conexão com WiFi, entre outros
- mostrar como a arquitetura suporta os requisitos funcionais e não funcionais

SEM EVIDÊNCIAS [0%]
A metodologia e o vídeo NÃO foram entregues dentro do prazo pelo grupo de alunos.

INSUFICIENTE [até 25%]
A metodologia descreve corretamente apenas os títulos das etapas do RM-ODP, mas não as etapas de forma detalhada. No vídeo, o grupo demonstra o diagrama da arquitetura da solução em blocos, porém esta figura não mostra microcontroladores (incluindo descrições), nem sensores (incluindo descrição e especificações técnicas), nem displays (incluindo descrição), não mostra o broker MQTT nem o dashboard/interface do usuário, e não explica as ligações entre os elementos. No vídeo, o grupo também não explica como a arquitetura suporta os requisitos funcionais e não funcionais.

BÁSICO [até 50%]
A metodologia descreve corretamente as etapas do RM-ODP de forma detalhada. No vídeo, o grupo demonstra o diagrama da arquitetura da solução em blocos, mostrando microcontroladores (incluindo descrições) e sensores (incluindo descrição e especificações técnicas), mas não mostra displays (incluindo descrição ), não mostra o broker MQTT e o dashboard/interface do usuário, e não explica as ligações entre os elementos. No vídeo, o grupo também não explica como a arquitetura suporta os requisitos funcionais e não funcionais.

INTERMEDIÁRIO [até 75%]
A metodologia descreve corretamente as etapas do RM-ODP de forma detalhada. No vídeo, o grupo demonstra o diagrama da arquitetura da solução em blocos, mostrando microcontroladores (incluindo descrições), sensores (incluindo descrição de função e especificações técnicas), mostra displays (incluindo descrição), mostra o broker MQTT e o dashboard/interface do usuário, mas não explica as ligações entre os elementos. No vídeo, o grupo também não explica como a arquitetura suporta os requisitos funcionais e não funcionais.

AVANÇADO [até 100%]
A metodologia descreve corretamente as etapas do RM-ODP de forma detalhada. No vídeo, o grupo demonstra o diagrama da arquitetura da solução em blocos, mostrando microcontroladores (incluindo descrições), sensores (incluindo descrição de função e especificações técnicas), displays (incluindo descrição de que tipo de informação será apresentada), o broker MQTT e o dashboard que é a interface do usuário, além de explicar de forma detalhada as ligações entre os elementos. No vídeo, o grupo também explica como a arquitetura suporta os requisitos funcionais e não funcionais.


Observação: lembrem-se que critérios de qualidade não são uma checklist! Os professores podem cobrar conhecimento prévio de outras sprints e até de outros módulos.


## Artefato 11

Descrição
Para a realização do Artefato de Física – Circuito RC, cada grupo deverá elaborar um relatório técnico-experimental em formato markdown, a ser incluído na documentação do projeto como ANEXO (crie uma seção "6. Anexos" ao final de seu documento e posicione a entrega dentro desta seção).


Para descrição do Artefato, acesse o seguinte link:
https://drive.google.com/file/d/1tWtIaKf3IM3JIPH30g9S8Dw9lX_doyTs/view?usp=sharing  


Recomenda-se seguir as seguintes sugestões para a elaboração do relatório:
1) Fundamentação Teórica
Nesta seção, seu grupo deve demonstrar que compreende a física por trás do experimento. Comece explicando o que acontece com a tensão em um capacitor Vc enquanto ele carrega. Apresente a fórmula (equação teórica) que descreve Vc em função do tempo. Em seguida, defina o que é a constante de tempo (τ) e, o mais importante, explique o seu significado físico (por exemplo, o que ela nos diz sobre a velocidade de carga e qual a porcentagem da carga total atingida nesse tempo).
2) Projeto e Cálculo
Aqui é onde você detalhará a matemática do seu projeto. Comece mostrando a resistência equivalente do circuito. Usando C = 2200 µF e o objetivo de τ aproximadamente igual a 60 s, calcule qual deveria ser o valor de R2, dado o R1 que seu grupo recebeu (Tabela 1). Em seguida, compare seu R2 calculado com os valores disponíveis na Tabela 2 e escolha o valor real mais próximo. Por fim, recalcule a constante de tempo teórica (τ) final do seu circuito, usando os valores reais de R1 e o R2 que você selecionou. É importante que todos os cálculos sejam mostrados passo a passo.
3) Montagem do Protótipo
Esta seção é a evidência visual do seu trabalho prático. Tire fotos claras e bem iluminadas do seu circuito montado na protoboard. As fotos devem mostrar como os resistores e o capacitor estão conectados entre si e como o circuito está conectado à fonte de 5V do microcontrolador. A legibilidade é essencial para que os corretores possam verificar a montagem.
4) Coleta de Dados Experimentais
Aqui você apresentará os dados brutos coletados. Organize os dados em uma tabela limpa, com colunas para "Tempo" e "Tensão no Capacitor". Certifique-se de que sua tabela contenha pelo menos 50 pontos de dados e que a coleta tenha seguido a recomendação de priorizar os momentos iniciais (mais pontos no primeiro minuto). Preste atenção às unidades e aos algarismos significativos.
5) Gráfico Experimental
Nesta etapa, você irá visualizar os dados da sua tabela. Use um software (como Google Sheets ou Excel) para criar um gráfico de dispersão (scatter plot) dos seus dados experimentais. O gráfico deve ter um título descritivo e os eixos devem estar claramente rotulados com seus nomes e unidades.
6) Análise Comparativa
Agora, você vai unir a teoria (Item 2) e a prática (Item 5). No mesmo gráfico que você criou no Item 5, adicione a curva da função teórica Vc(t). Esta curva deve usar a equação do Item 1 e o valor da constante de tempo teórica (τ) que você calculou no final do Item 2. O resultado deve ser um único gráfico mostrando seus pontos de dados experimentais e a linha suave da curva teórica.
7) Interpretação e Conclusão
Esta é a seção mais importante para a análise. Comece discutindo o gráfico do Item 6: seus pontos experimentais se ajustaram bem à curva teórica? Comente quaisquer discrepâncias. Em seguida, determine a sua constante de tempo experimental e compare-a com o valor teórico calculado no Item 2. Finalmente, discuta as fontes potenciais de erro que podem ter causado diferenças, como a tolerância dos resistores e do capacitor, ou atrasos na medição.
8) Vídeo demonstrativo
Faça um vídeo narrado de até 5 minutos, apresentando sua implementação. Comente sobre o projeto e cálculo, a montagem experimental, coleta dos dados, resultados e interpretações. Procure mostrar sua tela e o circuito montado com clareza, seja na imagem e no audio. Revise seu vídeo antes de publicar. Coloque o vídeo em um Google Drive e escreva o link ao final do relatório. 