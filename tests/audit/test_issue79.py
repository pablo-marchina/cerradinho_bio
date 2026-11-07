"""Regressões do verificador de destinos e âncoras explícitas."""
import importlib.util
from pathlib import Path
import tempfile
import unittest

spec = importlib.util.spec_from_file_location('verify_issue79', Path(__file__).resolve().parents[2] / 'scripts/verify_issue79.py')
audit = importlib.util.module_from_spec(spec)
spec.loader.exec_module(audit)


class LinkTests(unittest.TestCase):
    def check(self, source, destination=''):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'source.md'
            path.write_text(source, encoding='utf-8')
            (path.parent / 'target.md').write_text(destination, encoding='utf-8')
            return audit.check_links(path)

    def test_valid_cross_file_anchor(self):
        self.assertEqual([], self.check('[a](target.md#cenarios-planejados)', '<a name="cenarios-planejados"></a>'))

    def test_missing_anchor(self):
        self.assertTrue(self.check('[a](target.md#rtm-fisica)'))

    def test_missing_file(self):
        self.assertTrue(self.check('[a](missing.md)'))

    def test_duplicate_anchor(self):
        self.assertTrue(self.check('<a name="rtm-fisica"></a>\n<a name="rtm-fisica"></a>'))


if __name__ == '__main__':
    unittest.main()
