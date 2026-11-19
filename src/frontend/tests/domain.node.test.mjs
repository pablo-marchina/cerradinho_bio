import {test} from 'node:test';
import assert from 'node:assert/strict';
import {createSnapshot} from '../src/mocks/fixtures.ts';
import {filterAssets,daysOutside,overdue,prolonged,silent,fiscalPending,priority,age,lateDays} from '../src/services/domain.ts';
const s=createSnapshot();const find=(id)=>s.assets.find(a=>a.id===id);
test('Amostra contém seis ativos distintos e cinco em manutenção',()=>{assert.equal(s.assets.length,6);assert.equal(new Set(s.assets.map(a=>a.id)).size,6);assert.equal(filterAssets(s,'active').length,5);});
test('Violação tem prioridade operacional',()=>{assert.equal(filterAssets(s,'attention')[0].id,'AT-0042');assert.equal(priority(find('AT-0042'),s),0);});
test('Atraso e permanência são independentes',()=>{const a=find('AT-0060');assert.equal(overdue(a,s.asOf),false);assert.equal(prolonged(a,s.asOf),true);assert.equal(filterAssets(s,'overdue').length,1);assert.equal(filterAssets(s,'permanence').length,2);assert.equal(lateDays(find('AT-0017'),s.asOf),7);});
test('Limiar de permanência inclui o dia 150',()=>{const a={...find('AT-0060'),departedAt:'2026-06-15T13:00:00Z'};assert.equal(daysOutside(a,s.asOf),150);assert.ok(prolonged(a,s.asOf));});
test('Desconexão não equivale a silêncio semanal',()=>{assert.equal(find('AT-0055').connection,'disconnected');assert.equal(silent(find('AT-0055'),s.asOf),false);assert.equal(silent(find('AT-0031'),s.asOf),true);assert.equal(filterAssets(s,'silence').length,1);});
test('Retorno físico não encerra ciclo e congela dias fora',()=>{const a=find('AT-0008');assert.equal(fiscalPending(a),true);assert.equal(overdue(a,s.asOf),false);assert.equal(daysOutside(a,s.asOf),41);assert.equal(silent(a,s.asOf),false);});
test('Busca por TAG e NF e fornecedor',()=>{assert.equal(filterAssets(s,'all','gt-0042')[0].id,'AT-0042');assert.equal(filterAssets(s,'all','demo-0017')[0].id,'AT-0017');assert.equal(filterAssets(s,'all','Oficina Beta').length,1);assert.equal(filterAssets(s,'all','não existe').length,0);});
test('Idade da posição é independente de última comunicação',()=>{const a=find('AT-0042');assert.equal(age(a.position.at,s.asOf),'há 3 dias');assert.equal(age(a.lastContact,s.asOf),'há 8 min');assert.equal(age(null,s.asOf),'Sem informação');});
test('Resolver ocorrência não muda lacre físico nem evento histórico',()=>{const c=createSnapshot();c.alerts[0].status='resolved';assert.equal(c.assets[0].seal,'violated');assert.equal(filterAssets(c,'seal').length,1);assert.equal(c.events[0].title,'Abertura não autorizada do lacre');});


test('silêncio começa estritamente após dois intervalos, com precisão de milissegundo',()=>{
  const a={...find('AT-0055'),lastContact:'2026-11-01T13:00:00.000Z'};
  for(const intervalDays of [7,0.5]){
    a.intervalDays=intervalDays;
    const threshold=Date.parse(a.lastContact)+intervalDays*2*86400000;
    assert.equal(silent(a,new Date(threshold-1).toISOString()),false);
    assert.equal(silent(a,new Date(threshold).toISOString()),false);
    assert.equal(silent(a,new Date(threshold+1).toISOString()),true);
  }
});
test('sem primeiro contato, silêncio usa início do acompanhamento sem inventar comunicação',()=>{
  const a={...find('AT-0055'),lastContact:null,monitoringStartedAt:'2026-11-12T13:00:00.000Z'};
  const threshold=Date.parse(a.monitoringStartedAt)+2*a.intervalDays*86400000;
  assert.equal(silent(a,new Date(threshold-1).toISOString()),false);
  assert.equal(silent(a,new Date(threshold).toISOString()),false);
  assert.equal(silent(a,new Date(threshold+1).toISOString()),true);
  assert.equal(a.lastContact,null);
});
test('status retido sem horário não usa a saída como última comunicação',()=>{
  const a={...find('AT-0055'),lastContact:null,monitoringStartedAt:undefined,departedAt:'2026-01-01T00:00:00.000Z',closedAt:null,returnedAt:null};
  assert.equal(silent(a,'2026-11-17T00:00:00.000Z'),false);
});
test('proximidade de sete dias não cria prioridade operacional',()=>{
  const c=createSnapshot(),a=c.assets.find(x=>x.id==='AT-0055');
  a.dueAt=new Date(Date.parse(c.asOf)+6*86400000).toISOString();
  assert.equal(priority(a,c),7);
  assert.ok(!filterAssets(c,'attention').some(x=>x.id===a.id));
});
