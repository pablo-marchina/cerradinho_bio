import {test} from 'node:test';
import assert from 'node:assert/strict';
import {createSnapshot} from '../src/mocks/fixtures.ts';
import {startCommand,step,completeCommand,simulate,registerAsset,updateAlert,cycleId,DEFAULT_AUTHORIZATION_MINUTES} from '../src/services/simulator.ts';
import {filterAssets,fiscalPending,hasCycle,silent} from '../src/services/domain.ts';
const command=(s,id,type,payload={},scenario='normal')=>{const c=startCommand(s,{assetId:id,type,payload:{confirm:'true',...payload},scenario});step(s,c,'published');completeCommand(s,c);return c;};
const fields={tag:'GT-0100',nf:'DEMO-0100',supplier:'Oficina fictícia',owner:'Operador fictício',departedAt:'2026-11-12T03:00:00Z',dueAt:'2026-12-01T03:00:00Z',permanenceLimit:'150'};
test('comando correlacionado apresenta cadeia de estados e confirmação',()=>{const s=createSnapshot();simulate(s,'AT-0055','reconnect');const c=command(s,'AT-0055','authorize',{reason:'Teste'});assert.equal(c.status,'executed');assert.deepEqual(c.steps.map(x=>x.status),['created','published','received','executed']);assert.equal(c.tag,'GT-0055');assert.equal(s.assets.find(a=>a.id==='AT-0055').authorization,true);});
test('timeout, expiração e recusa não alteram lacre nem ciclo',()=>{for(const scenario of ['timeout','expired','refused']){const s=createSnapshot(),a=s.assets[0],before=structuredClone(a);const c=command(s,a.id,'authorize',{reason:'Teste'},scenario);assert.equal(c.status,scenario);assert.deepEqual(a,before);}});
test('confirmação obrigatória no simulador',()=>{const s=createSnapshot();simulate(s,'AT-0055','reconnect');assert.equal(command(s,'AT-0055','authorize',{reason:'x',confirm:'false'}).status,'refused');});
test('abertura autorizada não dispara violação',()=>{const s=createSnapshot();const a=s.assets.find(a=>a.id==='AT-0055');simulate(s,a.id,'reconnect');command(s,a.id,'authorize',{reason:'Reparo'});simulate(s,a.id,'open_seal');assert.equal(a.seal,'authorized');assert.ok(!s.alerts.some(x=>x.assetId===a.id&&x.kind==='seal'));});
test('autorização expirada não permite retirada',()=>{const s=createSnapshot(),a=s.assets.find(a=>a.id==='AT-0055');simulate(s,a.id,'reconnect');command(s,a.id,'authorize',{reason:'Reparo'});simulate(s,a.id,'expire_authorization');simulate(s,a.id,'open_seal');assert.equal(a.seal,'violated');assert.ok(s.alerts.some(x=>x.assetId===a.id&&x.kind==='seal'));});
test('histerese bateria 19→21→26→19',()=>{const s=createSnapshot(),id='AT-0055';for(const x of ['battery19','battery21','battery26','battery19'])simulate(s,id,x);assert.equal(s.events.filter(e=>e.assetId===id&&e.title==='BATERIA_BAIXA').length,2);});
test('falha de leitura não vira zero',()=>{const s=createSnapshot(),a=simulate(s,'AT-0055','battery_fault');assert.equal(a.battery,null);assert.equal(a.batteryFault,true);});
test('sem fix preserva última posição; recuperação registra nova',()=>{const s=createSnapshot(),a=s.assets[0],before=structuredClone(a.position);simulate(s,a.id,'no_fix');assert.deepEqual(a.position,before);assert.equal(a.gnss,'no_fix');simulate(s,a.id,'fix');assert.notEqual(a.position.at,before.at);});
test('no_fix→no_fix→fix→fix emite uma perda e uma recuperação com contexto',()=>{
  const s=createSnapshot(),a=s.assets[0],before=structuredClone(a.position),positions=a.positions.length;
  simulate(s,a.id,'no_fix');
  const firstAt=s.asOf;
  simulate(s,a.id,'no_fix');
  assert.deepEqual(a.position,before);
  assert.equal(a.positions.length,positions);
  assert.equal(a.gnssNoFixCount,2);
  simulate(s,a.id,'fix');
  const recoveryAt=s.asOf;
  simulate(s,a.id,'fix');
  const gnssEvents=s.events.filter(e=>e.assetId===a.id&&/^GNSS_(SEM_FIX|RECUPERADO)/.test(e.title));
  assert.equal(gnssEvents.length,2);
  assert.match(gnssEvents[0].title,/motivo:.*última posição preservada/);
  assert.match(gnssEvents[1].title,/motivo:.*duração: 2 s; coletas sem fix: 2/);
  assert.equal(Date.parse(recoveryAt)-Date.parse(firstAt),2000);
  assert.equal(a.gnssNoFixCount,undefined);
});
test('LWT e desconexão intencional diferentes; fila reportada e reconciliação',()=>{const s=createSnapshot(),a=s.assets[0];simulate(s,a.id,'offline');assert.equal(a.connectionReason,'lwt');simulate(s,a.id,'sleep');assert.equal(a.connectionReason,'sleep');simulate(s,a.id,'overflow');assert.equal(a.queue,8);assert.equal(a.overflow,3);simulate(s,a.id,'reconnect');assert.equal(a.queue,0);assert.equal(a.confirmed,8);});
test('cadastro duplicado e TAG ocupada são recusados',()=>{const s=createSnapshot();assert.throws(()=>registerAsset(s,{id:'AT-0042',name:'x',tag:'GT-9999'}));assert.throws(()=>registerAsset(s,{id:'AT-9999',name:'x',tag:'GT-0042'}));});
test('duas aberturas concorrentes preservam vínculo único',()=>{const s=createSnapshot(),a=registerAsset(s,{id:'AT-0100',name:'Ativo novo',tag:'GT-0100'});const first=startCommand(s,{assetId:a.id,type:'open',payload:fields,scenario:'normal'}),second=startCommand(s,{assetId:a.id,type:'open',payload:fields,scenario:'normal'});completeCommand(s,first);completeCommand(s,second);assert.equal(first.status,'executed');assert.equal(second.status,'refused');assert.equal(a.nf,fields.nf);});
test('TAG ativa não pode ser reaproveitada por outro ativo',()=>{const s=createSnapshot(),a=registerAsset(s,{id:'AT-0100',name:'x',tag:'GT-0100'});assert.equal(command(s,a.id,'open',{...fields,tag:'GT-0042'}).status,'refused');});
test('retorno físico e NF separados; fechamento exige lacre íntegro',()=>{const s=createSnapshot(),a=s.assets.find(x=>x.id==='AT-0008');assert.ok(fiscalPending(a));simulate(s,a.id,'reconnect');assert.equal(command(s,a.id,'close').status,'refused');command(s,a.id,'return_nf',{nf:'DEMO-RETORNO'});assert.equal(command(s,a.id,'close').status,'refused');simulate(s,a.id,'close_seal');command(s,a.id,'reapply',{reason:'Conferido'});assert.equal(command(s,a.id,'close').status,'executed');assert.ok(a.closedAt);assert.equal(fiscalPending(a),false);});
test('histórico e posição isolados ao reutilizar TAG em novo ciclo',()=>{const s=createSnapshot(),a=s.assets.find(x=>x.id==='AT-0008'),old=cycleId(a);simulate(s,a.id,'reconnect');command(s,a.id,'return_nf',{nf:'DEMO-RETORNO'});simulate(s,a.id,'close_seal');command(s,a.id,'reapply',{reason:'Conferido'});command(s,a.id,'close');assert.equal(command(s,a.id,'open',{...fields,tag:a.tag}).status,'executed');assert.notEqual(cycleId(a),old);assert.equal(a.position,null);assert.ok(s.archived.some(x=>x.cycleId===old&&x.asset.returnNf==='DEMO-RETORNO'));assert.ok(s.events.some(x=>x.cycleId===old));});
test('tratamento de alerta mantém trilha e estado físico',()=>{const s=createSnapshot(),a=s.alerts[0];assert.throws(()=>updateAlert(s,a.id,'resolved','pular'));updateAlert(s,a.id,'acknowledged','Ciente');updateAlert(s,a.id,'in_progress','Tratando');updateAlert(s,a.id,'resolved','Resolvido');assert.equal(a.actions.length,3);assert.equal(s.assets[0].seal,'violated');assert.ok(s.events.some(x=>x.id==='EV-1'));});
test('evento atrasado mantém horário de ocorrência anterior ao recebimento',()=>{const s=createSnapshot();simulate(s,'AT-0042','late_event');const e=s.events.at(-1);assert.ok(Date.parse(e.occurredAt)<Date.parse(e.receivedAt));});
test('avanço de relógio gera permanência e silêncio sem duplicar ocorrência aberta',()=>{const s=createSnapshot();simulate(s,'AT-0055','advance_14');assert.ok(filterAssets(s,'silence').some(x=>x.id==='AT-0055'));assert.equal(s.alerts.filter(x=>x.assetId==='AT-0055'&&x.kind==='silence').length,1);simulate(s,'AT-0055','advance_day');assert.equal(s.alerts.filter(x=>x.assetId==='AT-0055'&&x.kind==='silence').length,1);});

