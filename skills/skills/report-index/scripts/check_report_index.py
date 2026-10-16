"""Confere os índices de relatórios (document/outros/sprint-N/relatorios/README.md) contra os arquivos em disco.

Uso (da raiz do repositório):
    python skills/skills/report-index/scripts/check_report_index.py                        # todas as sprints
    python skills/skills/report-index/scripts/check_report_index.py document/outros/sprint-1/relatorios

Sai com código 1 se houver algum ERRO. Só lê arquivos.
"""
import re
import sys
from pathlib import Path

ROOT = Path('document/outros')
COLUMNS = 6  # Relatório | Autor | Issue de origem | Status | Alimenta | Implicação
HEADER_FIELDS = ['Pergunta investigada', 'O que foi feito', 'Evidência', 'Em aberto', 'Alimenta', 'Implicação']
LINK = re.compile(r'\[[^\]]*\]\(([^)#\s]+)(?:#[^)]*)?\)')


def cells(line):
    body = line.strip().strip('|')
    return [c.strip() for c in re.split(r'(?<!\\)\|', body)]


def check(folder):
    errors, warns = [], []
    index = folder / 'README.md'
    reports = sorted(p.name for p in folder.glob('*.md') if p.name != 'README.md')
    if not index.exists():
        if reports:
            errors.append(f'{folder}: {len(reports)} relatório(s) e nenhum README.md de índice')
        return errors, warns
    text = index.read_text(encoding='utf-8')
    rows = [l for l in text.splitlines() if l.strip().startswith('|')]
    seen = {}
    for line in rows[2:] if rows else []:
        cs = cells(line)
        if set(''.join(cs)) <= set('-: '):
            continue
        if len(cs) != COLUMNS:
            errors.append(f'{index}: linha com {len(cs)} células (esperado {COLUMNS}); um "|" sem escape? -> {line[:80]}')
        if any(not c for c in cs):
            errors.append(f'{index}: célula vazia (use "Não informado") -> {line[:80]}')
        if '�' in line or re.search(r'\w\?\w', line):
            warns.append(f'{index}: caractere possivelmente danificado -> {line[:80]}')
        m = LINK.search(cs[0]) if cs else None
        if m:
            name = m.group(1)
            seen[name] = seen.get(name, 0) + 1
    for name, n in seen.items():
        if n > 1:
            errors.append(f'{index}: {name} aparece {n} vezes')
    for link in set(LINK.findall(text)):
        if not link.startswith(('http://', 'https://', 'mailto:')) and not (folder / link).exists():
            errors.append(f'{index}: link para arquivo inexistente: {link}')
    for r in reports:
        if r not in seen:
            errors.append(f'{index}: relatório sem linha no índice: {r}')
        head = (folder / r).read_text(encoding='utf-8')[:2000]
        for field in HEADER_FIELDS:
            m = re.search(rf'\*\*{field}:\*\*[ \t]*(.*)', head)
            if not m:
                errors.append(f'{folder / r}: cabeçalho padrão sem "{field}"')
            elif not m.group(1).strip():
                errors.append(f'{folder / r}: campo "{field}" vazio')
    return errors, warns


def main():
    sys.stdout.reconfigure(encoding='utf-8')
    folders = [Path(a) for a in sys.argv[1:]] or sorted(ROOT.glob('sprint-*/relatorios'))
    if not folders:
        print('Nenhuma pasta document/outros/sprint-N/relatorios encontrada. Nada a conferir.')
        return 0
    total = 0
    for f in folders:
        errors, warns = check(f)
        total += len(errors)
        print(f'{f}: {len(errors)} erro(s), {len(warns)} aviso(s)')
        for e in errors:
            print(f'  ERRO   {e}')
        for w in warns:
            print(f'  aviso  {w}')
    return 1 if total else 0


if __name__ == '__main__':
    sys.exit(main())
