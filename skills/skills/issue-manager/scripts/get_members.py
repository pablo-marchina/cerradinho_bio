"""Lista os membros do projeto g01 (inclui herdados do grupo) no GitLab do Inteli (somente leitura).

Uso: python scripts/get_members.py [--project graduacao/2026-2b/t24/g01]
"""
import argparse
import sys

from audit_cards import DEFAULT_PROJECT, Api

ap = argparse.ArgumentParser()
ap.add_argument('--project', default=DEFAULT_PROJECT)
args = ap.parse_args()
sys.stdout.reconfigure(encoding='utf-8')
members = Api(args.project).paged('/members/all')
print(f'=== {len(members)} MEMBROS ===')
for m in sorted(members, key=lambda x: x['username']):
    print(f"  @{m['username']}  {m['name']}  (nível {m['access_level']})")