test('TAG desconectada não confirma autorização no fluxo normal',()=>{const s=createSnapshot();const c=command(s,'AT-0055','authorize',{reason:'Teste'});assert.equal(c.status,'timeout');assert.notEqual(s.assets.find(a=>a.id==='AT-0055').authorization,true);});


test('reaplicação exige fechamento após abertura autorizada ou violação',()=>{
  for(const authorized of [false,true]){
    const s=createSnapshot(),a=s.assets.find(a=>a.id==='AT-0055');
    simulate(s,a.id,'reconnect');
    if(authorized)command(s,a.id,'authorize',{reason:'Reparo'});
    simulate(s,a.id,'open_seal');
    const before=a.seal;
    assert.equal(command(s,a.id,'reapply',{reason:'Conferido'}).status,'refused');
    assert.equal(a.seal,before);
    simulate(s,a.id,'close_seal');
    assert.equal(command(s,a.id,'reapply',{reason:'Conferido'}).status,'executed');
    assert.equal(a.seal,'intact');
  }
});

test('silêncio resolvido só rearma depois de nova comunicação',()=>{
  const s=createSnapshot(),id='AT-0055';
  const alerts=()=>s.alerts.filter(a=>a.assetId===id&&a.kind==='silence');
  simulate(s,id,'advance_14');
  updateAlert(s,alerts()[0].id,'acknowledged','Ciente');
  updateAlert(s,alerts()[0].id,'resolved','Tratado');
  simulate(s,id,'advance_day');
  assert.equal(alerts().length,1);
  simulate(s,id,'reconnect');
  assert.equal(alerts().length,1);
  simulate(s,id,'advance_14');
  assert.deepEqual(alerts().map(a=>a.status),['resolved','open']);
});

