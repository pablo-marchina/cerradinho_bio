#!/usr/bin/env python3
"""Receptor de bancada independente via Serial/RFC2217.

Grava cada JSON em SQLite ANTES de responder ACK <CRC32>. Nao usa MQTT,
TLS, identidade criptografica ou infraestrutura de producao. Destinado a
validar o fluxo de confirmacao e reenfileiramento no Wokwi/ESP32 local.
"""
import argparse
import hashlib
import json
import pathlib
import re
import sqlite3
import sys
import time
import zlib

HEADER = re.compile(r"^\[TX_SIM\] token=([0-9A-Fa-f]{8}) tipo=(evento|resumo)\b")

class SerialAckReceiver:
    def __init__(self, database):
        self.db = sqlite3.connect(str(database))
        self.db.execute("PRAGMA synchronous=FULL")
        self.db.execute("""CREATE TABLE IF NOT EXISTS events (
            digest TEXT PRIMARY KEY, token TEXT NOT NULL, payload TEXT NOT NULL,
            device_id TEXT, event_type TEXT, stored_at TEXT DEFAULT CURRENT_TIMESTAMP
        )""")
        self.db.commit()
        self.expected = None
        self.saved = 0
        self.duplicates = 0
        self.rejected = 0

    def accept(self, line):
        """Aceita uma linha; devolve ACK se JSON, token e commit forem validos."""
        line = line.strip()
        match = HEADER.match(line)
        if match:
            self.expected = match.group(1).upper()
            return None
        if self.expected is None or not line.startswith('{'):
            return None
        token = self.expected
        self.expected = None
        try:
            parsed = json.loads(line)
            if not isinstance(parsed, dict) or not parsed.get('device_id') or not parsed.get('type'):
                raise ValueError('dispositivo/tipo ausentes')
            if f'{zlib.crc32(line.encode("utf-8")) & 0xFFFFFFFF:08X}' != token:
                raise ValueError('CRC inconsistente')
        except (json.JSONDecodeError, ValueError):
            self.rejected += 1
            return None
        digest = hashlib.sha256(line.encode('utf-8')).hexdigest()
        try:
            before = self.db.total_changes
            self.db.execute('INSERT OR IGNORE INTO events(digest,token,payload,device_id,event_type) VALUES (?,?,?,?,?)',
                            (digest, token, line, parsed['device_id'], parsed['type']))
            self.db.commit()  # NUNCA responder antes de confirmacao do banco
            if self.db.total_changes == before:
                self.duplicates += 1
            else:
                self.saved += 1
        except sqlite3.Error:
            self.db.rollback()
            self.rejected += 1
            return None
        return f'ACK {token}\n'

    def close(self):
        self.db.close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--db',default='build/wokwi-ci/receiver.sqlite3')
    parser.add_argument('--port',default='rfc2217://localhost:4000')
    parser.add_argument('--seconds',type=int,default=180)
    parser.add_argument('--stdin',action='store_true',help='teste local sem pyserial: ler linhas de stdin')
    args = parser.parse_args()
    if not 1 <= args.seconds <= 3600:
        parser.error('--seconds deve ser entre 1 e 3600')
    path=pathlib.Path(args.db)
    path.parent.mkdir(parents=True, exist_ok=True)
    receiver=SerialAckReceiver(path)
    print('[RECEIVER] bancada: SQLite antes do ACK; nao e servidor MQTT',flush=True)
    try:
        if args.stdin:
            for line in sys.stdin:
                ack=receiver.accept(line)
                if ack:
                    sys.stdout.write(ack)
                    sys.stdout.flush()
        else:
            try:
                import serial
            except ImportError:
                raise SystemExit('Instale pyserial no ambiente do teste: python -m pip install pyserial')
            deadline=time.monotonic()+args.seconds
            with serial.serial_for_url(args.port,baudrate=115200,timeout=1) as port:
                buffer=''
                while time.monotonic()<deadline:
                    block=port.read(2048)
                    if not block: continue
                    buffer+=block.decode('utf-8',errors='replace')
                    while '\n' in buffer:
                        line,buffer=buffer.split('\n',1)
                        ack=receiver.accept(line)
                        if ack:
                            port.write(ack.encode('ascii'))
                            print(f'[RECEIVER] persisted and ACKed: {ack.strip()}',flush=True)
    finally:
        print(f'[RECEIVER] novos={receiver.saved} duplicados={receiver.duplicates} rejeitados={receiver.rejected}',flush=True)
        receiver.close()

if __name__ == '__main__':
    main()
