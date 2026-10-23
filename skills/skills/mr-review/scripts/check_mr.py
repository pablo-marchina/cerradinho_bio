"""Confere um merge request do g01 contra as regras do grupo (somente leitura) e, com o OK de quem revisa,
publica o comentário de revisão.

Uso (de dentro de skills/skills/mr-review):
    python scripts/check_mr.py check --iid 24 [--json]
    python scripts/check_mr.py drafts --iid 24
    python scripts/check_mr.py note --iid 24 --body-file revisao.md [--path document/documentacao.md --line 120]
    python scripts/check_mr.py note --iid 24 --body-file revisao-v2.md --note-id 493338

`check` só lê. `note` é o único comando que escreve: publica um comentário geral, com --path e --line
um comentário na linha nova do diff, ou, com --note-id, reescreve um comentário já postado. Nenhum comando aprova nem faz merge.
O token vem do GITLAB_TOKEN pela classe Api da skill issue-manager e nunca é impresso.

Rascunhos ficam fora do repositório, em <temp>/mr-review/<iid>/ (no Windows, a pasta temporária é a %TEMP%):
`drafts` cria e mostra essa pasta, e um --body-file sem caminho absoluto é procurado nela. Arquivo dentro
do repositório é recusado, porque aparece como mudança no VS Code e acaba entrando em commit.
"""
import argparse
import json
import os
import re
import sys
import tempfile
import urllib.error
import urllib.parse
import urllib.request

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'issue-manager', 'scripts'))
from audit_cards import DEFAULT_PROJECT, FOOTER, HANDLE, Api  # noqa: E402

TYPES = 'feat|fix|docs|test|refactor|style|chore|build|ci'
BRANCH = re.compile(rf'^(?:{TYPES})/\d{{2,}}-[a-z0-9]+(?:-[a-z0-9]+)*-sprint[1-9]\d*$')
COMMIT = re.compile(rf'^(?:{TYPES})\(#(\d+)\)!?: \S')
TITLE = re.compile(rf'^(?:{TYPES})\(#\d+(?:,\s*#\d+)*\s*\)!?: \S')
AI = re.compile(FOOTER.pattern + r'|claude\.ai/(?:chat|share)/|chatgpt\.com/(?:c|share)/|gemini\.google\.com/share/', re.I)
REPORT = re.compile(r'outros/reports|reports[/\\]Sprint\d', re.I)
ID_CITED = re.compile(r'\b(R[FN]\d{2,3}|CT\d{2,3})\b')
ID_DEFINED = re.compile(r'^\|\s*(R[FN]\d{2,3}|CT\d{2,3})\s*\|', re.M)
DOC = 'document/documentacao.md'
INTELI = '@sou.inteli.edu.br'
DRAFTS = os.path.join(tempfile.gettempdir(), 'mr-review')
REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..'))
IN_REVIEW = {'Waiting Review', 'Review'}


def raw_file(api, path, ref):
    """Conteúdo de um arquivo numa branch, ou None se ele não existir lá."""
    url = f"{api.base}/repository/files/{urllib.parse.quote(path, safe='')}/raw?ref={urllib.parse.quote(ref, safe='')}"
    req = urllib.request.Request(url, headers={'PRIVATE-TOKEN': api.tok})
    try:
        with urllib.request.urlopen(req) as r:
            return r.read().decode('utf-8', errors='replace')
    except urllib.error.HTTPError:
        return None


def try_get(api, path):
    try:
        return api.call('GET', path)
    except SystemExit:
        return None


def diff_lines(diff):
    """(linhas adicionadas, linhas removidas) de um diff unificado, sem o sinal."""
    added, removed = [], []
    for line in diff.splitlines():
        if line.startswith('+') and not line.startswith('+++'):
            added.append(line[1:])
        elif line.startswith('-') and not line.startswith('---'):
            removed.append(line[1:])
    return added, removed


def main_issue(mr, commits):
    """Número da issue do MR: título, depois início da branch, depois o mais citado nos commits."""
    m = re.search(r'#(\d+)', mr['title']) or re.match(r'\D*?(\d+)', mr['source_branch'])
    if m:
        return int(m.group(1))
    nums = [int(n) for c in commits for n in re.findall(r'#(\d+)', c['title'])]
    return max(set(nums), key=nums.count) if nums else None


