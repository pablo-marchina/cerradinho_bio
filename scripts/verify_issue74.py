#!/usr/bin/env python3
"""Audita o inventário CT01–CT23/CT35 (V1 e V2) da issue #74.

Execute da raiz ou de qualquer pasta: python scripts/verify_issue74.py
Somente stdlib; não executa Wokwi nem declara atendimento integral a RF/RN.
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
FOLDER = ROOT / 'assets' / '3-desenvolvimento-resultados' / '3.4.1-prototipo-wokwi'
CSV = FOLDER / 'issue74-mapeamento-rtm.csv'
MD = FOLDER / 'ISSUE-74-RTM.md'
SHA = FOLDER / 'issue74-evidencias.sha256'
DOC = ROOT / 'document' / 'documentacao.md'
EXPECTED = {f'CT{i:02d}' for i in range(1, 24)} | {'CT35'}
NOT_REPEATED = {'CT19', 'CT20', 'CT21', 'CT22'}
FIELDS = [
    'CT', 'Situação', 'RF associados na RTM', 'RN associadas na RTM',
    'Print principal', 'Prints complementares', 'Log(s) serial', 'Ressalva / limite',
    'Situação V2 (02–03/11)', 'Prints V2', 'Logs V2', 'Ressalva V2',
]
CT_RE = re.compile(r'\bCT(?:0[1-9]|1\d|2[0-3]|35)\b')
RF_RE = re.compile(r'\bRF\d{3}\b')
RN_RE = re.compile(r'\bRN\d{2}\b')
SAFE_NAME_RE = re.compile(r'[\w.\-]+', re.ASCII)
HASH_RE = re.compile(r'([a-f0-9]{64})  ([\w.\-]+)', re.ASCII)


def split(value: str) -> list[str]:
    return [v.strip() for v in value.split(';') if v.strip()]


def rtm_associations(text: str) -> dict[str, dict[str, set[str]]]:
    """Usa exclusivamente as células RF, RN, CT da tabela 3.3.4."""
    section = text.split('### 3.3.4.', 1)
    if len(section) != 2 or '### 3.3.5.' not in section[1]:
        raise ValueError('Seção 3.3.4/3.3.5 ausente ou alterada')
    section = section[1].split('### 3.3.5.', 1)[0]
    result = defaultdict(lambda: {'rf': set(), 'rn': set()})
    for line in section.splitlines():
        if not line.startswith('|'):
            continue
        cells = [c.strip() for c in line.strip('|').split('|')]
        if len(cells) != 5:
            continue
        for ct in CT_RE.findall(cells[3]):
            result[ct]['rf'].update(RF_RE.findall(cells[1]))
            result[ct]['rn'].update(RN_RE.findall(cells[2]))
    return result


def validate_png(path: Path, *, v2: bool) -> None:
    """Aceita PNGs V1 de tela inteira e recortes pequenos do LCD V2."""
    with path.open('rb') as f:
        header = f.read(24)
    if len(header) < 24 or header[:8] != b'\x89PNG\r\n\x1a\n' or header[12:16] != b'IHDR':
        raise ValueError(f'{path.name}: cabeçalho PNG inválido')
    width, height = struct.unpack('>II', header[16:24])
    if v2:
        # Wokwi V2 recorta LCD 16x2 (~213x44), sem reduzir a evidência a 0 bytes.
        minimum = (100, 40, 1_000)
    else:
        minimum = (640, 360, 10_000)
    if width < minimum[0] or height < minimum[1] or path.stat().st_size < minimum[2]:
        raise ValueError(f'{path.name}: dimensões/tamanho insuficientes ({width}x{height})')


def sha256_evidence(raw: bytes, filename: str) -> str:
    # Checkout Windows pode converter LF em CRLF em logs de texto versionados.
    if filename.endswith('.serial.txt'):
        raw = raw.replace(b'\r\n', b'\n')
    return hashlib.sha256(raw).hexdigest()


def check_status(ct: str, row: dict[str, str], errors: list[str]) -> None:
    v1 = row['Situação']
    v2 = row['Situação V2 (02–03/11)']
    if not v1 or not v2 or not row['Ressalva / limite'] or not row['Ressalva V2']:
        errors.append(f'{ct}: situação/ressalva V1 ou V2 ausente')
    if ct == 'CT09' and ('BLOQUEADO' not in v1 or 'BLOQUEADO' not in v2):
        errors.append('CT09 deve continuar BLOQUEADO nas duas rodadas (ACK não comprovado)')
    if ct == 'CT18' and ('REPROVADO' not in v1 or 'REPROVADO' not in v2):
        errors.append('CT18 deve continuar REPROVADO nas duas rodadas')
    if ct == 'CT22' and 'REPROVADO' not in v1:
        errors.append('CT22 deve continuar REPROVADO na rodada V1')
    if ct == 'CT04' and ('APROVADO' not in v2 or '10' not in v2 or '150' not in v2):
        errors.append('CT04 V2 deve registrar aprovação nos dois limiares (10 e 150)')
    if ct == 'CT08' and 'PARCIAL' not in v2:
        errors.append('CT08 V2 deve explicitar tela Sem sinal não executada')
    if ct == 'CT15' and 'parcial' not in v2.lower():
        errors.append('CT15 V2 deve deixar a recusa RN01 pendente')
    if ct == 'CT35' and 'PARCIAL' not in v2:
        errors.append('CT35 V2 deve explicitar ausência de captura do silêncio')
    if ct in NOT_REPEATED:
        if v2 != 'NÃO REEXECUTADO NO V2' or row['Prints V2'] or row['Logs V2']:
            errors.append(f'{ct}: sem reteste V2; não atribuir novas evidências')
    elif not row['Logs V2'] or 'NÃO REEXECUTADO' in v2:
        errors.append(f'{ct}: falta log da rodada V2 executada')


def main() -> int:
    errors: list[str] = []
    if not CSV.is_file():
        print(f'FAIL: CSV não encontrado: {CSV}')
        return 1
    with CSV.open(encoding='utf-8-sig', newline='') as stream:
        reader = csv.DictReader(stream)
        if reader.fieldnames != FIELDS:
            errors.append('Cabeçalho do CSV diferente do esquema V1/V2 esperado')
        rows = list(reader)
    cases = Counter(row.get('CT', '') for row in rows)
    if set(cases) != EXPECTED or any(count != 1 for count in cases.values()):
        errors.append(f'CTs ausentes/duplicados/extras: {dict(cases)}')
    try:
        md = MD.read_text(encoding='utf-8')
        doc = DOC.read_text(encoding='utf-8')
        manifest = SHA.read_text(encoding='utf-8')
        rtm = rtm_associations(doc)
    except (OSError, ValueError) as exc:
        print(f'FAIL: {exc}')
        return 1
    if f'../assets/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/{MD.name}' not in doc:
        errors.append('Documento principal não referencia novo caminho do inventário')
    hashes: dict[str, str] = {}
    for line in manifest.splitlines():
        m = HASH_RE.fullmatch(line)
        if not m:
            errors.append(f'Linha SHA-256 inválida: {line[:90]}')
            continue
        digest, name = m.groups()
        if name in hashes:
            errors.append(f'Hash duplicado: {name}')
        hashes[name] = digest
    used: set[str] = set()
    print('Auditoria issue #74: 24 CTs (V1/V2), caminhos, PNG, hashes e RTM')
    for row in rows:
        ct = row['CT']
        if ct not in EXPECTED:
            continue
        check_status(ct, row, errors)
        if ct not in md or row['Situação'] not in md or row['Situação V2 (02–03/11)'] not in md:
            errors.append(f'{ct}: situação V1/V2 não encontrada no inventário Markdown')
        main = row['Print principal']
        if not main.lower().startswith(ct.lower()) or not main.endswith('.png'):
            errors.append(f'{ct}: print principal inválido: {main!r}')
        v1_pngs = [main] + split(row['Prints complementares'])
        v1_logs = split(row['Log(s) serial'])
        v2_pngs = split(row['Prints V2'])
        v2_logs = split(row['Logs V2'])
        if not v1_logs:
            errors.append(f'{ct}: falta log da V1')
        for name, is_png, is_v2 in (
            [(f, True, False) for f in v1_pngs] +
            [(f, False, False) for f in v1_logs] +
            [(f, True, True) for f in v2_pngs] +
            [(f, False, True) for f in v2_logs]
        ):
            if not SAFE_NAME_RE.fullmatch(name) or name in ('.', '..'):
                errors.append(f'{ct}: nome inseguro de evidência: {name!r}')
                continue
            if not name.endswith('.png' if is_png else '.serial.txt'):
                errors.append(f'{ct}: extensão inesperada: {name}')
                continue
            used.add(name)
            path = FOLDER / name
            if not path.is_file():
                errors.append(f'{ct}: evidência inexistente: {name}')
                continue
            if f']({name})' not in md:
                errors.append(f'{ct}: evidência não vinculada no inventário: {name}')
            raw = path.read_bytes()
            if is_png:
                try:
                    validate_png(path, v2=is_v2)
                except (ValueError, OSError) as exc:
                    errors.append(f'{ct}: {exc}')
            elif not raw.strip():
                errors.append(f'{ct}: log vazio: {name}')
            actual = sha256_evidence(raw, name)
            if hashes.get(name) != actual:
                errors.append(f'{ct}: SHA-256 incompatível/ausente: {name}')
        rf = set(split(row['RF associados na RTM'])) - {'—'}
        rn = set(split(row['RN associadas na RTM'])) - {'—'}
        if ct in {'CT17', 'CT35'}:
            if rf != {'Transversal'} or rn != {'Transversal'} or ct in rtm:
                errors.append(f'{ct}: precisa permanecer transversal (sem RF/RN na RTM)')
        elif rf != rtm[ct]['rf'] or rn != rtm[ct]['rn']:
            errors.append(f'{ct}: RF/RN divergentes da RTM: CSV={sorted(rf)}/{sorted(rn)}, DOC={sorted(rtm[ct]["rf"])}/{sorted(rtm[ct]["rn"])}')
    if set(hashes) != used:
        errors.append(f'Manifesto diferente do inventário: faltam={sorted(used-set(hashes))}, sobram={sorted(set(hashes)-used)}')
    print(f'CTs: {len(rows)} | Evidências únicas: {len(used)} | PNG: {sum(x.endswith(".png") for x in used)} | Logs: {sum(x.endswith(".serial.txt") for x in used)}')
    print('CT04 V2: 10/150 aprovados; CT09 bloqueado; CT18 reprovado V1/V2; CT22 reprovado V1')
    if errors:
        print(f'FAIL ({len(errors)} problema(s)):')
        for e in errors:
            print(f' - {e}')
        return 1
    print('PASS: inventário V1/V2 e associação RTM consistentes.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
