#!/usr/bin/env python3
"""Gera build/wokwi-3botoes: pasta para importar no Wokwi online (projeto com sketch).

O Wokwi online nao aceita build_flags: as definicoes do ambiente bancada-3botoes
(platformio.ini) viram #define no topo do sketch.ino. NAO copia secrets.h, entao o
firmware compila com MQTT desligado (sem rede real). NAO publica nem compila.
"""
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / 'build' / 'wokwi-3botoes'
PROTO = ROOT / 'src/firmware/prototipo_3botoes'
FLAGS = {'DEMO_MODE': 1, 'SWITCH_BUTTONS': 1, 'PREVIOUS_BUTTON_PIN': 4,
         'BUZZER_PIN': 13, 'LCD_SCL_PIN': 22}
HEADERS = ('config.h', 'tracking_math.h', 'cycle_rules.h', 'seal_rules.h', 'event_queue.h',
           'battery_rules.h', 'gnss_rules.h', 'sim_clock.h', 'screen_controller.h',
           'seal_debounce.h', 'dashboard_cmd.h')

if TARGET.exists():
    shutil.rmtree(TARGET)
TARGET.mkdir(parents=True)
defs = ''.join(f'#define {k} {v}\n' for k, v in FLAGS.items())
main = (ROOT / 'src/firmware/main.cpp').read_text(encoding='utf-8')
(TARGET / 'sketch.ino').write_text(
    '// Ambiente bancada-3botoes: flags do platformio.ini como #define (Wokwi online).\n'
    + defs + '\n' + main, encoding='utf-8')
for name in HEADERS:
    shutil.copy2(ROOT / 'include' / name, TARGET / name)
shutil.copy2(PROTO / 'diagram.json', TARGET / 'diagram.json')
for name in ('gps.chip.json', 'gps.chip.wasm', 'gps.chip.c', 'nmea.h'):
    src = ROOT / 'chips' / name
    if src.exists():
        shutil.copy2(src, TARGET / name)
(TARGET / 'libraries.txt').write_text('TinyGPSPlus\nLiquidCrystal I2C\nPubSubClient\n', encoding='utf-8')
assert not (TARGET / 'secrets.h').exists()
print(f'Pacote Wokwi online (3 botoes): {TARGET}')
