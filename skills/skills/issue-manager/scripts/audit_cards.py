"""Audit (and, on explicit request, fix) GitLab cards against the team card template
(.gitlab/issue_templates/card-kanban.md, in Portuguese) and the label/estimate rules graded in sprint reviews.

Read-only by default (run it on your own cards: each member is graded on their own issues):
    python scripts/audit_cards.py audit --milestone "Sprint 04" --assignee <your.username> [--iid 207 208] [--json]

Writes only through `apply`, one issue at a time, after the user has confirmed:
    python scripts/audit_cards.py apply --iid 207 [--add-labels "M,DOCUMENTATION"] [--remove-labels "G"]
        [--due 2026-09-11] [--estimate 1h] [--set-assignee user.name] [--start YYYY-MM-DD] [--spent-equals-estimate] [--milestone "Sprint 03"] [--description-file draft.md]

The token is read from the GITLAB_TOKEN user environment variable (Windows registry first, then the
process environment). It is never printed and never passed on a command line.
"""
import argparse
import json
import os
import re
import sys
import urllib.error
import urllib.parse
import urllib.request

HOST = 'https://git.inteli.edu.br/api/v4'
DEFAULT_PROJECT = 'graduacao/2026-2b/t24/g01'

PRIORITY = {'Prioridade Alta', 'Prioridade Média', 'Prioridade Baixa'}
SIZE = ['PP', 'P', 'M', 'G', 'GG']
# Team rule: PP < 30m, P = 30m, M = 1h, G = 2h up to under 3h, GG = 3h or more.
MIN_MINUTES = {'PP': 0, 'P': 30, 'M': 60, 'G': 120, 'GG': 180}
# An estimate that reaches the next size's floor is a warning: the size label is probably too small.
NEXT_FLOOR = {'PP': ('P', 30), 'P': ('M', 60), 'M': ('G', 120), 'G': ('GG', 180)}
START_DATE = r'(?im)^[ 	]*[-*]?[ 	]*\**(?:start date|data de in[ií]cio)\**:?\**[ 	]*(\d{4}-\d{2}-\d{2})'
DURATION = re.compile(r'(\d+)\s*(mo|[wdhm])')
UNITS = {'mo': 4 * 5 * 8 * 60, 'w': 5 * 8 * 60, 'd': 8 * 60, 'h': 60, 'm': 1}  # GitLab's 8h day, 5d week, 4w month
TYPE = {'Infraestrutura', 'CODE', 'DOCUMENTATION', 'DESIGN', 'DATA', 'infrastructure', 'BUSINESS', 'TEST', 'DEPLOY',
        'REQUIREMENTS', 'bug', 'BUG', 'chore', 'CODE_REVIEW', 'reunion'}
KANBAN = {'Backlog', 'Doing', 'Review', 'Waiting Review', 'Done'}

# Section headings in template order. Accepts "## DoR:" and "**DoR:**" spellings.
SECTIONS = [
    ('titulo', r'T[ií]tulo'),
    ('descricao', r'Descri[cç][aã]o(?:[ \t]*\(detalhada\))?'),
    ('dor', r'DoR(?:[ \t]*\(Definition of Ready\))?'),
    ('dod', r'DoD(?:[ \t]*\(Definition of Done\))?'),
    ('criterios', r'Crit[eé]rios[ \t]+de[ \t]+Aceita[cç][aã]o'),
    ('links', r'Links(?:[ \t]*\(se houver\))?'),
    ('observacoes', r'Observa[cç](?:[aã]o|[oõ]es)'),
    ('developer', r'Desenvolvedor(?:\(a\)|a)?'),
    ('reviewer', r'Revisor(?:\(a\)|a)?'),
]
OPTIONAL_BODY = {'links', 'observacoes'}  # heading required, content may be "None"/empty
SOFT_SECTIONS = {'titulo'}  # the GitLab title field already carries it, so absence or emptiness is only a warning

