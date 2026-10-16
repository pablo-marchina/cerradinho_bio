"""Lista as labels do projeto g01 no GitLab do Inteli (somente leitura).

Uso: python scripts/get_labels.py [--project graduacao/2026-2b/t24/g01]
O token vem da variável GITLAB_TOKEN (registro do Windows primeiro, depois o ambiente) e nunca é impresso.
"""
import argparse
import sys

from audit_cards import DEFAULT_PROJECT, Api

ap = argparse.ArgumentParser()
ap.add_argument('--project', default=DEFAULT_PROJECT)
args = ap.parse_args()
sys.stdout.reconfigure(encoding='utf-8')
labels = Api(args.project).paged('/labels')
print(f'=== {len(labels)} LABELS ===')
for label in sorted(labels, key=lambda x: x['name'].lower()):
    print(f"  {label['name']}  ({label['color']})")
