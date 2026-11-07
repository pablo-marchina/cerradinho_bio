"""Verifica a estrutura da RTM e suas âncoras explícitas, sem executar ensaios."""
from collections import Counter
from pathlib import Path
import re
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[1]
ANCHOR = re.compile(r'<a\s+(?:name|id)="([^"]+)"\s*>')
LINK = re.compile(r'\]\(([^\s)]+)\)')


def check_links(path):
    text = path.read_text(encoding='utf-8')
    errors = []
    counts = Counter(ANCHOR.findall(text))
    errors.extend(f'{path.name}: âncora duplicada {a}' for a, n in counts.items() if n > 1)
    for href in LINK.findall(text):
        url = urlsplit(href)
        if url.scheme or url.netloc:
            continue
        target = path.parent / unquote(url.path) if url.path else path
        if not target.is_file():
            errors.append(f'{path.name}: arquivo ausente {href}')
        elif url.fragment.startswith('rtm-') or url.fragment == 'cenarios-planejados':
            anchors = ANCHOR.findall(target.read_text(encoding='utf-8'))
            if anchors.count(unquote(url.fragment)) != 1:
                errors.append(f'{path.name}: âncora ausente/duplicada {href}')
    return errors


def main():
    doc = ROOT / 'document/documentacao.md'
    annex = ROOT / 'document/outros/3-desenvolvimento-resultados/validacao-e-limites-sprint1.md'
    inventory = ROOT / 'assets/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/ISSUE-74-RTM.md'
    text = doc.read_text(encoding='utf-8')
    rtm = text.split('### 3.3.4.', 1)[1].split('### 3.3.5.', 1)[0]
    requirements = text.split('### 3.3.3.', 1)[1].split('### 3.3.4.', 1)[0]
    errors = [error for path in (doc, annex, inventory) for error in check_links(path)]
    rows = [[c.strip() for c in line.strip('|').split('|')] for line in rtm.splitlines() if line.startswith('|')]
    rf = [r[1] for r in rows if len(r) == 5 and re.fullmatch(r'RF\d{3}', r[1])]
    if Counter(rf) != Counter(f'RF{i:03}' for i in range(1, 36)):
        errors.append('RTM: esperados 35 RFs únicos')
    missing = set(re.findall(r'RNF\d{2}\.\d+', requirements)) - set(re.findall(r'RNF\d{2}\.\d+', rtm))
    if missing:
        errors.append(f'RNFs ausentes: {sorted(missing)}')
    planned = set(re.findall(r'CT-[FU]\d{2}', rtm))
    if planned - set(re.findall(r'CT-[FU]\d{2}', annex.read_text(encoding='utf-8'))):
        errors.append('Cenários planejados ausentes no anexo')
    physical = [r for r in rows if len(r) == 7 and re.fullmatch(r'CT(?:4[6-9]|[56]\d)', r[0])]
    if Counter(r[0] for r in physical) != Counter(f'CT{i}' for i in range(46, 70)):
        errors.append('Esperados 24 CTs físicos únicos')
    for row in physical:
        if row[0] in ('CT67', 'CT68') and 'NÃO EXECUTADO' not in row[4]:
            errors.append(f'{row[0]} deve permanecer planejado')
    for stale in ('#cobertura-do-protótipo-físico', '#cenários-para-cts-futuros-não-executados', '(futuro)', 'Planejado (não simulado)'):
        if stale in text:
            errors.append(f'Vocabulário/âncora antiga: {stale}')
    for error in errors:
        print(f'FAIL: {error}')
    if not errors:
        print(f'PASS: 35 RFs, 24 CTs físicos, {len(planned)} cenários planejados, RNFs, arquivos e âncoras explícitas.')
    return bool(errors)


if __name__ == '__main__':
    raise SystemExit(main())