def check(api, iid):
    out = []

    def erro(msg):
        out.append(('ERRO', msg))

    def aviso(msg):
        out.append(('AVISO', msg))

    mr = api.call('GET', f'/merge_requests/{iid}')
    commits = api.paged(f'/merge_requests/{iid}/commits')
    diffs = api.paged(f'/merge_requests/{iid}/diffs')
    issue_n = main_issue(mr, commits)
    mr_nums = {int(n) for n in re.findall(r'#(\d+)', mr['title'])} or ({issue_n} if issue_n else set())
    author = mr['author']['username']
    reviewers = [r['username'] for r in mr.get('reviewers', [])]

    # MR
    if mr['target_branch'] == 'main' and 'snapshot' not in mr['source_branch']:
        erro('alvo é a main: só a branch de snapshot (skill repo-snapshot) vai para a main; o resto vai para a develop')
    elif mr['target_branch'] not in ('develop', 'main'):
        aviso(f"alvo é {mr['target_branch']}, não a develop: confirme que foi combinado")
    if not BRANCH.match(mr['source_branch']) and 'snapshot' not in mr['source_branch']:
        aviso(f"branch {mr['source_branch']!r} fora do padrão tipo/NN-descricao-sprintN (AGENT.md, seção 7)")
    if not TITLE.match(mr['title']):
        aviso(f"título {mr['title']!r} fora do padrão tipo(#NN): descrição (AGENT.md, seção 7)")
    if not reviewers:
        erro('MR sem revisor')
    elif author in reviewers:
        erro('quem abriu o MR também é revisor')
    if len((mr.get('description') or '').strip()) < 100:
        aviso('descrição do MR vazia ou curta: diga o que muda, por quê e como conferir')
    if AI.search(mr.get('description') or ''):
        erro('descrição do MR com coautoria ou link de conversa de IA (AGENT.md, seção 7)')
    if mr.get('has_conflicts'):
        erro('MR com conflito com a branch alvo')
    if mr.get('draft'):
        aviso('MR em draft: confirme com o autor se já é para revisar')

    # Commits
    merges = [c for c in commits if len(c.get('parent_ids', [])) > 1 or c['title'].startswith('Merge ')]
    for c in commits:
        if c in merges:
            continue
        short = f"{c['short_id']} {c['title'][:70]!r}"
        m = COMMIT.match(c['title'])
        if not m:
            aviso(f'commit fora do padrão tipo(#NN): descrição: {short}')
        nums = {int(n) for n in re.findall(r'#(\d+)', c['title'])}
        if nums and mr_nums and not nums & mr_nums:
            erro(f'commit de outra issue ({", ".join(f"#{n}" for n in sorted(nums))}) dentro do MR da #{issue_n}: {short}')
        if not c['author_email'].endswith(INTELI):
            aviso(f"commit com e-mail fora do Inteli ({c['author_email']}): o GitLab não liga ao perfil: {short}")
        if AI.search(c['message']):
            erro(f'commit com coautoria ou link de conversa de IA: {short}')
    if merges:
        aviso(f'{len(merges)} merge(s) de outra branch no MR: confira no diff se nada da develop foi desfeito')

    # Card ligado
    if not issue_n:
        erro('não achei a issue do MR (título, branch ou commits)')
    else:
        issue = try_get(api, f'/issues/{issue_n}')
        if not issue:
            erro(f'issue #{issue_n} não existe no projeto')
        else:
            labels = set(issue['labels'])
            if mr['state'] == 'opened' and not labels & IN_REVIEW:
                col = ', '.join(sorted(labels & {'Backlog', 'Doing'})) or 'sem coluna'
                aviso(f'card #{issue_n} em {col} com o MR aberto: mover para Waiting Review (skill issue-manager)')
            if mr['state'] == 'opened' and issue['state'] == 'closed':
                aviso(f'card #{issue_n} fechado com o MR ainda aberto')
            if not issue['time_stats']['total_time_spent']:
                aviso(f'card #{issue_n} sem tempo gasto registrado')
            if not issue.get('milestone'):
                aviso(f'card #{issue_n} sem milestone')
            desc = issue.get('description') or ''
            rev = re.search(r'(?is)Revisor(?:\(a\)|a)?:?\**\s*(.*?)(?:\n##|\n---|$)', desc)
            named = set(HANDLE.findall(rev.group(1))) if rev else set()
            if reviewers and named and not named & set(reviewers):
                aviso(f"revisor do card ({', '.join('@' + n for n in sorted(named))}) diferente do revisor do MR "
                      f"({', '.join('@' + r for r in reviewers)})")

    # Diff
    doc_added = []
    for d in diffs:
        path = d['new_path']
        added, removed = diff_lines(d.get('diff') or '')
        rules = path == 'AGENT.md' or path.startswith('skills/')  # esses arquivos citam a regra de propósito
        if not rules and any(AI.search(line) for line in added):
            erro(f'{path}: linha adicionada com coautoria ou link de conversa de IA')
        deliverable = path.startswith('document/') and not path.startswith('document/outros/') or path == 'README.md'
        if deliverable:
            for line in added:
                if REPORT.search(line):
                    erro(f'{path}: cita report interno como fonte (AGENT.md, seção 4): {line.strip()[:90]!r}')
        if path.endswith('.md'):
            kept = {h.lstrip('#').strip() for h in added if h.startswith('#')}
            gone = [h.lstrip('#').strip() for h in removed if h.startswith('#') and h.lstrip('#').strip() not in kept]
            if gone:
                more = f' e mais {len(gone) - 3}' if len(gone) > 3 else ''
                aviso(f"{path}: {len(gone)} título(s) removido(s) ({'; '.join(g[:50] for g in gone[:3])}{more}): "
                      'confira se é do escopo do card e não desfaz trabalho de outra pessoa')
        if path == DOC:
            doc_added = added
            if len(removed) > 30:
                aviso(f'{DOC}: {len(removed)} linhas removidas: confira se a remoção é do escopo do card')
            for k, line in enumerate(added):
                if '![' in line and not any(re.search(r'Figura|Fonte', a) for a in added[k + 1:k + 5]):
                    aviso(f'{DOC}: figura sem legenda (*Figura — ...* e fonte) logo abaixo: {line.strip()[:80]!r}')

    if doc_added:
        head = raw_file(api, DOC, mr['source_branch']) or ''
        defined = set(ID_DEFINED.findall(head))
        cited = {i for line in doc_added for i in ID_CITED.findall(line)}
        missing = sorted(cited - defined)
        if missing and defined:
            erro(f"{DOC}: IDs citados que não estão definidos em nenhuma tabela da branch: {', '.join(missing)}")
        target = raw_file(api, DOC, mr['target_branch']) or ''
        only_here = sorted((cited & defined) - set(ID_DEFINED.findall(target)) - set(ID_DEFINED.findall('\n'.join(doc_added))))
        if only_here:
            aviso(f"{DOC}: IDs citados que não existem na {mr['target_branch']} nem são criados neste MR: "
                  f"{', '.join(only_here)} (dependem de outro MR?)")

    info = {'iid': iid, 'title': mr['title'], 'url': mr['web_url'], 'state': mr['state'], 'author': author,
            'reviewers': reviewers, 'source': mr['source_branch'], 'target': mr['target_branch'], 'issue': issue_n,
            'commits': len(commits), 'files': [d['new_path'] for d in diffs]}
    return info, out


