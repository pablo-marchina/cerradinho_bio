"""Roda os cenarios wokwi-cli dos CTs do V2 e confere os logs da Serial.

Uso: python tests/wokwi-ci/run_cts.py [nome-do-cenario ...]
     python tests/wokwi-ci/run_cts.py --checar nome-do-cenario ...  (so confere o log ja
     gravado em build/wokwi-ct/, por exemplo de uma execucao manual no Wokwi for VS Code)
Token: WOKWI_CLI_TOKEN no ambiente ou em .tools/wokwi-token.txt (ignorado pelo Git).
Pre-requisito: firmware demo compilado (.pio/build/demo) e chips/gps.chip.wasm.
Logs e capturas do LCD ficam em build/wokwi-ct/.
WOKWI_CT_PROJECT aponta para outra pasta com diagram.json e wokwi.toml (por exemplo, uma
copia do diagrama commitado enquanto o da pasta do circuito esta em edicao).
"""
import hashlib
import json
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
OUT = ROOT / 'build' / 'wokwi-ct'
CLI = ROOT / '.tools' / 'wokwi-cli.exe'
PROJECT = Path(os.environ.get('WOKWI_CT_PROJECT', ROOT / 'src' / 'firmware' / 'prototipo_wokwi'))


def parse(log):
    lines = log.splitlines()
    events = []
    for index, line in enumerate(lines):
        if line.startswith('{'):
            try:
                record = json.loads(line)
                record['_line'] = index
                events.append(record)
            except json.JSONDecodeError:
                pass
    return lines, events


def of(events, *types):
    return [e for e in events if e['type'] in types]


def line_of(lines, text, start=0):
    return next((i for i in range(start, len(lines)) if text in lines[i]), None)


# Cada verificacao recebe (lines, events) e devolve {CT: (ok, detalhe)}.
def check_ct01_03(lines, events):
    first = events[0]
    weekly = [e['dias_fora'] for e in of(events, 'STATUS_SEMANAL')]
    late = of(events, 'ALERTA_ATRASO')
    return {
        'CT01': (first['type'] == 'STATUS' and first['dias_fora'] == 0 and first['seal'] == 'intact'
                 and first['contexto_restaurado'] is False and first['ciclo_ativo'] is False,
                 f"primeiro evento {first['type']} dias_fora={first.get('dias_fora')} seal={first['seal']} ciclo_ativo={first.get('ciclo_ativo')}"),
        'CT02': (weekly == [7, 14], f'STATUS_SEMANAL nos dias {weekly}'),
        'CT03': (len(late) == 1 and late[0]['dias_fora'] == 6, f"ALERTA_ATRASO x{len(late)} em {[e['dias_fora'] for e in late]}"),
    }


def check_ct04(threshold):
    def check(lines, events):
        stay = of(events, 'ALERTA_PERMANENCIA')
        return {f'CT04 ({threshold})': (len(stay) == 1 and stay[0]['dias_fora'] == threshold,
                                        f"ALERTA_PERMANENCIA x{len(stay)} em {[e['dias_fora'] for e in stay]}")}
    return check


def check_ct05_07(lines, events):
    flow = [e['type'] for e in of(events, 'VIOLACAO_LACRE', 'LACRE_FECHADO', 'AUTORIZACAO_REGISTRADA',
                                  'RETIRADA_AUTORIZADA', 'LACRE_REAPLICADO')]
    violations = of(events, 'VIOLACAO_LACRE')
    return {
        'CT05': (flow[:2] == ['VIOLACAO_LACRE', 'LACRE_FECHADO'] and all(e['critico'] for e in violations),
                 f'sequencia {flow[:2]}'),
        'CT06': (flow[2:5] == ['AUTORIZACAO_REGISTRADA', 'RETIRADA_AUTORIZADA', 'LACRE_REAPLICADO'], f'sequencia {flow[2:5]}'),
        'CT07': (flow[5:] == ['AUTORIZACAO_REGISTRADA', 'VIOLACAO_LACRE', 'LACRE_FECHADO'], f'sequencia {flow[5:]}'),
    }


