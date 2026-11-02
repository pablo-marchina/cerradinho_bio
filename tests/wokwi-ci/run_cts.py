"""Roda os cenarios wokwi-cli dos CTs do V2 e confere os logs da Serial.

Uso: python tests/wokwi-ci/run_cts.py [nome-do-cenario ...]
Token: WOKWI_CLI_TOKEN no ambiente ou em .tools/wokwi-token.txt (ignorado pelo Git).
Pre-requisito: firmware demo compilado (.pio/build/demo) e chips/gps.chip.wasm.
Logs e capturas do LCD ficam em build/wokwi-ct/.
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
PROJECT = ROOT / 'src' / 'firmware' / 'prototipo_wokwi'


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
                 and first['contexto_restaurado'] is False, f"primeiro evento {first['type']} dias_fora={first.get('dias_fora')} seal={first['seal']}"),
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
    # Dia original: o registro reenviado traz o dia e o uptime de quando ocorreu, anteriores ao reenvio.
    beats = [int(m) for line in lines[:flush or 0] for m in re.findall(r'\[vivo\] tempo=(\d+) s', line)]
    kept_day = (violation is not None and violation['dia_ocorrencia'] == violation['dias_fora']
                and beats and violation['uptime_ms'] // 1000 < beats[-1])
    return {
        'CT08': (stored is not None and 'VIOLACAO_LACRE' not in before and line_of(lines, '[FILA] 1 pendente') is not None,
                 f'guardada na linha {stored}; nenhum JSON de violacao antes da reconexao'),
        'CT09 (local)': ([e['type'] for e in after][:2] == ['VIOLACAO_LACRE', 'LACRE_FECHADO'] and kept_day
                         and violation['network_sim_available'] is False,
                         f"reenvio {[e['type'] for e in after][:2]}, dia_ocorrencia={violation and violation['dia_ocorrencia']}"),
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
    c, s = (closed or [{}])[0], (started or [{}])[0]
    return {
        'CT14': (len(closed) == 1 and c['ciclo_ativo'] is False and c['dias_fora'] == 0 and c['ciclos_encerrados'] == 1
                 and len(started) == 1 and s['ciclo_id'] == 2 and s['dias_fora'] == 0,
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
        'CT17 (V2)': (nav is not None and line_of(lines, 'tela=5 ', nav) is not None
                      and line_of(lines, 'avancar=1 voltar=1') is not None, 'navegacao circular e leitura bruta no heartbeat'),
        'CT35': (silent, f'desligado na linha {off}, religado com STATUS na linha {back}, sem saidas entre eles'),
    }


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
}


def main(names):
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
        log = OUT / f'{name}.serial.txt'
        run = subprocess.run([str(CLI), str(PROJECT), '--scenario', str(HERE / f'{name}.yaml'),
                              '--timeout', str(timeout), '--serial-log-file', str(log), '--quiet'],
                             env=env, capture_output=True, text=True, encoding='utf-8', errors='replace')
        if run.returncode != 0:
            tail = (run.stderr or run.stdout).strip().splitlines()[-3:]
            results.append((name, 'CENARIO', False, f'wokwi-cli saiu com {run.returncode}: {" | ".join(tail)}'))
        if not log.exists():
            continue
        try:
            for ct, (ok, detail) in check(*parse(log.read_text(encoding='utf-8', errors='replace'))).items():
                results.append((name, ct, ok, detail))
        except (IndexError, KeyError, TypeError, ValueError) as error:
            results.append((name, 'LOG', False, f'log incompleto: {error!r}'))
    for name, ct, ok, detail in results:
        print(f"{'PASS' if ok else 'FALHA':5} {ct:13} {detail}  [{name}]")
    sys.exit(0 if all(ok for *_, ok, _ in results) else 1)


if __name__ == '__main__':
    main(sys.argv[1:])
