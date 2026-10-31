"""Verifica consistencia entre diagrama e configuracao antes de abrir o Wokwi."""
import json
import configparser
import pathlib
import re

root = pathlib.Path(__file__).resolve().parents[1]
prototype = root / 'src/firmware/prototipo_wokwi'


def read_jsonc(path):
    """VS Code/PlatformIO geram JSON com comentarios e virgulas finais."""
    text = path.read_text(encoding='utf-8-sig')
    string = r'"(?:\\.|[^"\\])*"'
    text = re.sub(string + r'|//[^\r\n]*|/\*[\s\S]*?\*/',
                  lambda match: match[0] if match[0].startswith('"') else ' ', text)
    text = re.sub(string + r'|,\s*(?=[}\]])',
                  lambda match: match[0] if match[0].startswith('"') else '', text)
    return json.loads(text)


diagram = json.loads((prototype / 'diagram.json').read_text(encoding='utf-8'))
chip = json.loads((root / 'chips/gps.chip.json').read_text(encoding='utf-8'))
for path in (root / '.vscode').glob('*.json'):
    read_jsonc(path)
assert {'platformio.platformio-ide', 'wokwi.wokwi-vscode'} <= set(
    value.lower() for value in read_jsonc(root / '.vscode/extensions.json')['recommendations'])
assert diagram['serialMonitor']['display'] == 'always'
assert diagram['serialMonitor']['collapse'] is False
parts = {part['id']: part for part in diagram['parts']}
assert len(parts) == len(diagram['parts']), 'IDs duplicados no circuito'
for name, kind in [('esp', 'wokwi-esp32-devkit-v1'), ('gps', 'chip-gps'),
                   ('seal', 'wokwi-slide-switch'), ('battery', 'wokwi-potentiometer')]:
    assert parts[name]['type'] == kind, f'Componente divergente: {name}'
hardware_ids = {'esp', 'gps', 'seal', 'battery', 'sw_rede', 'sw_energia', 'btn_autorizar',
                'btn_ciclo', 'btn_avancar', 'btn_voltar', 'lcd', 'buzzer', 'led', 'r_led', 'bb1', 'bb2'}
for name, kind in [('sw_rede', 'wokwi-slide-switch'), ('sw_energia', 'wokwi-slide-switch'),
                   ('btn_autorizar', 'wokwi-pushbutton'), ('btn_ciclo', 'wokwi-pushbutton'),
                   ('btn_avancar', 'wokwi-pushbutton'), ('btn_voltar', 'wokwi-pushbutton'),
                   ('lcd', 'wokwi-lcd1602'), ('buzzer', 'wokwi-buzzer'),
                   ('led', 'wokwi-led'), ('r_led', 'wokwi-resistor'), ('bb1', 'wokwi-breadboard'),
                   ('bb2', 'wokwi-breadboard')]:
    assert parts[name]['type'] == kind, f'Componente divergente: {name}'
assert parts['lcd']['attrs'].get('pins') == 'i2c', 'LCD deve usar o modulo I2C'
assert [p['id'] for p in diagram['parts'] if p['type'] == 'wokwi-led'] == ['led'], 'V2 usa um unico LED, o de violacao'
assert parts['led']['attrs'].get('color') == 'red' and parts['r_led']['attrs'].get('value') == '220'
assert all(p['type'] == 'wokwi-text' for key, p in parts.items()
           if key not in hardware_ids), 'Componente fora do escopo'
# Texto por cima de um controle rouba o clique no Wokwi (o voltar ficou inacessivel assim).
# Medidas do Wokwi: texto com ~8,2 unidades por caractere e 24 por linha.
CONTROL_SIZE = {'wokwi-pushbutton': (67.3, 45.4), 'wokwi-slide-switch': (32.1, 34.9),
                'wokwi-potentiometer': (75.6, 75.6), 'wokwi-buzzer': (75, 83.6), 'wokwi-led': (40, 50)}
def box(part):
    if part['type'] == 'wokwi-text':
        lines = part['attrs']['text'].split('\n')
        w, h = 8.2 * max(len(line) for line in lines), 24 * len(lines)
    else:
        w, h = CONTROL_SIZE[part['type']]
        if part.get('rotate', 0) % 180:  # gira em torno do centro: troca largura e altura
            dx = (w - h) / 2
            return part['left'] + dx, part['top'] - dx, part['left'] + dx + h, part['top'] - dx + w
    return part['left'], part['top'], part['left'] + w, part['top'] + h
