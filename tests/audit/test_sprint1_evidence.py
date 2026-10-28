import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[2] / 'scripts'))
from sprint1_evidence import evaluate_external, section_word_count

class EvidenceTests(unittest.TestCase):
    def test_blank_or_prose_cannot_certify_external(self):
        for value in (None, {}, {'status': 'pendente'}, {'status': 'aprovado'}):
            self.assertFalse(evaluate_external(value, 'lgpd')[0])
    def test_missing_result_rejected(self):
        data = {'status':'aprovado','responsavel':'Equipe de testes',
                'data':'2026-10-24','evidencia_url':'https://exemplo.org/evidencia'}
        self.assertFalse(evaluate_external(data, 'wokwi')[0])
        data['commit'] = 'a'*40
        self.assertTrue(evaluate_external(data, 'wokwi')[0])
        data['evidencia_url'] = 'http://sem-tls.exemplo.org'
        self.assertFalse(evaluate_external(data, 'wokwi')[0])
    def test_word_count_tables(self):
        md='### 3.1.1. Título\n| A | B |\n|---|---|\n| um | dois |\n### 3.1.2. Próxima\n'
        self.assertEqual(section_word_count(md, '### 3.1.1.', '### 3.1.2.'), 8)

if __name__ == '__main__': unittest.main()
