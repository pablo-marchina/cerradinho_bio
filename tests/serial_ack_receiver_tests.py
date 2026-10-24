"""Testes independentes do receptor local ACK e persistencia SQLite."""
import importlib.util
import json
from pathlib import Path
import tempfile
import zlib

file=Path(__file__).resolve().parents[1]/'scripts/serial-ack-receiver.py'
spec=importlib.util.spec_from_file_location('guara_receiver',file)
mod=importlib.util.module_from_spec(spec)
spec.loader.exec_module(mod)
with tempfile.TemporaryDirectory() as tmp:
    target=Path(tmp)/'events.sqlite3'
    receiver=mod.SerialAckReceiver(target)
    payload=json.dumps({'device_id':'prototipo-001','type':'VIOLACAO_LACRE','critico':True}, separators=(',',':'))
    token=f'{zlib.crc32(payload.encode()):08X}'
    assert receiver.accept(f'[TX_SIM] token={token} tipo=evento (sem confirmacao remota)') is None
    assert receiver.accept(payload)==f'ACK {token}\n'
    assert receiver.saved==1
    assert receiver.accept(f'[TX_SIM] token={token} tipo=evento') is None
    assert receiver.accept(payload)==f'ACK {token}\n'  # reenviado = confirmar de novo sem duplicar banco
    assert receiver.duplicates==1
    assert receiver.db.execute('SELECT COUNT(*) FROM events').fetchone()[0]==1
    assert receiver.accept('[TX_SIM] token=DEADBEEF tipo=evento') is None
    assert receiver.accept(payload) is None
    assert receiver.rejected==1
    receiver.close()
    receiver2=mod.SerialAckReceiver(target)
    assert receiver2.db.execute('SELECT COUNT(*) FROM events').fetchone()[0]==1
    receiver2.close()
print('PASS: receptor ACK independente: SQLite commit, deduplicacao, CRC e reabertura')