def check_ct08_09(lines, events):
    flush = line_of(lines, '[FILA] rede disponivel')
    stored = line_of(lines, '[FILA] guardado: VIOLACAO_LACRE')
    before = [e['type'] for e in events if flush is None or e['_line'] < flush]
    after = [e for e in events if flush is not None and e['_line'] > flush]
    violation = next((e for e in after if e['type'] == 'VIOLACAO_LACRE'), None)
    # Dia original: o registro reenviado traz o uptime e os dias do ciclo de quando ocorreu,
    # anteriores ao reenvio. dia_ocorrencia segue o relogio do dispositivo e dias_fora, o ciclo.
    beats = [int(m) for line in lines[:flush or 0] for m in re.findall(r'\[vivo\] tempo=(\d+) s', line)]
    days = [int(m) for line in lines[:flush or 0] for m in re.findall(r'dias_fora=(\d+)', line)]
    kept_day = (violation is not None and beats and days and violation['uptime_ms'] // 1000 < beats[-1]
                and violation['dias_fora'] < days[-1])
    return {
        'CT08': (stored is not None and 'VIOLACAO_LACRE' not in before and line_of(lines, '[FILA] 1 pendente') is not None,
                 f'guardada na linha {stored}; nenhum JSON de violacao antes da reconexao'),
        'CT09 (local)': ([e['type'] for e in after][:2] == ['VIOLACAO_LACRE', 'LACRE_FECHADO'] and kept_day
                         and violation['network_sim_available'] is False,
                         f"reenvio {[e['type'] for e in after][:2]}, dias_fora={violation and violation['dias_fora']} "
                         f"(dia {days and days[-1]} na reconexao), uptime={violation and violation['uptime_ms']} ms"),
    }


def check_ct10a(lines, events):
    flush = line_of(lines, '[FILA] rede disponivel')
    after = [e for e in events if flush is not None and e['_line'] > flush]
    dropped = sum('FILA_CHEIA: descartado' in line for line in lines)
    limits = [e['limiar_dias'] for e in after if e['type'] == 'LIMIAR_ALTERADO']
    ok = dropped == 3 and after and after[0]['type'] == 'VIOLACAO_LACRE' and limits == list(range(204, 211))
    return {'CT10 A': (ok, f'{dropped} descartes; reenvio: violacao={bool(after) and after[0]["type"]} limiares={limits}')}


def check_ct10b(lines, events):
    flush = line_of(lines, '[FILA] rede disponivel')
    after = [e['type'] for e in events if flush is not None and e['_line'] > flush]
    summary = of(events, 'VIOLACAO_LACRE_RESUMO')
    flow = [t for t in after if t in ('VIOLACAO_LACRE', 'VIOLACAO_LACRE_RESUMO')]
    ok = flow == ['VIOLACAO_LACRE'] * 8 + ['VIOLACAO_LACRE_RESUMO'] and summary[0]['quantidade'] == 4
    return {'CT10 B': (ok, f"{flow.count('VIOLACAO_LACRE')} individuais + resumo de {summary and summary[0]['quantidade']}")}


def check_ct11(lines, events):
    released = sum('BTN_AUTORIZAR solto antes' in line for line in lines)
    granted = len(of(events, 'AUTORIZACAO_REGISTRADA'))
    return {'CT11': (released == 3 and granted == 1, f'{released} toques curtos ignorados; {granted} autorizacao(oes)')}


def check_battery(lines, events):
    flow = [e['type'] for e in of(events, 'BATERIA_BAIXA', 'SENSOR_BATERIA_FALHA')]
    levels = [int(v) for v in re.findall(r'bateria_sim=(\d+)%', '\n'.join(lines))]
    return {
        'CT12': (flow[:2] == ['BATERIA_BAIXA', 'BATERIA_BAIXA'], f'alertas antes da falha: {flow[:2]}'),
        'CT13': (flow[2:] == ['SENSOR_BATERIA_FALHA'], f'apos ADC zero: {flow[2:]}'),
        'CT23': (min(levels) == 0 and max(levels) == 100 and any(45 <= v <= 55 for v in levels),
                 f'faixa observada {min(levels)}..{max(levels)}%'),
    }


def check_ct14_16(lines, events):
    reasons = [e['motivo'] for e in of(events, 'ENCERRAMENTO_RECUSADO')]
    closed = of(events, 'CICLO_ENCERRADO')
    started = of(events, 'CICLO_INICIADO')
    # O primeiro CICLO_INICIADO e o vinculo do boot (ciclo 1); o segundo, a reutilizacao.
    c, s = (closed or [{}])[0], (started or [{}])[-1]
    return {
        'CT14': (len(closed) == 1 and c['ciclo_ativo'] is False and c['dias_fora'] == 0 and c['ciclos_encerrados'] == 1
                 and [e['ciclo_id'] for e in started] == [1, 2] and s['dias_fora'] == 0,
                 f"encerrado ativo={c.get('ciclo_ativo')} dias={c.get('dias_fora')} n={c.get('ciclos_encerrados')}; novo ciclo_id={s.get('ciclo_id')}"),
        'CT15 (V2)': (not of(events, 'VINCULO_RECUSADO') and reasons[2:] == ['nf_retorno_ausente'],
                      f'VINCULO_RECUSADO x{len(of(events, "VINCULO_RECUSADO"))}; gesto com ciclo ativo -> recusa {reasons[2:]}'),
        'CT16': (reasons[:2] == ['nf_retorno_ausente', 'lacre_aberto'], f'motivos {reasons[:2]}'),
    }