PLACEHOLDER = re.compile(r'<[^>@\n]{3,}>|\[(?:insert|describe|add|todo)[^\]]*\]|\bTODO\b|\bTBD\b|lorem ipsum', re.I)
# Not placeholders: code, HTML comments, autolinks (<https://...>) and common HTML tags. A tag may only
# carry name="value" attributes (or `open`), so "<a short description>" still reads as a placeholder.
NOT_PLACEHOLDER = re.compile(
    r'```.*?```|`[^`\n]*`|<!--.*?-->|<(?:https?|mailto):[^>\s]*>'
    r'|</?(?:details|summary|br|hr|img|a|b|i|em|strong|sub|sup|kbd|p|div|span|ul|ol|li|table|tr|td|th)'
    r'''(?:\s+(?:open|[\w-]+=(?:"[^"]*"|'[^']*'|[^\s>]+)))*\s*/?>''',
    re.S | re.I)
HANDLE = re.compile(r'(?<![\w.])@([\w.\-]+)')  # the lookbehind skips e-mail addresses
FOOTER = re.compile(r'Generated with \[?Claude Code|Co-Authored-By:\s*Claude|claude\.com/claude-code', re.I)



def token():
    try:
        import winreg
        with winreg.OpenKey(winreg.HKEY_CURRENT_USER, 'Environment') as k:
            return winreg.QueryValueEx(k, 'GITLAB_TOKEN')[0]
    except Exception:
        t = os.environ.get('GITLAB_TOKEN')
        if not t:
            sys.exit('GITLAB_TOKEN is not set.')
        return t


class Api:
    def __init__(self, project):
        self.project = project
        self.base = f"{HOST}/projects/{urllib.parse.quote(project, safe='')}"
        self.tok = token()

    def graphql(self, query, variables=None):
        data = json.dumps({'query': query, 'variables': variables or {}}).encode()
        req = urllib.request.Request(HOST.replace('/api/v4', '/api/graphql'), data=data, method='POST',
                                     headers={'PRIVATE-TOKEN': self.tok, 'Content-Type': 'application/json'})
        try:
            with urllib.request.urlopen(req) as r:
                out = json.load(r)
        except urllib.error.HTTPError as e:
            sys.exit(f'graphql -> HTTP {e.code}: {e.read().decode(errors="replace")[:300]}')
        if out.get('errors'):
            sys.exit(f"graphql: {out['errors'][0]['message']}")
        return out['data']

    def dates(self, iids):
        """{iid: (work item gid, startDate, dueDate)} from the Datas field of the card (START_AND_DUE_DATE widget).

        The REST API only exposes due_date; the start date the board shows lives only in this widget."""
        q = '''query($p: ID!, $iids: [String!]) { project(fullPath: $p) { workItems(iids: $iids, first: 100) {
                 nodes { iid id widgets { type ... on WorkItemWidgetStartAndDueDate { startDate dueDate } } } } } }'''
        out = {}
        for k in range(0, len(iids), 100):
            data = self.graphql(q, {'p': self.project, 'iids': [str(i) for i in iids[k:k + 100]]})
            for n in data['project']['workItems']['nodes']:
                w = next((w for w in n['widgets'] if w['type'] == 'START_AND_DUE_DATE'), {})
                out[int(n['iid'])] = (n['id'], w.get('startDate'), w.get('dueDate'))
        return out

    def set_dates(self, iid, start=None, due=None):
        """Write start and/or due date into the card's Datas field; the date not given keeps its value."""
        gid, cur_start, cur_due = self.dates([iid])[iid]
        m = '''mutation($id: WorkItemID!, $s: Date, $d: Date) { workItemUpdate(input: {id: $id,
                 startAndDueDateWidget: {startDate: $s, dueDate: $d}}) { errors } }'''
        errs = self.graphql(m, {'id': gid, 's': start or cur_start, 'd': due or cur_due})['workItemUpdate']['errors']
        if errs:
            sys.exit(f'set dates: {errs}')

    def call(self, method, path, body=None):
        data = json.dumps(body).encode() if body is not None else None
        req = urllib.request.Request(self.base + path, data=data, method=method,
                                     headers={'PRIVATE-TOKEN': self.tok, 'Content-Type': 'application/json'})
        try:
            with urllib.request.urlopen(req) as r:
                return json.load(r)
        except urllib.error.HTTPError as e:
            sys.exit(f'{method} {path} -> HTTP {e.code}: {e.read().decode(errors="replace")[:300]}')

    def call_global(self, method, path):
        req = urllib.request.Request(HOST + path, method=method, headers={'PRIVATE-TOKEN': self.tok})
        try:
            with urllib.request.urlopen(req) as r:
                return json.load(r)
        except urllib.error.HTTPError as e:
            sys.exit(f'{method} {path} -> HTTP {e.code}')

    def paged(self, path):
        out, page = [], 1
        sep = '&' if '?' in path else '?'
        while True:
            chunk = self.call('GET', f'{path}{sep}per_page=100&page={page}')
            out += chunk
            if len(chunk) < 100:
                return out
            page += 1


