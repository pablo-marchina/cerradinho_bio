"""Regras puras para auditoria: conteúdo descritivo não prova homologação externa."""
import datetime
import json
import pathlib
import re

HEX_SHA = re.compile(r'^[0-9a-f]{40}$')

def section_word_count(markdown, heading, next_heading):
    """Conta conservadoramente palavras de seções, inclusive legendas e tabelas."""
    start = markdown.index(heading)
    end = markdown.index(next_heading, start + len(heading))
    return len(re.findall(r'\b[\wÀ-ÿ]+\b', markdown[start:end]))


def evaluate_external(record, kind):
    """Comprovação documental referenciável, não substitui revisão humana."""
    if not isinstance(record, dict) or record.get('status') != 'aprovado':
        return False, 'aguardando aprovação com evidência institucional ou execução oficial'
    for key in ('responsavel', 'data', 'evidencia_url'):
        val = record.get(key)
        if not isinstance(val, str) or not val.strip():
            return False, f'falta {key}'
    try:
        datetime.date.fromisoformat(record['data'])
    except ValueError:
        return False, 'data inválida'
    if not re.match(r'^https://[^\s]+$', record['evidencia_url']):
        return False, 'evidência precisa ter URL HTTPS de resultado verificável'
    if kind == 'wokwi' and not HEX_SHA.fullmatch(str(record.get('commit', ''))):
        return False, 'Wokwi deve identificar commit Git de 40 caracteres'
    return True, 'declaração com metadados completos; conferir documento/resultado externo'


def load_evidence(path):
    try:
        return json.loads(pathlib.Path(path).read_text(encoding='utf-8'))
    except (FileNotFoundError, ValueError):
        return {}
