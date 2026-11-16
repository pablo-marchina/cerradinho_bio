#!/usr/bin/env python3
"""Prepara uma pasta sincronizavel no Wokwi web; NAO publica nem compila."""
from pathlib import Path
import hashlib
import json
import shutil

ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / 'build' / 'wokwi-publicacao'
SOURCE = ROOT / 'src/firmware/prototipo_wokwi'
TARGET.mkdir(parents=True, exist_ok=True)
# Ao copiar o codigo compilavel e todos os cabecalhos, evita manter uma segunda
# implementacao simplificada (sketch de diagnostico) como se fosse integrada.
shutil.copy2(ROOT / 'src/firmware/main.cpp', TARGET / 'sketch.ino')
for source in (ROOT / 'include').glob('*.h'):
    if source.name.endswith('.local.h') or source.name == 'secrets.h':
        continue
    shutil.copy2(source, TARGET / source.name)
for name in ('diagram.json',):
    shutil.copy2(SOURCE / name, TARGET / name)
for name in ('gps.chip.c', 'gps.chip.json', 'nmea.h', 'wokwi-api.h'):
    src_name = 'gps.chip.c' if name == 'gps.chip.c' else name
    if (ROOT / 'chips' / src_name).exists():
        shutil.copy2(ROOT / 'chips' / src_name, TARGET / name)
    elif name == 'wokwi-api.h':
        print('Aviso: chips/wokwi-api.h nao gerado. Execute scripts/setup.ps1 antes de compilar o chip customizado.')
    else:
        raise FileNotFoundError(ROOT / 'chips' / src_name)
(TARGET / 'libraries.txt').write_text('TinyGPSPlus\nLiquidCrystal I2C\n', encoding='utf-8')
manifest = {'source': 'src/firmware/main.cpp', 'note': 'Exportacao, nao publicacao',
            'files': {p.name: hashlib.sha256(p.read_bytes()).hexdigest()
                      for p in sorted(TARGET.iterdir()) if p.is_file() and p.name != 'manifest.json'}}
(TARGET / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
print(f'Pacote de importacao: {TARGET}')
print('Conferir compilacao e comportamento no Wokwi web antes de publicar; chip GPS custom pode requerer adaptacao da plataforma.')
