"""Gera circuitos isolados de ensaio; preserva o diagrama de produção."""
import json
from pathlib import Path
import shutil

base = Path(__file__).resolve().parent
root = base.parent.parent
shutil.copyfile(root / 'chips/wokwi-api.h', base / 'wokwi-api.h')
shutil.copyfile(base / 'estimulos.chip.json', root / 'build/estimulos.chip.json')
original = json.loads((root / 'src/firmware/prototipo_wokwi/diagram.json').read_text(encoding='utf-8'))
for caso in range(18, 23):
    folder = base / f'CT{caso}'
    folder.mkdir(exist_ok=True)
    diagram = json.loads(json.dumps(original))
    for part in diagram['parts']:
        if part['id'] == 'gps':
            part.update(type='chip-estimulos', attrs={'caso': str(caso)})
        if part['id'] == 'title':
            part['attrs']['text'] = f'CT{caso} | Ensaio temporizado da Sprint 1 | instrumentacao externa'
        if part['id'] == 'sealLabel':
            part['attrs']['text'] = 'LACRE | GPIO27 controlado pelo gerador de estimulos'
        if part['id'] == 'networkLabel':
            part['attrs']['text'] = 'REDE | GPIO18 controlado pelo gerador de estimulos'
    removed = {'gps:VCC', 'gps:GND', 'seal:2', 'sw_rede:2', 'btn_autorizar:1.l'}
    diagram['connections'] = [c for c in diagram['connections'] if not removed.intersection(c[:2])]
    diagram['connections'] += [[a, b, color, []] for a, b, color in (
        ('gps:SEAL', 'esp:D27', 'orange'), ('gps:NET', 'esp:D18', 'violet'),
        ('gps:AUTH', 'esp:D33', 'green'), ('gps:TRACE', 'esp:D32', 'cyan'),
        ('esp:TX0', 'gps:MONITOR', 'gray'))]
    (folder / 'diagram.json').write_text(json.dumps(diagram, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    (folder / 'wokwi.toml').write_text('''[wokwi]
version = 1
firmware = "../.pio/build/demo/firmware.bin"
elf = "../.pio/build/demo/firmware.elf"
rfc2217ServerPort = 4000

[[chip]]
name = "estimulos"
binary = "../../../build/estimulos.chip.wasm"
''', encoding='utf-8')

# Contraprova do CT18 com o binário original, sem a sonda no loop do ESP32.
baseline = base / 'CT18-original'
baseline.mkdir(exist_ok=True)
shutil.copyfile(base / 'CT18/diagram.json', baseline / 'diagram.json')
(baseline / 'wokwi.toml').write_text('''[wokwi]
version = 1
firmware = "../../../.pio/build/demo/firmware.bin"
elf = "../../../.pio/build/demo/firmware.elf"
rfc2217ServerPort = 4000

[[chip]]
name = "estimulos"
binary = "../../../build/estimulos.chip.wasm"
''', encoding='utf-8')