def minutes(human):
    if not human:
        return 0
    return sum(int(n) * UNITS[u] for n, u in DURATION.findall(human))


def split_sections(desc):
    """Return {key: (start_index, body_text, styled)} for every template heading found."""
    found = []
    for key, pat in SECTIONS:
        # [ \t]* and not \s*: \s also matches newlines, which let an empty section swallow the next heading.
        matches = list(re.finditer(rf'^[ \t]*(#+[ \t]*|\*\*)?[ \t]*{pat}[ \t]*(?(1):?|:)[ \t]*(?:\*\*)?[ \t]*:?(.*)$',
                                   desc, re.I | re.M))
        if matches:
            # Prefer a ## heading, then a **bold** one, over a prose line that happens to start with "Developer:".
            m = min(matches, key=lambda x: (0 if (x.group(1) or '').startswith('#') else 1 if x.group(1) else 2,
                                            x.start()))
            found.append((m.start(), m.end(), key, m.group(2).strip(), bool(m.group(1))))
    found.sort()
    result = {}
    for n, (start, end, key, inline, styled) in enumerate(found):
        nxt = found[n + 1][0] if n + 1 < len(found) else len(desc)
        body = (inline + '\n' + desc[end:nxt]).strip().strip('-').strip()
        result[key] = (start, body, styled)
    return result


