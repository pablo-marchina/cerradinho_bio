#!/usr/bin/env python3
"""Audit tool: static/portable release inputs; does not certify external acceptance."""
import argparse
import hashlib
import json
import pathlib
import re
import sys
from zipfile import ZipFile

R = pathlib.Path(__file__).resolve().parents[1]

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--json',action='store_true')
    args=ap.parse_args()
    results=[]
    def test(n,ok,detail=''):
        results.append({'id':n,'passed':bool(ok),'detail':detail})

    doc=(R/'document/documentacao.md').read_text(encoding='utf8')
    code=(R/'src/firmware/main.cpp').read_text(encoding='utf8')
    test('persona fiscal corrigida', (R/'assets/personas/persona-02-clarice-revisada.png').exists() and
         'persona-02-clarice-revisada.png' in doc)
    test('RTM 29 RF',all(rf in doc[doc.index('### 3.3.4.'):doc.index('### 3.3.5.')] for rf in (f'RF{x:03d}' for x in range(1,30))))
    test('RTM 20 RN',all(rn in doc[doc.index('### 3.3.4.'):doc.index('### 3.3.5.')] for rn in (f'RN{x:02d}' for x in range(1,21))))
    test('aviso de integridade simulada', 'ACK_SIM' in code and 'TransportAck::parseHex8' in code and
         'savePending()' in code and 'ACK manual' in doc)
    test('CT18 e CT22 continuam reprovados',
         all(x in doc[doc.index('### 3.3.4.'):doc.index('### 3.3.5.')] for x in ('CT18', 'CT22','REPROVADO')))
    with ZipFile(R/'assets/negocios/BOM-GuaraTracking-Sprint1.xlsx') as z:
        test('BOM .xlsx integro', z.testzip() is None and '[Content_Types].xml' in z.namelist())
    for name in ('inventario-privacidade.md','protocolo-ack-simulado.md','FECHAMENTO-S1.md'):
        test(f'Anexo {name}',(R/'document/outros'/name).is_file())
    test('config do firmware demo e weekly',
         'env:demo' in (R/'platformio.ini').read_text() and 'env:weekly' in (R/'platformio.ini').read_text())
    test('Pipeline CI', (R/'.gitlab-ci.yml').is_file())
    # Rastreio: não confundir ausência de ambiente homologado com código falho.
    result={'base_commit':'a70d1d6','checks':results,'pass_count':sum(r['passed'] for r in results),
            'total':len(results),'unresolved_external':[
            'CT18/CT22: repetir no Wokwi/hardware com evidencias originais',
            'CT09: ACK de servidor real ou aceite formal de escopo',
            'Publicar Wokwi integrado e integrar versao na main',
            'Homologar LGPD com controlador/Juridico e confirmar canal/DPO',
            'Cotar 12 linhas da BOM sem preco, impostos e frete',
            'Confirmar equipe, escopo e aceite institucional do parceiro']}
    if args.json: print(json.dumps(result,ensure_ascii=False,indent=2))
    else:
        for x in results: print(('[PASS] ' if x['passed'] else '[FALHA] ')+x['id'])
        print(f"RESULTADO {result['pass_count']}/{result['total']} checagens; externas nao aferidas.")
    return 0 if all(x['passed'] for x in results) else 1
if __name__ == '__main__': sys.exit(main())
