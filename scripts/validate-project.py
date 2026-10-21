"""Verifica consistencia entre diagrama e configuracao antes de abrir o Wokwi."""
import json
import configparser
import pathlib
import re

root = pathlib.Path(__file__).resolve().parents[1]


def read_jsonc(path):
    """VS Code/PlatformIO geram JSON com comentarios e virgulas finais."""
    text = path.read_text(encoding='utf-8-sig')
    string = r'"(?:\\.|[^"\\])*"'
    text = re.sub(string + r'|//[^\r\n]*|/\*[\s\S]*?\*/',
                  lambda match: match[0] if match[0].startswith('"') else ' ', text)
    text = re.sub(string + r'|,\s*(?=[}\]])',
                  lambda match: match[0] if match[0].startswith('"') else '', text)
    return json.loads(text)


diagram = json.loads((root / 'diagram.json').read_text(encoding='utf-8'))
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
assert all(p['type'] == 'wokwi-text' for key, p in parts.items()
           if key not in {'esp', 'gps', 'seal', 'battery'}), 'Componente fora do escopo'
connections = {frozenset(connection[:2]) for connection in diagram['connections']}
for source, target in (
    ('esp:TX0', '$serialMonitor:RX'), ('esp:RX0', '$serialMonitor:TX'),
    ('gps:TX', 'esp:RX2'), ('gps:RX', 'esp:TX2'),
    ('seal:2', 'esp:D27'), ('seal:1', 'esp:GND.1'),
    ('battery:SIG', 'esp:D34'), ('gps:VCC', 'esp:3V3'),
    ('gps:GND', 'esp:GND.1'), ('battery:VCC', 'esp:3V3'),
    ('battery:GND', 'esp:GND.1'),
):
    assert frozenset((source, target)) in connections, f'Ligacao ausente: {source} -> {target}'
assert {'TX', 'RX', 'VCC', 'GND'} <= set(chip['pins'])
assert {c['id'] for c in chip['controls']} == set(parts['gps']['attrs'])
assert not any('seal:3' in c for c in connections), 'Lacre violado deve ficar aberto'
config = (root / 'include/config.h').read_text(encoding='utf-8')
for name, pin in [('GpsRx', 16), ('GpsTx', 17), ('SealPin', 27), ('BatteryPin', 34)]:
    assert re.search(rf'\b{name}\s*=\s*{pin}\s*;', config), f'Pino divergente: {name}'
for name, value in [('GpsBaud', 9600), ('ConsoleBaud', 115200), ('HeartbeatMs', 2000)]:
    assert re.search(rf'\b{name}\s*=\s*{value}\s*;', config), f'Configuracao divergente: {name}'
source = (root / 'src/firmware/main.cpp').read_text(encoding='utf-8')
assert 'pinMode(Config::SealPin, INPUT_PULLUP)' in source
gps_source = (root / 'chips/gps.chip.c').read_text(encoding='utf-8')
assert re.search(r'\.baud_rate\s*=\s*9600\b', gps_source)
pio = configparser.ConfigParser(interpolation=None)
pio.read(root / 'platformio.ini', encoding='utf-8')
assert pio['platformio']['src_dir'] == 'src/firmware'
assert pio['env']['framework'] == 'arduino' and pio['env']['board'] == 'esp32dev'
assert 'espressif32' in pio['env']['platform'] and 'TinyGPSPlus' in pio['env']['lib_deps']
assert '-DDEMO_MODE=1' in pio['env:demo']['build_flags']
assert '-DDEMO_MODE=0' in pio['env:weekly']['build_flags']
wasm = root / 'chips/gps.chip.wasm'
assert wasm.read_bytes()[:8] == b'\x00asm\x01\x00\x00\x00', 'GPS nao compilado para WebAssembly'
for environment, filename in [('demo', 'wokwi.toml'), ('weekly', 'wokwi.weekly.toml')]:
    text = (root / filename).read_text()
    assert f'.pio/build/{environment}/firmware.bin' in text
    assert f'.pio/build/{environment}/firmware.elf' in text
    assert 'chips/gps.chip.wasm' in text
    firmware = root / f'.pio/build/{environment}/firmware.bin'
    assert firmware.exists(), f'Compile tambem o ambiente {environment}'
    assert firmware.read_bytes()[0] == 0xE9, 'Cabecalho de firmware ESP32 invalido'
    elf = root / f'.pio/build/{environment}/firmware.elf'
    assert elf.read_bytes()[:4] == b'\x7fELF', 'ELF ausente ou invalido'
print('PASS: componentes, alimentacao, UARTs, monitor expandido, pinos/baud, ambientes e binarios')
