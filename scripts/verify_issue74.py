#!/usr/bin/env python3
"""Audit issue #74: screenshots, Serial logs, hashes and RTM associations.

Run from any directory: python scripts/verify_issue74.py
Uses Python 3 stdlib only. Does not simulate hardware or attest test approval.
"""
from __future__ import annotations

import csv
import hashlib
import re
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
FOLDER = ROOT / 'assets' / 'wokwi'
CSV = FOLDER / 'issue74-mapeamento-rtm.csv'
MD = FOLDER / 'ISSUE-74-RTM.md'
SHA = FOLDER / 'issue74-evidencias.sha256'
DOC = ROOT / 'document' / 'documentacao.md'
EXPECTED = {f'CT{i:02d}' for i in range(1, 24)} | {'CT35'}
FIELDS = [
    'CT', 'Situação', 'RF associados na RTM', 'RN associadas na RTM',
    'Print principal', 'Prints complementares', 'Log(s) serial', 'Ressalva / limite',
]
CT_RE = re.compile(r'\bCT(?:0[1-9]|1\d|2[0-3]|35)\b')
RF_RE = re.compile(r'\bRF\d{3}\b')
RN_RE = re.compile(r'\bRN\d{2}\b')


def fail(errors: list[str], description: str) -> None:
    errors.append(description)


def split(value: str) -> list[str]:
    return [x.strip() for x in value.split(';') if x.strip()]


def validate_png(p: Path) -> None:
    # Check the PNG header and IHDR for a real raster capture.
    with p.open('rb') as f:
        header = f.read(24)
    if len(header) < 24 or header[:8] != b'\x89PNG\r\n\x1a\n' or header[12:16] != b'IHDR':
        raise ValueError(f'{p.name} is not a valid PNG header')
    width, height = struct.unpack('>II', header[16:24])
    if width < 640 or height < 360 or p.stat().st_size < 10_000:
        raise ValueError(f'{p.name} is too small to support the expected screenshot ({width}x{height})')


def rtm_associations(source: str) -> dict[str, dict[str, set[str]]]:
    rtm = source.split('### 3.3.4.', 1)[-1].split('### 3.3.5.', 1)[0]
    mapping = defaultdict(lambda: {'rf': set(), 'rn': set()})
    for line in rtm.splitlines():
        if not line.startswith('|'):
            continue
        cells = [c.strip() for c in line.strip('|').split('|')]
        if len(cells) != 5:
            continue
        for ct in CT_RE.findall(cells[3]):
            mapping[ct]['rf'].update(RF_RE.findall(cells[1]))
            mapping[ct]['rn'].update(RN_RE.findall(cells[2]))
    return mapping


def sha256_for_evidence(data: bytes, filename: str) -> str:
    """Hash images byte-for-byte, and Serial logs using Git's LF text form.

    With ``* text=auto`` Git may check out tracked text files as CRLF on
    Windows, even though their repository content and manifest use LF. Only
    that line-ending conversion is normalized: other content changes still
    invalidate the SHA-256 digest.
    """
    if filename.endswith('.serial.txt'):
        data = data.replace(b'\r\n', b'\n')
    return hashlib.sha256(data).hexdigest()


