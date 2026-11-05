"""Gera as configs do Wokwi for VS Code a partir do diagrama de producao.

CT12: um chip no GPIO34 percorre os niveis de bateria no lugar do potenciometro.
CT43: o GPS comeca sem fix (controle fixValid=0); o resto e manual.
CT45: um chip no lugar do GPS faz perda e retorno do fix e comanda a rede no GPIO18.
"""
import json
from pathlib import Path

base = Path(__file__).resolve().parent
root = base.parent.parent
original = json.loads((root / 'src/firmware/prototipo_wokwi/diagram.json').read_text(encoding='utf-8'))
TOML = '''[wokwi]
version = 1
firmware = "../../../.pio/build/demo/firmware.bin"
elf = "../../../.pio/build/demo/firmware.elf"
rfc2217ServerPort = 4000

[[chip]]
name = "gps"
binary = "../../../chips/gps.chip.wasm"
'''


def chip(name):
    return f'\n[[chip]]\nname = "{name}"\nbinary = "../../../build/{name}.chip.wasm"\n'


def write(caso, diagram, extra=''):
    folder = base / caso
    folder.mkdir(exist_ok=True)
    (folder / 'diagram.json').write_text(json.dumps(diagram, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    (folder / 'wokwi.toml').write_text(TOML + extra, encoding='utf-8')


def part(diagram, part_id):
    return next(p for p in diagram['parts'] if p['id'] == part_id)


ct12 = json.loads(json.dumps(original))
bat = part(ct12, 'battery')
ct12['parts'].append({'type': 'chip-bateria-ct12', 'id': 'bat_ct12', 'top': bat['top'] - 90, 'left': bat['left'], 'attrs': {}})
ct12['connections'] = [c for c in ct12['connections'] if 'battery:SIG' not in c[:2]]
ct12['connections'] += [['bat_ct12:OUT', 'esp:D34', 'orange', []], ['bat_ct12:GND', 'esp:GND.1', 'black', []]]
write('CT12', ct12, chip('bateria-ct12'))

ct43 = json.loads(json.dumps(original))
part(ct43, 'gps')['attrs']['fixValid'] = '0'
write('CT43', ct43)

ct45 = json.loads(json.dumps(original))
gps = part(ct45, 'gps')
gps['type'], gps['attrs'] = 'chip-gps-ct45', {}
ct45['connections'] = [c for c in ct45['connections'] if 'sw_rede:2' not in c[:2]]
ct45['connections'].append(['gps:NET', 'esp:D18', 'violet', []])
write('CT45', ct45, chip('gps-ct45'))
