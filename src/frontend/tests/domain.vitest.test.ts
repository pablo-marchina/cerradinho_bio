import { describe,expect,it } from 'vitest';
import {createSnapshot} from '../src/mocks/fixtures';
import {filterAssets,prolonged,overdue,silent,age,fiscalPending} from '../src/services/domain';
describe('Regras operacionais da demonstração',()=>{
 const s=createSnapshot();
 it('ordena violações antes dos demais ativos',()=>expect(filterAssets(s,'attention')[0].id).toBe('AT-0042'));
 it('filtra prazos e permanência de modo independente',()=>{const a=s.assets.find(x=>x.id==='AT-0060')!;expect(prolonged(a,s.asOf)).toBe(true);expect(overdue(a,s.asOf)).toBe(false);});
 it('não trata desconexão MQTT como silêncio semanal',()=>{const a=s.assets.find(x=>x.id==='AT-0055')!;expect(a.connection).toBe('disconnected');expect(silent(a,s.asOf)).toBe(false);});
 it('distingue a idade da posição e do contato',()=>{const a=s.assets[0];expect(age(a.position!.at,s.asOf)).toBe('há 3 dias');expect(age(a.lastContact,s.asOf)).toBe('há 8 min');});
 it('não encerra o ciclo no retorno físico',()=>expect(fiscalPending(s.assets.find(x=>x.id==='AT-0008')!)).toBe(true));
});