def check_ct17_35(lines, events):
    off = line_of(lines, 'DISPOSITIVO_DESLIGADO')
    back = next((e['_line'] for e in events if e['type'] == 'STATUS' and off is not None and e['_line'] > off), None)
    silent = off is not None and back is not None and not any(
        lines[i].startswith(('{', '[vivo]')) for i in range(off + 1, back))
    nav = line_of(lines, 'tela=1 ', line_of(lines, '"type":"STATUS"') or 0)
    return {
        # Leitura bruta: os dois botoes juntos (cenario) ou um de cada vez (execucao manual).
        'CT17 (V2)': (nav is not None and line_of(lines, 'tela=5 ', nav) is not None
                      and line_of(lines, 'avancar=1') is not None and line_of(lines, 'voltar=1') is not None,
                      'navegacao circular e leitura bruta no heartbeat'),
        'CT35': (silent, f'desligado na linha {off}, religado com STATUS na linha {back}, sem saidas entre eles'),
    }


def check_ct43(lines, events):
    valid = of(events, 'location_sample')
    ages = [e['last_valid_age_ms'] for e in of(events, 'location_unavailable')
            if valid and e['_line'] > valid[0]['_line'] and e['last_valid_age_ms'] is not None]
    # O texto do LCD ("sem fix ainda", "ha N s", "ha N min") e conferido nas capturas.
    return {'CT43 (s, min)': (len(valid) >= 1 and ages and max(ages) >= 30_000,
                              f'{len(valid)} coleta(s) valida(s); idades sem fix {ages} ms')}


def check_ct44(lines, events):
    boot = events[0]
    link = next((e for e in events if e['type'] == 'CICLO_INICIADO'), None)
    refused = line_of(lines, 'sem ciclo ativo; inicie o ciclo antes da REMESSA')
    early = [e['type'] for e in events if link and e['_line'] < link['_line']
             and e['type'] in ('STATUS_SEMANAL', 'ALERTA_ATRASO', 'ALERTA_PERMANENCIA')]
    days_before = {int(d) for line in lines[:link['_line'] if link else 0] for d in re.findall(r'dias_fora=(\d+)', line)}
    # Contando do vinculo, o dia 7 chega ~7 s depois dele (o JSON do vinculo sai alguns ms apos
    # o inicio do ciclo); contando do boot, chegaria ~3 s depois.
    weekly = next((e for e in of(events, 'STATUS_SEMANAL')), None)
    since = weekly and link and weekly['uptime_ms'] - link['uptime_ms']
    return {'CT44': (boot['type'] == 'STATUS' and boot['ciclo_ativo'] is False and boot['dias_fora'] == 0
                     and refused is not None and link is not None and refused < link['_line']
                     and link['ciclo_id'] == 1 and link['dias_fora'] == 0 and link['uptime_ms'] > 4_000
                     and not early and days_before <= {0} and weekly['dias_fora'] == 7 and 6_500 <= since < 8_000,
                     f"boot ciclo_ativo={boot.get('ciclo_ativo')}; REMESSA recusada={refused is not None}; "
                     f"vinculo ciclo_id={link and link['ciclo_id']} em {link and link['uptime_ms']} ms; "
                     f"dias antes do vinculo {sorted(days_before)}; STATUS_SEMANAL dia {weekly and weekly['dias_fora']} "
                     f"{since} ms apos o vinculo")}