def audit_issue(issue, members, milestone_due, start_field=None):
    errors, warns = [], []
    labels = set(issue['labels'])
    desc = issue.get('description') or ''

    if not issue['assignees']:
        errors.append('no assignee')
    if not issue.get('milestone'):
        errors.append('no milestone')
    if not issue.get('due_date'):
        errors.append(f'no due date (milestone due: {milestone_due or "unset"})')
    if not start_field:
        errors.append('no start date in the card Datas field (fix with --start YYYY-MM-DD)')

    pr = labels & PRIORITY
    if len(pr) != 1:
        errors.append(f'priority label: expected 1, found {sorted(pr) or "none"}')
    sz = [s for s in SIZE if s in labels]
    if len(sz) != 1:
        errors.append(f'size label: expected 1, found {sz or "none"}')
    if not labels & TYPE:
        errors.append('no type label (CODE/DOCUMENTATION/DESIGN/DATA/infrastructure/...)')
    if not any(label.lower().startswith(('artifact', 'artefact', 'artefato')) for label in labels):
        warns.append('no Artifact label (fine if the card is not tied to an artifact)')

    col = labels & KANBAN
    if not col and issue['state'] == 'closed':
        warns.append('closed with no kanban label (the board shows it under Closed; add Done to be explicit)')
    elif len(col) != 1:
        errors.append(f'kanban column label: expected 1, found {sorted(col) or "none"}')
    elif issue['state'] == 'closed' and col != {'Done'}:
        errors.append(f'issue is closed but kanban label is {next(iter(col))}, expected Done')

    est = issue['time_stats']['human_time_estimate']
    if not est:
        errors.append('no time estimate')
    elif len(sz) == 1 and minutes(est) < MIN_MINUTES[sz[0]]:
        errors.append(f'estimate {est} below the {sz[0]} minimum ({MIN_MINUTES[sz[0]]}m)')
    elif len(sz) == 1 and sz[0] in NEXT_FLOOR and minutes(est) >= NEXT_FLOOR[sz[0]][1]:
        bigger, floor = NEXT_FLOOR[sz[0]]
        warns.append(f'estimate {est} reaches the {bigger} floor ({floor}m); size {sz[0]} may be too small')

    ts = issue['time_stats']
    if not ts['total_time_spent'] and (issue['state'] == 'closed' or col & {'Review', 'Waiting Review', 'Done'}):
        errors.append('no time spent, but the work is done or in review (log it with --spent 2h or --spent-equals-estimate)')
    elif issue['state'] == 'closed' and ts['time_estimate'] and ts['total_time_spent'] != ts['time_estimate']:
        errors.append(f"time spent {ts['human_total_time_spent'] or 'none'} differs from the estimate {est} "
                      "(closed cards record spent = estimate; fix with --spent-equals-estimate)")

    if not desc.strip():
        errors.append('empty description')
        return errors, warns
    if not re.search(START_DATE, desc):
        errors.append('no "Data de início: YYYY-MM-DD" under ## Datas (fix with --start YYYY-MM-DD)')

    secs = split_sections(desc)
    missing = [k for k, _ in SECTIONS if k not in secs and k not in SOFT_SECTIONS]
    if missing:
        errors.append(f'missing sections: {", ".join(missing)}')
    if 'titulo' not in secs:
        warns.append('no "## Título:" section in the body')
    elif not secs['titulo'][1]:
        warns.append('"## Título:" section in the body is empty')
    plain = [k for k, v in secs.items() if not v[2]]
    if plain:
        warns.append(f'headings without ## formatting: {", ".join(plain)}')
    order = [k for _, k in sorted((v[0], k) for k, v in secs.items())]
    expected = [k for k, _ in SECTIONS if k in secs]
    if order != expected:
        warns.append('sections are out of template order')
    for key, (_, body, _styled) in secs.items():
        if key in OPTIONAL_BODY or key in SOFT_SECTIONS or key in ('developer', 'reviewer'):
            continue
        if not body:
            errors.append(f'section "{key}" is empty')
    if 'descricao' in secs and 0 < len(secs['descricao'][1]) < 150:
        warns.append(f'Descrição (detalhada) is only {len(secs["descricao"][1])} chars')

    people = {}
    for role in ('developer', 'reviewer'):
        body = secs.get(role, (0, '', True))[1]
        handles = [h for h in (x.rstrip('.') for x in HANDLE.findall(body)) if h]  # usernames never end in "."
        people[role] = handles
        if role in secs and not handles:
            errors.append(f'{role} has no @username')
        for h in handles:
            if h not in members:
                errors.append(f'{role} @{h} is not a project member')
    if set(people['developer']) & set(people['reviewer']):
        errors.append('reviewer is also the developer (self-review)')
    assignees = {a['username'] for a in issue['assignees']}
    if people['developer'] and assignees and not assignees & set(people['developer']):
        warns.append(f'Developer {people["developer"]} does not match assignees {sorted(assignees)}')

    if FOOTER.search(desc):
        errors.append('Claude Code attribution in the description')
    for m in PLACEHOLDER.finditer(NOT_PLACEHOLDER.sub('', desc)):
        errors.append(f'leftover template placeholder: {m.group(0)!r}')

    return errors, warns


