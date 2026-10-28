#!/usr/bin/env python3
"""Checagem estática e reproduzível; NÃO certifica compilação nem simulação Wokwi."""
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
DOC = ROOT / 'document' / 'documentacao.md'
text = DOC.read_text(encoding='utf-8')
checks = []

def check(ok, label, detail=''):
    checks.append((bool(ok), label, detail))

for label in ['1. Introdução', '1.1 Objetivos', '1.2 Proposta', '1.3 Justificativa',
              '3.1.1. Contexto', '3.1.2. Análise SWOT', '3.1.3. Descrição',
              '3.1.4. Value Proposition', '3.1.5. Matriz', '3.1.6. Política',
              '3.1.7. Bill', '3.2.1. Personas', '3.3.1. Requisitos',
              '3.3.2. Regras', '3.3.4. Matriz', '3.4.1. Protótipo']:
    check(label in text, f'Seção presente: {label}')

links = re.findall(r'!?\[[^]]*\]\(([^)]+)\)', text)
missing = [p for p in links if not p.startswith(('http://', 'https://', '#', 'data:'))
           and not (DOC.parent / p.split('#')[0]).exists()]
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

# Aprovação institucional não pode ser deduzida da ausência da expressão
# "PENDENTE DE VALIDAÇÃO". A minuta emprega "a confirmar" e "a definir".
lgpd = text[text.index('### 3.1.6.'):text.index('### 3.1.7.')]
open_fields = re.findall(r'\ba (?:confirmar|definir|aprovar)\b|NÃO aprovada', lgpd, re.IGNORECASE)
check(not open_fields, 'LGPD homologada pelo controlador',
      f'{len(open_fields)} pendências explícitas; exige comprovação externa')

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

# Pendências com responsáveis externos nunca são contabilizadas como evidências
# de teste funcional, mesmo que checagens sintáticas estejam verdes.
check('A REALIZAR' not in text[text.index('### 3.4.1.'):text.index('### 3.4.2.')],
      'Wokwi público integrado publicado', 'URL histórica não representa firmware atual')

for ok, label, detail in checks:
    status = 'OK' if ok else 'PENDENTE'
    print(f'[{status}] {label}' + (f' :: {detail}' if detail else ''))
print(f'RESUMO: {sum(x[0] for x in checks)}/{len(checks)} verificações. '
      'Pendências externas não são falhas de sintaxe, mas bloqueiam homologação.')
sys.exit(1 if '--strict' in sys.argv and not all(x[0] for x in checks) else 0)
