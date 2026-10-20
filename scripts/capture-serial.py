"""Captura somente a saida serial do Wokwi local; nao envia comandos ao firmware."""
import argparse
import pathlib
import time
import serial

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--seconds', type=int, default=240)
args = parser.parse_args()
if not 1 <= args.seconds <= 3600:
    parser.error('--seconds deve ficar entre 1 e 3600')
path = pathlib.Path(__file__).resolve().parents[1] / 'build' / 'simulation-serial.log'
path.parent.mkdir(exist_ok=True)
deadline = time.monotonic() + 30
print('Aguardando Wokwi demo em localhost:4000 por ate 30 s...', flush=True)
while True:
    try:
        port = serial.serial_for_url('rfc2217://localhost:4000', baudrate=115200, timeout=1)
        break
    except (serial.SerialException, OSError) as error:
        if time.monotonic() >= deadline:
            raise SystemExit('Inicie Wokwi: Start Simulator com wokwi.toml. ' + str(error))
        time.sleep(0.25)
print(f'Capturando por {args.seconds} s de relogio; log anexado em {path}', flush=True)
with port, path.open('ab') as log:
    deadline = time.monotonic() + args.seconds
    while time.monotonic() < deadline:
        data = port.read(4096)
        if data:
            log.write(data)
            log.flush()
            print(data.decode('utf-8', errors='replace'), end='', flush=True)
print('\nCaptura encerrada. Esta ferramenta nao avalia os resultados automaticamente.')