def main() -> int:
    errors: list[str] = []
    rows: list[dict[str, str]] = []
    if not CSV.exists():
        print('FAIL: issue74-mapeamento-rtm.csv não encontrado')
        return 1
    with CSV.open(encoding='utf-8-sig', newline='') as f:
        reader = csv.DictReader(f)
        if reader.fieldnames != FIELDS:
            fail(errors, 'Cabeçalho CSV inesperado')
        rows = list(reader)
    cases = Counter(row.get('CT', '') for row in rows)
    if set(cases) != EXPECTED or any(n != 1 for n in cases.values()):
        fail(errors, f'IDs incompletos/duplicados: ausentes={sorted(EXPECTED-set(cases))}; extras={sorted(set(cases)-EXPECTED)}; duplicados={[k for k, v in cases.items() if v > 1]}')

    try:
        md = MD.read_text(encoding='utf-8')
        source = DOC.read_text(encoding='utf-8')
        hashes_text = SHA.read_text(encoding='utf-8')
    except OSError as exc:
        print(f'FAIL: {exc}')
        return 1
    rtm = rtm_associations(source)
    hashes: dict[str, str] = {}
    for line in hashes_text.splitlines():
        match = re.fullmatch(r'([a-f0-9]{64})  ([\w.\-]+)', line)
        if not match:
            fail(errors, f'Linha SHA-256 inválida: {line[:90]}')
            continue
        digest, name = match.groups()
        if name in hashes:
            fail(errors, f'Hash duplicado: {name}')
        hashes[name] = digest
    used: set[str] = set()
    print('Auditoria issue #74 (imagens e logs existentes; sem reexecutar Wokwi)')
    for row in rows:
        ct, status = row['CT'], row['Situação']
        main_img = row['Print principal']
        pngs = [main_img] + split(row['Prints complementares'])
        logs = split(row['Log(s) serial'])
        if not status or not row['Ressalva / limite']:
            fail(errors, f'{ct}: falta situação ou ressalva')
        if not pngs or not logs:
            fail(errors, f'{ct}: falta captura ou log')
        if not main_img.startswith(ct) or not main_img.endswith('.png'):
            fail(errors, f'{ct}: print principal não identificado com o caso: {main_img}')
        if (ct in {'CT09','CT18','CT22'}) != ('BLOQUEADO' in status or 'REPROVADO' in status):
            fail(errors, f'{ct}: possível classificação incorreta: {status}')
        if ct in {'CT09'} and 'BLOQUEADO' not in status:
            fail(errors, 'CT09 deve continuar bloqueado sem ACK do servidor')
        if ct in {'CT18','CT22'} and 'REPROVADO' not in status:
            fail(errors, f'{ct} deve continuar reprovado')
        if ct not in md:
            fail(errors, f'{ct}: ausência no inventário Markdown')
        for name in pngs + logs:
            if not re.fullmatch(r'[\w.\-]+', name) or name in {'.', '..'}:
                fail(errors, f'{ct}: nome de arquivo inseguro: {name!r}')
                continue
            if name in pngs and not name.endswith('.png') or name in logs and not name.endswith('.serial.txt'):
                fail(errors, f'{ct}: extensão incorreta em {name}')
            used.add(name)
            path = FOLDER / name
            if not path.is_file():
                fail(errors, f'{ct}: arquivo inexistente: {name}')
                continue
            if f']({name})' not in md:
                fail(errors, f'{ct}: arquivo não vinculado no Markdown: {name}')
            content = path.read_bytes()
            if name in pngs:
                try:
                    validate_png(path)
                except (ValueError, OSError) as exc:
                    fail(errors, f'{ct}: {exc}')
            elif not content.strip():
                fail(errors, f'{ct}: log Serial vazio: {name}')
            actual_hash = sha256_for_evidence(content, name)
            if hashes.get(name) != actual_hash:
                fail(errors, f'{ct}: checksum ausente/incompatível: {name}')
        expected_rf = set(split(row['RF associados na RTM']))
        expected_rn = set(split(row['RN associadas na RTM']))
        if ct in {'CT17','CT35'}:
            if expected_rf != {'Transversal'} or expected_rn != {'Transversal'} or ct in rtm:
                fail(errors, f'{ct}: deveria permanecer transversal na RTM')
        else:
            expected_rn -= {'—'}
            expected_rf -= {'—'}
            if expected_rf != rtm[ct]['rf'] or expected_rn != rtm[ct]['rn']:
                fail(errors, f'{ct}: divergência na RTM: CSV RF={sorted(expected_rf)}, RN={sorted(expected_rn)}; DOC RF={sorted(rtm[ct]["rf"])}, RN={sorted(rtm[ct]["rn"])}')
        if ct not in source:
            fail(errors, f'{ct}: não encontrado na documentação')
    extra_hashes = set(hashes) - used
    if extra_hashes:
        fail(errors, f'Arquivos de hash não utilizados no mapeamento: {sorted(extra_hashes)}')
    print(f'Casos: {len(rows)} | Arquivos vinculados: {len(used)} | Imagens: {sum(f.endswith(".png") for f in used)} | Logs: {sum(f.endswith(".txt") for f in used)}')
    print('CT09: bloqueado | CT18 e CT22: reprovados | CT14/CT16: reteste corrigido preservado')
    if errors:
        print(f'FALHA ({len(errors)} problema(s)):')
        for error in errors: print(f' - {error}')
        return 1
    print('PASS: CTs, caminhos, cabeçalhos PNG, hashes e associações RTM conferidos.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