def cmd_audit(api, args):
    members = {m['username'] for m in api.paged('/members/all')}
    milestones = {m['title']: m.get('due_date') for m in api.paged('/milestones')}
    if args.iid:
        issues = [api.call('GET', f'/issues/{i}') for i in args.iid]
    else:
        q = {'scope': 'all', 'state': args.state}
        if args.milestone:
            q['milestone'] = args.milestone
        if args.assignee:
            q['assignee_username'] = args.assignee
        issues = api.paged('/issues?' + urllib.parse.urlencode(q))
    issues.sort(key=lambda i: i['iid'])
    dates = api.dates([i['iid'] for i in issues]) if issues else {}

    report = []
    for i in issues:
        ms = (i.get('milestone') or {}).get('title')
        start = dates.get(i['iid'], (None, None, None))[1]
        errors, warns = audit_issue(i, members, milestones.get(ms or args.milestone), start)
        report.append({'iid': i['iid'], 'title': i['title'], 'state': i['state'], 'url': i['web_url'],
                       'labels': i['labels'], 'milestone': ms, 'start_date': start, 'due_date': i.get('due_date'),
                       'estimate': i['time_stats']['human_time_estimate'],
                       'assignees': [a['username'] for a in i['assignees']],
                       'errors': errors, 'warnings': warns})
    if args.json:
        print(json.dumps(report, ensure_ascii=False, indent=2))
        return
    ok = sum(1 for r in report if not r['errors'])
    print(f'{len(report)} cards audited, {ok} fully conformant, {len(report) - ok} with errors\n')
    for r in report:
        mark = 'OK ' if not r['errors'] else 'FIX'
        print(f"[{mark}] #{r['iid']} {r['title'][:70]} ({r['state']})")
        print(f"      labels={r['labels']} milestone={r['milestone']} start={r['start_date']} due={r['due_date']} "
              f"estimate={r['estimate']} assignees={r['assignees']}")
        for e in r['errors']:
            print(f'      ERROR  {e}')
        for w in r['warnings']:
            print(f'      warn   {w}')


def set_start_date(text, date, due=None):
    """Set the 'Data de início' line under ## Datas, creating the line (or the section) when missing."""
    if re.search(START_DATE, text):
        return re.sub(r'(?im)^([ \t]*[-*]?[ \t]*\**(?:start date|data de in[ií]cio)\**:?\**[ \t]*)\d{4}-\d{2}-\d{2}',
                      lambda m: m.group(1) + date, text, count=1)
    if re.search(r'(?im)^##\s*Datas:?', text):
        return re.sub(r'(?im)^(##\s*Datas:?[ \t]*\n+)', lambda m: m.group(1) + f'- Data de início: {date}\n', text, count=1)
    block = f'## Datas:\n\n- Data de início: {date}\n' + (f'- Conclusão prevista: {due}\n' if due else '') + '\n'
    m = re.search(r'(?im)^##\s*Desenvolvedor', text)
    return text[:m.start()] + block + text[m.start():] if m else text.rstrip() + '\n\n' + block