texts = [p for p in diagram['parts'] if p['type'] == 'wokwi-text']
for control in (p for p in diagram['parts'] if p['type'] in CONTROL_SIZE):
    cx0, cy0, cx1, cy1 = box(control)
    for text in texts:
        tx0, ty0, tx1, ty1 = box(text)
        assert tx1 <= cx0 or tx0 >= cx1 or ty1 <= cy0 or ty0 >= cy1, \
            f"Texto {text['id']} cobre {control['id']} e bloqueia o clique no Wokwi"
connections = {frozenset(connection[:2]) for connection in diagram['connections']}
# Protoboards: cada trilho e um no so, e cada meia-coluna (a-e ou f-j) tambem; GNDs do ESP32 sao um no.
net = {}
def find(pin):
    net.setdefault(pin, pin)
    while net[pin] != pin:
        net[pin] = net[net[pin]]
        pin = net[pin]
    return pin
def join(a, b):
    net[find(a)] = find(b)
for a, b, *_ in diagram['connections']:
    join(a, b)
for pin in list(net):
    hole = re.fullmatch(r'(bb\d):(tp|tn|bp|bn|\d+[tb])\.\w+', pin)
    if hole:
        join(pin, f'{hole[1]}:{hole[2]}')
join('esp:GND.1', 'esp:GND.2')
for key, part in parts.items():  # o botao liga 1.l a 1.r e 2.l a 2.r por dentro
    if part['type'] == 'wokwi-pushbutton':
        join(f'{key}:1.l', f'{key}:1.r')
        join(f'{key}:2.l', f'{key}:2.r')
def linked(a, b):
    return frozenset((a, b)) in connections or find(a) == find(b)
# bb2 cobre o trilho de baixo de bb1: o do meio e bb2:tp/tn. Trilhos: de cima e do meio 3V3/GND; de baixo 5V (VIN)/GND.
assert not any(re.match(r'bb1:b[pn]\.', p) for p in net), 'Trilho de baixo de bb1 fica escondido sob bb2'
assert all(linked(r, 'esp:GND.1') for r in ('bb1:tn', 'bb2:tn', 'bb2:bn')), 'Trilhos de GND desligados'
assert linked('bb1:tp', 'esp:3V3') and linked('bb2:tp', 'esp:3V3'), 'Trilhos de 3V3 desligados'
assert linked('bb2:bp', 'esp:VIN'), 'Trilho de 5V desligado'
assert not linked('esp:3V3', 'esp:VIN') and not linked('esp:3V3', 'esp:GND.1') \
    and not linked('esp:VIN', 'esp:GND.1'), '3V3, 5V e GND em curto'
for source, target in (
    ('esp:TX0', '$serialMonitor:RX'), ('esp:RX0', '$serialMonitor:TX'),
    ('gps:TX', 'esp:RX2'), ('gps:RX', 'esp:TX2'),
    ('seal:2', 'esp:D27'), ('seal:1', 'esp:GND.2'),
    ('battery:SIG', 'esp:D34'), ('gps:VCC', 'esp:3V3'),
    ('gps:GND', 'esp:GND.1'), ('battery:VCC', 'esp:3V3'),
    ('battery:GND', 'esp:GND.2'),
    ('btn_autorizar:1.r', 'esp:D33'), ('btn_autorizar:2.l', 'esp:GND.2'),
    ('btn_ciclo:1.r', 'esp:D14'), ('btn_ciclo:2.l', 'esp:GND.2'),
    ('sw_rede:2', 'esp:D18'), ('sw_rede:1', 'esp:GND.1'),
    ('sw_energia:2', 'esp:D19'), ('sw_energia:1', 'esp:GND.1'),
    ('lcd:SDA', 'esp:D21'), ('lcd:SCL', 'esp:D22'), ('lcd:VCC', 'esp:VIN'), ('lcd:GND', 'esp:GND.1'),
    ('btn_avancar:1.r', 'esp:D25'), ('btn_avancar:2.l', 'esp:GND.2'),
    ('btn_voltar:1.r', 'esp:D26'), ('btn_voltar:2.l', 'esp:GND.2'),
    ('buzzer:2', 'esp:D23'), ('buzzer:1', 'esp:GND.1'),
    ('esp:D13', 'r_led:2'), ('r_led:1', 'led:A'), ('led:C', 'esp:GND.2'),
):
    assert linked(source, target), f'Ligacao ausente: {source} -> {target}'
