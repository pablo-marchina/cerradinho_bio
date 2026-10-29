# BOM detalhada — Sprint 1 (estimativa)

> Valores de referência, não pedido de compra ou orçamento homologado. Fonte primária: [planilha editável](../../assets/negocios/BOM-GuaraTracking-Sprint1.xlsx). Conferir preços, disponibilidade, frete, impostos, consumíveis, compatibilidade e quantidade do piloto antes de adquirir.

### 3.1.7. Bill of Material (BOM) (sprint 1)

BOM preliminar de um dispositivo, organizada com os campos da planilha do Inteli [42]. Os valores são preços de referência consultados em 22 e 23/10/2026, em reais; preços em dólar foram convertidos pela PTAX de venda de 22/10/2026 (R$ 5,0383) [35]. Não incluem frete, impostos de importação nem encargos, e os modelos serão confirmados antes da construção do protótipo físico [6]. O detalhamento, com fonte, data e estado de cada cotação, está na [planilha da BOM](../../assets/negocios/BOM-GuaraTracking-Sprint1.xlsx).

| Categoria | Quantidade | Referência | Código / modelo do fabricante | Valor unitário (R$) | Valor total (R$) |
|---|---:|---|---|---:|---:|
| Microcontrolador | 1 | U1 | ESP32 DevKit V1 (ESP-WROOM-32) [43] | 51,00 | 51,00 |
| GNSS | 1 | U2 | Quectel LC76G-PA / LC76GPATA [34, 35] | 247,38 | 247,38 |
| Antena GNSS | 1 | ANT1 | Antena ativa 28 dB, conector SMA [44] | 73,81 | 73,81 |
| Bateria | 1 | BT1 | LiPo 3,7 V 2000 mAh com proteção [45] | 62,93 | 62,93 |
| Alimentação | 1 | U3 | Módulo carregador TP4056 com proteção [46] | 8,91 | 8,91 |
| Sensor do lacre | 1 | S1 | Reed switch com ímã MC-38 [47] | 7,35 | 7,35 |
| Gabinete | 1 | ENC1 | Caixa de junção ABS 100 × 68 × 50 mm [48] | 55,37 | 55,37 |
| Fixação | 4 | FIX1 | Abraçadeira de nylon 300 mm (pacote de 100) [49] | 0,29 | 1,16 |
| Comunicação externa | 1 | COM1 | Módulo SIM7080G (LTE-M/NB-IoT), opção a validar [50] | 195,69 | 195,69 |
| PCI | 1 | PCB1 | Placa fenolite perfurada 10 × 10 cm [51] | 15,66 | 15,66 |
| LEDs | 4 | D1–D4 | LED 5 mm verde, amarelo, vermelho e azul [52] | 0,25 | 1,00 |
| Resistores | 4 | R1–R4 | Resistor 220 Ω 1/4 W [52] | 0,15 | 0,60 |
| Botões | 2 | SW1–SW2 | Chave tátil 6 × 6 mm [52] | 0,30 | 0,60 |
| Chaves | 3 | SW3–SW5 | Mini chave liga-desliga [53] | 0,78 | 2,34 |
| Potenciômetro | 1 | RV1 | Potenciômetro linear 10 kΩ (bateria simulada na bancada) [53] | 1,79 | 1,79 |
| Cabos | 1 | J1 | Kit jumper macho-fêmea 40 peças [52] | 8,40 | 8,40 |
| **Total de materiais** |  |  |  |  | **733,99** |

O GNSS estava sem estoque na consulta, e a comunicação externa depende do teste de cobertura nas oficinas, por isso o SIM7080G é uma das opções e não a escolha final. LEDs, botões, chaves e potenciômetro reproduzem o circuito do Wokwi para a bancada.

**Mão de obra estimada:** 40 dias, 2 horas/dia por pessoa (80 h); referências salariais, não cotações [36–39].

| Perfil (CBO) | Pessoas | R$/h | Subtotal (R$) |
|---|---:|---:|---:|
| Engenheiro de hardware computacional (2122-10) | 3 | 68,47 | 16.432,80 |
| Pesquisador em ciências da computação (2031-05) | 2 | 54,95 | 8.792,00 |
| Analista de sistemas (2124-05) | 1 | 40,28 | 3.222,40 |
| Engenheiro de software (2122-15) | 1 | 70,73 | 5.658,40 |
| **Total** | **7** |  | **34.105,60** |

**Total estimado do protótipo:** R$ 34.839,59 (R$ 733,99 de materiais e R$ 34.105,60 de mão de obra), sem frete, tributos e encargos. Para cinco dispositivos, o custo não é cinco vezes o unitário, porque a mão de obra e parte dos materiais de bancada são compartilhados.

## Referências e rastreabilidade das cotações

As referências **[34]–[53]** usadas neste anexo correspondem aos itens de mesmo número na **seção 5 de `document/documentacao.md`**; esta vinculação não substitui conferência de estoque ou nova cotação. Fontes consultadas em 22–23/10/2026, PTAX [35] de 22/10/2026. Para fornecedores, códigos, URLs e valores, utilizar a planilha original e sua data de consulta.