def draft_dir(iid):
    path = os.path.join(DRAFTS, str(iid))
    os.makedirs(path, exist_ok=True)
    return path


def draft_file(iid, name):
    """Caminho do rascunho: absoluto como veio, relativo dentro da pasta temporária do MR; nunca no repositório."""
    path = os.path.abspath(name if os.path.isabs(name) else os.path.join(draft_dir(iid), name))
    if os.path.commonpath([path, REPO]) == REPO:
        sys.exit(f'recusado: {path} está dentro do repositório; salve o rascunho em {draft_dir(iid)}')
    return path


def note(api, args):
    body = open(draft_file(args.iid, args.body_file), encoding='utf-8').read()
    if AI.search(body):
        sys.exit('recusado: o comentário tem coautoria ou link de conversa de IA')
    if args.note_id:
        r = api.call('PUT', f'/merge_requests/{args.iid}/notes/{args.note_id}', {'body': body})
        print(f"nota {r['id']} reescrita")
        return
    if args.path and args.line:
        refs = api.call('GET', f'/merge_requests/{args.iid}')['diff_refs']
        position = {'position_type': 'text', 'base_sha': refs['base_sha'], 'start_sha': refs['start_sha'],
                    'head_sha': refs['head_sha'], 'new_path': args.path, 'old_path': args.path, 'new_line': args.line}
        r = api.call('POST', f'/merge_requests/{args.iid}/discussions', {'body': body, 'position': position})
        print(f"comentário publicado em {args.path}:{args.line} (discussão {r['id']})")
    elif args.path or args.line:
        sys.exit('--path e --line vão juntos')
    else:
        r = api.call('POST', f'/merge_requests/{args.iid}/notes', {'body': body})
        print(f"comentário publicado (nota {r['id']})")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('command', choices=['check', 'note', 'drafts'])
    ap.add_argument('--iid', type=int, required=True)
    ap.add_argument('--project', default=DEFAULT_PROJECT)
    ap.add_argument('--json', action='store_true')
    ap.add_argument('--body-file', help='note: rascunho do comentário; nome solto é procurado na pasta de drafts')
    ap.add_argument('--path', help='note: arquivo do diff para comentário na linha')
    ap.add_argument('--line', type=int, help='note: número da linha nova no arquivo')
    ap.add_argument('--note-id', type=int, help='note: reescreve esta nota em vez de criar outra')
    args = ap.parse_args()
    sys.stdout.reconfigure(encoding='utf-8')
    if args.command == 'drafts':
        return print(draft_dir(args.iid))
    api = Api(args.project)
    if args.command == 'note':
        if not args.body_file:
            sys.exit('note precisa de --body-file')
        return note(api, args)
    info, found = check(api, args.iid)
    if args.json:
        print(json.dumps({**info, 'findings': [{'level': lv, 'msg': m} for lv, m in found]}, ensure_ascii=False, indent=2))
    else:
        print(f"!{info['iid']} {info['title']}\n  {info['source']} -> {info['target']} | autor @{info['author']} | "
              f"revisor {', '.join('@' + r for r in info['reviewers']) or '-'} | issue #{info['issue']} | "
              f"{info['commits']} commits, {len(info['files'])} arquivos | {info['state']}\n  {info['url']}")
        for lv, m in sorted(found, key=lambda x: x[0] != 'ERRO'):
            print(f'  [{lv}] {m}')
        print(f"  {sum(lv == 'ERRO' for lv, _ in found)} erro(s), {sum(lv == 'AVISO' for lv, _ in found)} aviso(s)")
    sys.exit(1 if any(lv == 'ERRO' for lv, _ in found) else 0)


if __name__ == '__main__':
    main()