def check_ct45(lines, events):
    lost, back = of(events, 'GNSS_SEM_FIX'), of(events, 'GNSS_RECUPERADO')
    missed = [e for e in of(events, 'location_unavailable') if not back or e['_line'] < back[0]['_line']]
    queued = all(line_of(lines, f'[FILA] guardado: {t}') is not None for t in ('GNSS_SEM_FIX', 'GNSS_RECUPERADO'))
    ok = (len(lost) == 1 and lost[0]['motivo'] == 'sem_fix' and lost[0]['satelites'] == 0 and len(missed) >= 2
          and len(back) == 1 and back[0]['coletas_sem_fix'] == len(missed) and back[0]['tempo_sem_posicao_s'] > 0
          and not any(e['critico'] for e in lost + back) and queued)
    return {'CT45': (ok, f"GNSS_SEM_FIX x{len(lost)} motivo={lost and lost[0]['motivo']}; {len(missed)} coletas sem fix; "
                         f"GNSS_RECUPERADO x{len(back)} coletas_sem_fix={back and back[0]['coletas_sem_fix']} "
                         f"tempo={back and back[0]['tempo_sem_posicao_s']} s; na fila={queued}")}


SCENARIOS = {
    'ct01-03-ciclo': (60_000, check_ct01_03),
    'ct04-limiar10': (60_000, check_ct04(10)),
    'ct04-limiar150': (400_000, check_ct04(150)),
    'ct05-07-lacre': (60_000, check_ct05_07),
    'ct08-09-fila': (60_000, check_ct08_09),
    'ct10a-fila-cheia': (60_000, check_ct10a),
    'ct10b-resumo': (60_000, check_ct10b),
    'ct11-botao': (60_000, check_ct11),
    'ct12-13-23-bateria': (60_000, check_battery),
    'ct14-16-encerramento': (90_000, check_ct14_16),
    'ct17-35-interface': (90_000, check_ct17_35),
    'ct43-posicao': (90_000, check_ct43),
    'ct44-vinculo': (60_000, check_ct44),
    'ct45-gnss': (120_000, check_ct45),
}


def main(names):
    if names[:1] == ['--checar']:
        results = []
        for name in names[1:]:
            lines_events = parse((OUT / f'{name}.serial.txt').read_text(encoding='utf-8', errors='replace'))
            try:
                results += [(name, ct, ok, detail) for ct, (ok, detail) in SCENARIOS[name][1](*lines_events).items()]
            except (IndexError, KeyError, TypeError, ValueError) as error:
                results.append((name, 'LOG', False, f'log incompleto: {error!r}'))
        report(results)
    env = dict(os.environ)
    token_file = ROOT / '.tools' / 'wokwi-token.txt'
    if not env.get('WOKWI_CLI_TOKEN') and token_file.exists():
        env['WOKWI_CLI_TOKEN'] = token_file.read_text().strip()
    if not env.get('WOKWI_CLI_TOKEN'):
        sys.exit('Defina WOKWI_CLI_TOKEN ou crie .tools/wokwi-token.txt.')
    OUT.mkdir(parents=True, exist_ok=True)
    firmware = ROOT / '.pio' / 'build' / 'demo' / 'firmware.bin'
    print(f'firmware demo SHA-256 {hashlib.sha256(firmware.read_bytes()).hexdigest().upper()}')
    results = []
    for name in names or SCENARIOS:
        timeout, check = SCENARIOS[name]
        log, fresh = OUT / f'{name}.serial.txt', OUT / f'{name}.serial.tmp'
        run = subprocess.run([str(CLI), str(PROJECT), '--scenario', str(HERE / f'{name}.yaml'),
                              '--timeout', str(timeout), '--serial-log-file', str(fresh), '--quiet'],
                             env=env, capture_output=True, text=True, encoding='utf-8', errors='replace')
        if run.returncode != 0:
            tail = (run.stderr or run.stdout).strip().splitlines()[-3:]
            results.append((name, 'CENARIO', False, f'wokwi-cli saiu com {run.returncode}: {" | ".join(tail)}'))
        # Uma falha antes da simulacao (cota, rede) deixa o log vazio; nao apaga o da rodada anterior.
        if fresh.exists() and fresh.stat().st_size > 0:
            fresh.replace(log)
        else:
            fresh.unlink(missing_ok=True)
            continue
        try:
            for ct, (ok, detail) in check(*parse(log.read_text(encoding='utf-8', errors='replace'))).items():
                results.append((name, ct, ok, detail))
        except (IndexError, KeyError, TypeError, ValueError) as error:
            results.append((name, 'LOG', False, f'log incompleto: {error!r}'))
    report(results)


def report(results):
    for name, ct, ok, detail in results:
        print(f"{'PASS' if ok else 'FALHA':5} {ct:13} {detail}  [{name}]")
    sys.exit(0 if all(ok for *_, ok, _ in results) else 1)


if __name__ == '__main__':
    main(sys.argv[1:])