def cmd_apply(api, args):
    iid = args.iid[0]
    # Validate before the first write, so a bad value cannot leave the card half-updated.
    if args.estimate and not re.fullmatch(r'\s*(?:\d+\s*(?:mo|[wdhm])\s*)+', args.estimate):
        sys.exit(f'--estimate {args.estimate!r} is not a GitLab duration (e.g. 30m, 1h, 1h 30m)')
    if args.due and not re.fullmatch(r'\d{4}-\d{2}-\d{2}', args.due):
        sys.exit(f'--due {args.due!r} is not YYYY-MM-DD')
    before = api.call('GET', f'/issues/{iid}')
    before['start_field'] = api.dates([iid])[iid][1]
    body = {}
    if args.add_labels:
        body['add_labels'] = args.add_labels
    if args.remove_labels:
        body['remove_labels'] = args.remove_labels
    if args.due:
        body['due_date'] = args.due
    if args.set_assignee:
        users = api.call_global('GET', f'/users?username={urllib.parse.quote(args.set_assignee)}')
        if not users:
            sys.exit(f'user {args.set_assignee!r} not found')
        body['assignee_ids'] = [users[0]['id']]
    if args.milestone:
        match = [m for m in api.paged('/milestones') if m['title'] == args.milestone]
        if not match:
            sys.exit(f'milestone {args.milestone!r} not found')
        body['milestone_id'] = match[0]['id']
    if args.start:
        if not re.fullmatch(r'\d{4}-\d{2}-\d{2}', args.start):
            sys.exit(f'--start {args.start!r} is not YYYY-MM-DD')
        base = open(args.description_file, encoding='utf-8').read() if args.description_file else before['description'] or ''
        body['description'] = set_start_date(base, args.start, args.due)
    if args.description_file and not args.start:
        with open(args.description_file, encoding='utf-8') as f:
            text = f.read()
        if FOOTER.search(text):
            sys.exit('refusing: the description file contains Claude Code attribution')
        body['description'] = text
    if 'description' in body and FOOTER.search(body['description']):
        sys.exit('refusing: the description contains Claude Code attribution')
    if body:
        api.call('PUT', f'/issues/{iid}', body)
    if args.start:
        api.set_dates(iid, start=args.start, due=args.due)
    if args.estimate:
        api.call('POST', f'/issues/{iid}/time_estimate?duration={urllib.parse.quote(args.estimate)}')
    if args.spent_equals_estimate or args.spent:
        want = args.spent or api.call('GET', f'/issues/{iid}')['time_stats']['human_time_estimate']
        if not want:
            sys.exit('no estimate to copy: set --estimate first or pass --spent')
        api.call('POST', f'/issues/{iid}/reset_spent_time')
        api.call('POST', f'/issues/{iid}/add_spent_time?duration={urllib.parse.quote(want)}')
    after = api.call('GET', f'/issues/{iid}')
    after['start_field'] = api.dates([iid])[iid][1]
    for field, get in [('labels', lambda i: i['labels']),
                       ('milestone', lambda i: (i.get('milestone') or {}).get('title')),
                       ('assignees', lambda i: [a['username'] for a in i['assignees']]),
                       ('start date (campo Datas)', lambda i: i['start_field']),
                       ('due_date', lambda i: i.get('due_date')),
                       ('estimate', lambda i: i['time_stats']['human_time_estimate']),
                       ('spent', lambda i: i['time_stats']['human_total_time_spent']),
                       ('data de início', lambda i: (re.search(START_DATE, i.get('description') or '') or [None, None])[1]),
                       ('description chars', lambda i: len(i.get('description') or ''))]:
        b, a = get(before), get(after)
        print(f"#{iid} {field}: {b} -> {a}" if a != b else f"#{iid} {field}: unchanged")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('command', choices=['audit', 'apply'])
    ap.add_argument('--project', default=DEFAULT_PROJECT)
    ap.add_argument('--milestone')
    ap.add_argument('--assignee')
    ap.add_argument('--state', default='all', choices=['all', 'opened', 'closed'])
    ap.add_argument('--iid', nargs='+', type=int)
    ap.add_argument('--json', action='store_true')
    ap.add_argument('--add-labels')
    ap.add_argument('--remove-labels')
    ap.add_argument('--due')
    ap.add_argument('--start', help='apply: set the start date in the card Datas field and the "Data de início" line in the description (YYYY-MM-DD)')
    ap.add_argument('--spent', help='apply: set time spent to this duration (replaces what was logged)')
    ap.add_argument('--spent-equals-estimate', action='store_true', help='apply: set time spent equal to the estimate')
    ap.add_argument('--set-assignee', help='apply: username to assign the card to, e.g. arthur.loyola')
    ap.add_argument('--estimate')
    ap.add_argument('--description-file')
    args = ap.parse_args()
    sys.stdout.reconfigure(encoding='utf-8')
    api = Api(args.project)
    if args.command == 'audit':
        if not (args.milestone or args.assignee or args.iid):
            sys.exit('audit needs --milestone, --assignee or --iid')
        cmd_audit(api, args)
    else:
        if not args.iid or len(args.iid) != 1:
            sys.exit('apply takes exactly one --iid')
        cmd_apply(api, args)


if __name__ == '__main__':
    main()
