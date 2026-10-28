#!/usr/bin/env python3
"""Auditoria local da Sprint 1; nunca certifica execução Wokwi ou homologação LGPD.

--strict-local: exige apenas os critérios verificáveis pelo repositório (para CI).
--strict: exige inclusive declarações externas respaldadas por evidência.
"""
import pathlib
import re
import sys

from sprint1_evidence import evaluate_external, load_evidence, section_word_count

ROOT = pathlib.Path(__file__).resolve().parents[1]
DOC = ROOT / 'document' / 'documentacao.md'
content = DOC.read_text(encoding='utf-8')
evidence = load_evidence(ROOT / 'document' / 'outros' / 'evidencias_externas_sprint1.json')
checks = []

def check(ok, label, detail='', external=False):
    checks.append((bool(ok), label, detail, external))

for label in ['1. Introdução', '1.1 Objetivos', '1.2 Proposta', '1.3 Justificativa',
              '3.1.1. Contexto', '3.1.2. Análise SWOT', '3.1.3. Descrição',
              '3.1.4. Value Proposition', '3.1.5. Matriz', '3.1.6. Política',
              '3.1.7. Bill', '3.2.1. Personas', '3.3.1. Requisitos',
              '3.3.2. Regras', '3.3.4. Matriz', '3.4.1. Protótipo']:
    check(label in content, f'Seção presente: {label}')

# Verifica também os anexos novos e a documentação de aceite. Não varrer
# TemplateManual.md: ele contém imagens-modelo não fornecidas pela disciplina.
readme = ROOT / 'README.md'
annexes = [ROOT / 'document' / 'outros' / n for n in
           ('politica-privacidade-proposta.md', 'bom-detalhamento-sprint1.md')]
reports = [ROOT / 'document' / 'outros' / 'reports' / 'Sprint01' / n for n in
           ('FECHAMENTO-S1.md', 'RELATORIO_ENTREGA_SPRINT1.md')]
markdown_files = [DOC, readme, *annexes, *reports]
missing = []
count_links = 0
for markdown_file in markdown_files:
    source = markdown_file.read_text(encoding='utf-8')
    links = re.findall(r'!?\[[^]]*\]\(([^)]+)\)', source)
    count_links += len(links)
    for dest in links:
        if dest.startswith(('http://', 'https://', '#', 'data:', 'mailto:')):
            continue
        if not (markdown_file.parent / dest.split('#')[0]).exists():
            missing.append(f'{markdown_file.relative_to(ROOT)}: {dest}')
check(not missing, f'Links Markdown locais ({count_links} verificados)', str(missing[:10]))
ids_rf = set(re.findall(r'^\|\s*(RF\d{3})\s*\|', content, re.M))
ids_rn = set(re.findall(r'^\|\s*(RN\d{2})\s*\|', content, re.M))
rtm = content[content.index('### 3.3.4. Matriz'):content.index('### 3.3.5.')]
missing_rf = sorted(ids_rf - set(re.findall(r'\bRF\d{3}\b', rtm)))
missing_rn = sorted(ids_rn - set(re.findall(r'\bRN\d{2}\b', rtm)))
check(not missing_rf, f'RF na RTM ({len(ids_rf)} RF)', str(missing_rf))
check(not missing_rn, f'RN na RTM ({len(ids_rn)} RN)', str(missing_rn))

code = (ROOT / 'src/firmware/main.cpp').read_text(encoding='utf-8')
header = (ROOT / 'include/indicator_controller.h').read_text(encoding='utf-8')
check('void setup()' in code and 'void loop()' in code, 'Funções setup()/loop()')
check('class IndicatorController' in header and 'indicators.begin();' in code and
      'indicators.display(' in code, 'Classe própria usada em setup()/loop()')
ct = set(re.findall(r'\bCT\d{2}\b', content[content.index('### 3.4.1.'):content.index('### 3.4.2.')]))
check(len(ct) >= 16, f'Casos de teste documentados ({len(ct)})')
check('CT18' in rtm and 'REPROVADO' in rtm, 'CT18 mantém reprovação histórica')
check('CT22' in rtm and 'REPROVADO' in rtm, 'CT22 mantém reprovação histórica')

for start, end in [('3.1.1.', '3.1.2.'), ('3.1.2.', '3.1.3.'),
                   ('3.1.3.', '3.1.4.'), ('3.1.4.', '3.1.5.'),
                   ('3.1.5.', '3.1.6.'), ('3.1.6.', '3.1.7.'),
                   ('3.1.7.', '## 3.2.')]:
    tail = '## 3.2.' if end == '## 3.2.' else '### ' + end
    count = section_word_count(content, '### ' + start, tail)
    check(count <= 300, f'Limite 300 palavras §{start}: {count}',
          'acima do teto estabelecido' if count > 300 else '')

for kind, name in [('lgpd','LGPD: homologação institucional'),
                   ('wokwi','Wokwi: publicação e regressão funcional no simulador')]:
    ok, detail = evaluate_external(evidence.get(kind), kind)
    check(ok, name, detail, external=True)

for ok, label, detail, external in checks:
    category = 'EXTERNO' if external else 'LOCAL'
    state = 'OK' if ok else 'PENDENTE'
    print(f'[{state}/{category}] {label}' + (f' :: {detail}' if detail else ''))
local = [c for c in checks if not c[3]]
print(f'LOCAL: {sum(x[0] for x in local)}/{len(local)}; '
      f'EXTERNO: {sum(x[0] for x in checks if x[3])}/{len(checks)-len(local)}. '
      'Nenhum resultado de host substitui execução do simulador ou homologação.')
if '--strict' in sys.argv:
    sys.exit(1 if not all(c[0] for c in checks) else 0)
if '--strict-local' in sys.argv:
    sys.exit(1 if not all(c[0] for c in local) else 0)