test('telemetria recebida atualiza último contato e rearma silêncio resolvido',()=>{
  const s=createSnapshot(),id='AT-0055',a=s.assets.find(x=>x.id===id);
  const alerts=()=>s.alerts.filter(x=>x.assetId===id&&x.kind==='silence');
  simulate(s,id,'reconnect');
  simulate(s,id,'advance_14');
  assert.equal(alerts().length,1);
  updateAlert(s,alerts()[0].id,'acknowledged','Ciente');
  updateAlert(s,alerts()[0].id,'resolved','Tratado');
  const before=a.lastContact;
  simulate(s,id,'fix');
  assert.equal(a.lastContact,s.asOf);
  assert.notEqual(a.lastContact,before);
  simulate(s,id,'advance_14');
  assert.deepEqual(alerts().map(x=>x.status),['resolved','open']);
});

test('coletas GNSS e bateria recebidas atualizam contato, sem contato fictício no avanço do relógio',()=>{
  const s=createSnapshot(),id='AT-0055',a=s.assets.find(x=>x.id===id);
  for(const scenario of ['no_fix','battery19','battery_fault','overflow']){
    const before=a.lastContact;
    simulate(s,id,scenario);
    assert.equal(a.lastContact,s.asOf,scenario);
    assert.notEqual(a.lastContact,before,scenario);
  }
  const lastContact=a.lastContact;
  simulate(s,id,'advance_day');
  assert.equal(a.lastContact,lastContact);
  simulate(s,id,'offline');
  assert.equal(a.lastContact,lastContact);
});

test('reconexão não duplica alerta de silêncio ainda aberto',()=>{
  const s=createSnapshot(),id='AT-0055';
  simulate(s,id,'advance_14');
  simulate(s,id,'reconnect');
  simulate(s,id,'advance_14');
  assert.equal(s.alerts.filter(a=>a.assetId===id&&a.kind==='silence').length,1);
});

test('autorização padrão usa três dias e preserva validade configurável',()=>{
  assert.equal(DEFAULT_AUTHORIZATION_MINUTES,4320);
  for(const validityMinutes of [undefined,'10']){
    const s=createSnapshot(),id='AT-0055';
    simulate(s,id,'reconnect');
    const c=command(s,id,'authorize',{reason:'Reparo',...(validityMinutes?{validityMinutes}:{})});
    assert.equal(c.status,'executed');
    const appliedAt=c.steps.find(x=>x.status==='received').at;
    const a=s.assets.find(a=>a.id===id);
    assert.equal(Date.parse(a.authorizationExpiresAt)-Date.parse(appliedAt),Number(validityMinutes??4320)*60000);
  }
});

test('cadastro sem ciclo é distinto de ciclo encerrado e passa a ter ciclo na abertura',()=>{
  const s=createSnapshot(),a=registerAsset(s,{id:'AT-0100',name:'Ativo novo',tag:'GT-0100'});
  assert.equal(hasCycle(a),false);
  assert.ok(!filterAssets(s,'active').some(x=>x.id===a.id));
  assert.equal(command(s,a.id,'open',fields).status,'executed');
  assert.equal(hasCycle(a),true);
  assert.ok(!s.archived.some(x=>x.cycleId==='Sem ciclo'));
});
test('ciclo recém-aberto sem contato aguarda dois intervalos completos para alertar',()=>{
  const s=createSnapshot(),a=registerAsset(s,{id:'AT-0100',name:'Ativo novo',tag:'GT-0100'});
  assert.equal(command(s,a.id,'open',fields).status,'executed');
  const startedAt=a.monitoringStartedAt;
  assert.equal(a.lastContact,null);
  assert.equal(silent(a,s.asOf),false);
  assert.equal(s.alerts.filter(x=>x.assetId===a.id&&x.kind==='silence').length,0);
  s.asOf=new Date(Date.parse(startedAt)+2*a.intervalDays*86400000).toISOString();
  assert.equal(silent(a,s.asOf),false);
  simulate(s,a.id,'advance_day');
  assert.equal(s.alerts.filter(x=>x.assetId===a.id&&x.kind==='silence').length,1);
  assert.equal(a.lastContact,null);
});
