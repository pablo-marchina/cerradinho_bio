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
                'btn_ciclo', 'btn_avancar', 'btn_voltar', 'lcd', 'buzzer'}
for name, kind in [('sw_rede', 'wokwi-slide-switch'), ('sw_energia', 'wokwi-slide-switch'),
                   ('btn_autorizar', 'wokwi-pushbutton'), ('btn_ciclo', 'wokwi-pushbutton'),
                   ('btn_avancar', 'wokwi-pushbutton'), ('btn_voltar', 'wokwi-pushbutton'),
                   ('lcd', 'wokwi-lcd1602'), ('buzzer', 'wokwi-buzzer')]:
    assert parts[name]['type'] == kind, f'Componente divergente: {name}'
assert parts['lcd']['attrs'].get('pins') == 'i2c', 'LCD deve usar o modulo I2C'
assert not any(p['type'] == 'wokwi-led' for p in diagram['parts']), 'V2 nao usa LEDs de status'
assert all(p['type'] == 'wokwi-text' for key, p in parts.items()
           if key not in hardware_ids), 'Componente fora do escopo'
connections = {frozenset(connection[:2]) for connection in diagram['connections']}
for source, target in (
    ('esp:TX0', '$serialMonitor:RX'), ('esp:RX0', '$serialMonitor:TX'),
    ('gps:TX', 'esp:RX2'), ('gps:RX', 'esp:TX2'),
    ('seal:2', 'esp:D27'), ('seal:1', 'esp:GND.1'),
    ('battery:SIG', 'esp:D34'), ('gps:VCC', 'esp:3V3'),
    ('gps:GND', 'esp:GND.1'), ('battery:VCC', 'esp:3V3'),
    ('battery:GND', 'esp:GND.1'),
    ('btn_autorizar:1.l', 'esp:D33'), ('btn_autorizar:2.r', 'esp:GND.1'),
    ('btn_ciclo:1.l', 'esp:D13'), ('btn_ciclo:2.r', 'esp:GND.1'),
    ('sw_rede:2', 'esp:D18'), ('sw_rede:1', 'esp:GND.1'),
    ('sw_energia:2', 'esp:D19'), ('sw_energia:1', 'esp:GND.1'),
    ('lcd:SDA', 'esp:D21'), ('lcd:SCL', 'esp:D22'), ('lcd:VCC', 'esp:VIN'), ('lcd:GND', 'esp:GND.2'),
    ('btn_avancar:1.l', 'esp:D25'), ('btn_avancar:2.r', 'esp:GND.1'),
    ('btn_voltar:1.l', 'esp:D26'), ('btn_voltar:2.r', 'esp:GND.1'),
    ('buzzer:2', 'esp:D23'), ('buzzer:1', 'esp:GND.1'),
):
    assert frozenset((source, target)) in connections, f'Ligacao ausente: {source} -> {target}'
assert {'TX', 'RX', 'VCC', 'GND'} <= set(chip['pins'])
assert {c['id'] for c in chip['controls']} == set(parts['gps']['attrs'])
assert not any('seal:3' in c for c in connections), 'Lacre violado deve ficar aberto'
assert not any('sw_rede:3' in c for c in connections), 'Rede indisponivel deve ficar aberta'
assert not any('sw_energia:3' in c for c in connections), 'Desligado deve ficar aberto'
config = (root / 'include/config.h').read_text(encoding='utf-8')
pins = [('GpsRx', 16), ('GpsTx', 17), ('SealPin', 27), ('BatteryPin', 34),
        ('LcdSdaPin', 21), ('LcdSclPin', 22), ('NextButtonPin', 25), ('PreviousButtonPin', 26),
        ('BuzzerPin', 23),
        ('AuthorizeButtonPin', 33), ('CycleButtonPin', 13), ('NetworkSwitchPin', 18),
        ('PowerSwitchPin', 19)]
assert len({pin for _, pin in pins}) == len(pins), 'GPIO compartilhado entre funcoes'
for name, pin in pins:
    assert re.search(rf'\b{name}\s*=\s*{pin}\s*;', config), f'Pino divergente: {name}'
for name, value in [('GpsBaud', 9600), ('ConsoleBaud', 115200), ('HeartbeatMs', 2000),
                    ('LongPressMs', 3000), ('LcdAddress', '0x27')]:
    assert re.search(rf'\b{name}\s*=\s*{value}\s*;', config), f'Configuracao divergente: {name}'
source = (root / 'src/firmware/main.cpp').read_text(encoding='utf-8')
assert 'pinMode(Config::SealPin, INPUT_PULLUP)' in source
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
