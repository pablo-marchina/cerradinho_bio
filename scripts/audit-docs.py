#!/usr/bin/env python3
"""Checagem estática e reproduzível; NÃO certifica compilação nem simulação Wokwi."""
import pathlib
import re
import subprocess
from sprint1_evidence import evaluate_external, load_evidence
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
DOC = ROOT / 'document' / 'documentacao.md'
text = DOC.read_text(encoding='utf-8')
checks = []

def check(ok, label, detail='', external=False):
    checks.append((bool(ok), label, detail, external))

for label in ['1. Introdução', '1.1 Objetivos', '1.2 Proposta', '1.3 Justificativa',
              '3.1.1. Contexto', '3.1.2. Análise SWOT', '3.1.3. Descrição',
              '3.1.4. Value Proposition', '3.1.5. Matriz', '3.1.6. Política',
              '3.1.7. Bill', '3.2.1. Personas', '3.3.1. Requisitos',
              '3.3.2. Regras', '3.3.4. Matriz', '3.4.1. Protótipo']:
    check(label in text, f'Seção presente: {label}')

annexes = [ROOT / 'document/outros/3-desenvolvimento-resultados' / folder / name
           for folder,name in [('3.1.6-politica-privacidade','politica-privacidade-proposta.md'),
                               ('3.1.7-bom','bom-detalhamento-sprint1.md')]]
markdowns = [DOC, ROOT / 'README.md', *annexes]
links=[]
missing=[]
for md in markdowns:
    if not md.exists():
        missing.append(f'arquivo ausente: {md}');continue
    for link in re.findall(r'!?\[[^]]*\]\(([^)]+)\)', md.read_text(encoding='utf-8')):
        links.append(link)
        if link.startswith(('http://','https://','#','data:','mailto:')):continue
        if not (md.parent/link.split('#')[0]).exists():missing.append(f'{md.relative_to(ROOT)}: {link}')
check(not missing, f'Links Markdown locais ({len(links)} verificados)', str(missing[:10]))

ids_rf = set(re.findall(r'^\|\s*(RF\d{3})\s*\|', text, re.M))
ids_rn = set(re.findall(r'^\|\s*(RN\d{2})\s*\|', text, re.M))
start = text.index('### 3.3.4. Matriz')
end = text.index('### 3.3.5.', start)
rtm = text[start:end]
missing_rf = sorted(ids_rf - set(re.findall(r'\bRF\d{3}\b', rtm)))
missing_rn = sorted(ids_rn - set(re.findall(r'\bRN\d{2}\b', rtm)))
check(not missing_rf, f'RF em RTM ({len(ids_rf)} RF)', str(missing_rf))
check(not missing_rn, f'RN em RTM ({len(ids_rn)} RN)', str(missing_rn))

code = (ROOT / 'src/firmware/main.cpp').read_text(encoding='utf-8')
header = (ROOT / 'include/screen_controller.h').read_text(encoding='utf-8')
check('void setup()' in code and 'void loop()' in code, 'Funções setup()/loop()')
check('class ScreenController' in header and 'screens.render(' in code and
      'updateDisplay(' in code, 'Classe própria invocada por setup()/loop()')

# Testes documentados podem incluir falhas: contabilizar é observação, não reprovar automatismo.
ct_ids = set(re.findall(r'\bCT\d{2}\b', text[text.index('### 3.4.1.'):text.index('### 3.4.2.')]))
check(len(ct_ids) >= 16, f'Casos de teste mencionados ({len(ct_ids)})', ', '.join(sorted(ct_ids)))

# Não misturar resultados reprovados com evidências de aprovação.
failed = sorted(set(re.findall(r'CT\d{2}', 'CT18 CT22')) & ct_ids)
check('CT18' in rtm and 'REPROVADO' in rtm, 'CT18 marcado como reprovado')
check('CT22' in rtm and 'REPROVADO' in rtm, 'CT22 marcado como reprovado')

# Homologação institucional e publicação Wokwi são dependências externas,
# jamais inferidas a partir da ausência de expressões como "pendente".
evidence = load_evidence(ROOT / 'document/outros/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/evidencias_externas_sprint1.json')
for kind, label in [('lgpd','LGPD homologada pelo controlador'),
                    ('wokwi','Wokwi público integrado e retestado')]:
    ok, detail = evaluate_external(evidence.get(kind), kind)
    check(ok, label, detail, external=True)

# Contagem conservadora incluindo cabeçalhos e tabelas; links/Markdown podem
# ser contados de outra forma pelo avaliador, logo manter margem de segurança.
for section, following in [('3.1.1.', '3.1.2.'), ('3.1.2.', '3.1.3.'),
                           ('3.1.3.', '3.1.4.'), ('3.1.4.', '3.1.5.'),
                           ('3.1.5.', '3.1.6.'), ('3.1.6.', '3.1.7.'),
                           ('3.1.7.', '## 3.2.')]:
    a = text.index('### ' + section)
    b = text.index('## 3.2.', a) if following == '## 3.2.' else text.index('### ' + following, a)
    # Conta só o texto visível: destinos de link e tags HTML não são palavras.
    visible = re.sub(r'<[^>]+>', ' ', re.sub(r'\]\([^)]*\)', ']', text[a:b]))
    count = len(re.findall(r'\b[\wÀ-ÿ]+\b', visible))
    check(count <= 300, f'Limite 300 palavras: {section} ({count})')

for ok, label, detail, external in checks:
    status = 'OK' if ok else 'PENDENTE'
    print(f'[{status}/{"EXTERNO" if external else "LOCAL"}] {label}' +
          (f' :: {detail}' if detail else ''))
local = [c for c in checks if not c[3]]
print(f'LOCAL: {sum(c[0] for c in local)}/{len(local)}; '
      f'EXTERNO: {sum(c[0] for c in checks if c[3])}/{len(checks)-len(local)}')
if '--strict' in sys.argv:
    sys.exit(0 if all(c[0] for c in checks) else 1)
if '--strict-local' in sys.argv:
    sys.exit(0 if all(c[0] for c in local) else 1)
