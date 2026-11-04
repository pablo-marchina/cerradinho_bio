"""Grava a Serial do Wokwi for VS Code (RFC2217, porta 4000), um log por cenario.

Uso: .tools/venv/Scripts/python.exe tests/wokwi-vscode/capturar.py nome1 nome2 ...
Para cada nome, espera a porta 4000, grava em build/wokwi-ct/<nome>.serial.txt e passa
ao proximo quando a porta fecha ou apos 180 s sem dados. Conexao sem nenhum dado nao
consome o nome. A conexao pode sobreviver ao reinicio da simulacao no VS Code: nesse
caso o log junta as rodadas e vale o trecho a partir do ultimo boot ("ets Jul").
Nos cenarios de COMANDOS, envia sozinho os comandos Serial: (gatilho no log, atraso
em s, comando), uma vez cada.
"""
import pathlib, sys, time
import serial

COMANDOS = {
    # REMESSA antes do vinculo, para registrar a recusa sem ciclo ativo.
    'ct44-vinculo': [(b'"type":"STATUS"', 8, 'REMESSA NF-000123')],
    # NF so depois da primeira recusa por falta de NF (a do ciclo 2 fica sem NF).
    'ct14-16-encerramento': [(b'"motivo":"nf_retorno_ausente"', 1, 'NF')],
}

OUT = pathlib.Path(__file__).resolve().parents[2] / 'build' / 'wokwi-ct'
OUT.mkdir(parents=True, exist_ok=True)
names = list(sys.argv[1:])
while names:
    name = names[0]
    print(f'[{time.strftime("%H:%M:%S")}] aguardando a simulacao de {name}', flush=True)
    deadline = time.monotonic() + 3 * 3600
    while True:
        try:
            port = serial.serial_for_url('rfc2217://localhost:4000', baudrate=115200, timeout=1)
            break
        except (serial.SerialException, OSError):
            if time.monotonic() > deadline:
                sys.exit(f'{name}: a simulacao nao abriu a porta 4000 em 3 h')
            time.sleep(0.05)
    print(f'[{time.strftime("%H:%M:%S")}] {name}: conectado', flush=True)
    pending = list(COMANDOS.get(name, []))
    due = []  # (instante, comando) ja disparados, esperando o atraso
    text, size, last = b'', 0, time.monotonic()
    tmp = OUT / f'{name}.serial.tmp'
    with tmp.open('wb') as log:
        try:
            # O heartbeat sai a cada 2 s; 180 s sem nada (o CT35 fica desligado uns segundos) tambem indica simulacao parada.
            while time.monotonic() - last < 180:
                data = port.read(4096)
                if data:
                    log.write(data)
                    log.flush()
                    size += len(data)
                    text += data
                    last = time.monotonic()
                for item in [p for p in pending if p[0] in text]:
                    pending.remove(item)
                    due.append((time.monotonic() + item[1], item[2]))
                for at, cmd in [d for d in due if d[0] <= time.monotonic()]:
                    due.remove((at, cmd))
                    port.write(cmd.encode() + b'\n')
                    print(f'[{time.strftime("%H:%M:%S")}] {name}: enviado {cmd}', flush=True)
        except (serial.SerialException, OSError, EOFError):
            pass
    port.close()
    if size == 0:
        # Conexao sem dados (simulacao anterior ainda aberta): o cenario continua na fila.
        tmp.unlink()
        print(f'[{time.strftime("%H:%M:%S")}] {name}: nada recebido; pare a simulacao e inicie de novo', flush=True)
        time.sleep(5)
        continue
    tmp.replace(OUT / f'{name}.serial.txt')
    names.pop(0)
    print(f'[{time.strftime("%H:%M:%S")}] {name}: {size} bytes gravados', flush=True)
    time.sleep(2)