assert {'TX', 'RX', 'VCC', 'GND'} <= set(chip['pins'])
assert {c['id'] for c in chip['controls']} == set(parts['gps']['attrs'])
# Terminal 3 das chaves fica aberto: no maximo encaixado num furo sem mais nada.
for switch, message in (('seal', 'Lacre violado'), ('sw_rede', 'Rede indisponivel'), ('sw_energia', 'Desligado')):
    terminal = find(f'{switch}:3')
    others = [p for p in list(net) if find(p) == terminal and not re.match(r'bb\d:', p)]
    assert others == [f'{switch}:3'], f'{message} deve ficar aberto'
config = (root / 'include/config.h').read_text(encoding='utf-8')
pins = [('GpsRx', 16), ('GpsTx', 17), ('SealPin', 27), ('BatteryPin', 34),
        ('LcdSdaPin', 21), ('LcdSclPin', 22), ('NextButtonPin', 25), ('PreviousButtonPin', 26),
        ('BuzzerPin', 23), ('ViolationLedPin', 13),
        ('AuthorizeButtonPin', 33), ('CycleButtonPin', 14), ('NetworkSwitchPin', 18),
        ('PowerSwitchPin', 19)]
assert len({pin for _, pin in pins}) == len(pins), 'GPIO compartilhado entre funcoes'
for name, pin in pins:
    assert re.search(rf'\b{name}\s*=\s*{pin}\s*;', config), f'Pino divergente: {name}'
for name, value in [('GpsBaud', 9600), ('ConsoleBaud', 115200), ('HeartbeatMs', 2000),
                    ('LongPressMs', 3000), ('LcdAddress', '0x27')]:
    assert re.search(rf'\b{name}\s*=\s*{value}\s*;', config), f'Configuracao divergente: {name}'
source = (root / 'src/firmware/main.cpp').read_text(encoding='utf-8')
assert 'pinMode(Config::SealPin, INPUT_PULLUP)' in source
assert 'pinMode(Config::ViolationLedPin, OUTPUT)' in source
for name in ('AuthorizeButtonPin', 'CycleButtonPin', 'NetworkSwitchPin', 'PowerSwitchPin',
             'NextButtonPin', 'PreviousButtonPin'):
    assert f'pinMode(Config::{name}, INPUT_PULLUP)' in source
gps_source = (root / 'chips/gps.chip.c').read_text(encoding='utf-8')
assert re.search(r'\.baud_rate\s*=\s*9600\b', gps_source)
pio = configparser.ConfigParser(interpolation=None)
pio.read(root / 'platformio.ini', encoding='utf-8')
assert pio['platformio']['src_dir'] == 'src/firmware'
assert pio['env']['build_src_filter'] == '+<main.cpp>', 'Compilar somente o firmware principal'
assert pio['env']['framework'] == 'arduino' and pio['env']['board'] == 'esp32dev'
assert 'espressif32' in pio['env']['platform'] and 'TinyGPSPlus' in pio['env']['lib_deps']
assert 'LiquidCrystal_I2C' in pio['env']['lib_deps']
assert '-DDEMO_MODE=1' in pio['env:demo']['build_flags']
assert '-DDEMO_MODE=0' in pio['env:weekly']['build_flags']
wasm = root / 'chips/gps.chip.wasm'
assert wasm.read_bytes()[:8] == b'\x00asm\x01\x00\x00\x00', 'GPS nao compilado para WebAssembly'
for environment, filename in [('demo', 'wokwi.toml'), ('weekly', 'wokwi.weekly.toml')]:
    text = (prototype / filename).read_text()
    for field, expected in [('firmware', root / f'.pio/build/{environment}/firmware.bin'),
                            ('elf', root / f'.pio/build/{environment}/firmware.elf'),
                            ('binary', wasm)]:
        match = re.search(rf'^{field}\s*=\s*"([^"]+)"', text, re.MULTILINE)
        assert match and (prototype / match[1]).resolve() == expected.resolve(), f'Caminho invalido: {filename}/{field}'
    firmware = root / f'.pio/build/{environment}/firmware.bin'
    assert firmware.exists(), f'Compile tambem o ambiente {environment}'
    assert firmware.read_bytes()[0] == 0xE9, 'Cabecalho de firmware ESP32 invalido'
    elf = root / f'.pio/build/{environment}/firmware.elf'
    assert elf.read_bytes()[:4] == b'\x7fELF', 'ELF ausente ou invalido'
print('PASS: circuito V2 integrado, LCD I2C, botoes, buzzer, entradas, GPS, UARTs, GPIOs e caminhos dos binarios')
