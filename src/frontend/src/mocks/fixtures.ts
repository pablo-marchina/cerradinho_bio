import {init} from '../services/simulator.ts';
import type { Asset, Snapshot } from '../services/types.ts';
// Cenário sintético congelado: todas as datas, NFs, pessoas e fornecedores são fictícios.
const asOf='2026-11-12T13:00:00Z';
const base: Omit<Asset,'id'|'name'|'tag'|'supplier'|'nf'>={owner:'Responsável de demonstração',returnNf:null,departedAt:'2026-10-01T13:00:00Z',dueAt:'2026-11-30T13:00:00Z',returnedAt:null,closedAt:null,seal:'intact',battery:76,batteryFault:false,connection:'disconnected',lastContact:'2026-11-10T13:00:00Z',queue:0,queueReportedAt:'2026-11-10T13:00:00Z',position:null,intervalDays:7,permanenceLimit:150};
const make=(id:string,name:string,supplier:string,extra:Partial<Asset>={}):Asset=>({...base,id:`AT-${id}`,name,tag:`GT-${id}`,supplier,nf:`DEMO-${id}`, ...extra});
export function createSnapshot(): Snapshot {return init({asOf,assets:[
make('0042','Motor trifásico','Oficina Alfa',{seal:'violated',battery:62,connection:'connected',lastContact:'2026-11-12T12:52:00Z',queue:2,queueReportedAt:'2026-11-12T12:52:00Z',dueAt:'2026-11-18T13:00:00Z',position:{label:'Ponto A (simulado)',lat:-21.1,lng:-48.5,at:'2026-11-09T13:00:00Z'}}),
make('0017','Bomba centrífuga','Oficina Beta',{dueAt:'2026-11-05T13:00:00Z',battery:16,departedAt:'2026-06-01T13:00:00Z',position:{label:'Ponto B (simulado)',lat:-20.7,lng:-49.1,at:'2026-11-06T13:00:00Z'}}),
make('0031','Redutor industrial','Oficina Gama',{dueAt:'2026-11-16T13:00:00Z',lastContact:'2026-10-24T13:00:00Z',queue:5,queueReportedAt:'2026-10-24T13:00:00Z',battery:48,position:{label:'Ponto C (simulado)',lat:-22.2,lng:-47.8,at:'2026-10-24T13:00:00Z'}}),
make('0008','Válvula de controle','Oficina Delta',{returnedAt:'2026-11-11T13:00:00Z',seal:'authorized',battery:null,batteryFault:true,queue:null,queueReportedAt:null,connection:'unknown',lastContact:null}),
make('0055','Compressor','Oficina Épsilon',{dueAt:'2026-12-10T13:00:00Z',battery:90,position:{label:'Ponto D (simulado)',lat:-21.4,lng:-47.6,at:'2026-11-10T13:00:00Z'}}),
make('0060','Cilindro hidráulico','Oficina Zeta',{departedAt:'2026-05-30T13:00:00Z',dueAt:'2026-12-01T13:00:00Z',seal:'reapply',battery:30})],alerts:[
{id:'AL-1',assetId:'AT-0042',kind:'seal',at:'2026-11-12T12:52:00Z',status:'open'},
{id:'AL-2',assetId:'AT-0017',kind:'overdue',at:'2026-11-06T13:00:00Z',status:'open'},
{id:'AL-3',assetId:'AT-0017',kind:'battery',at:'2026-11-10T13:00:00Z',status:'open'},
{id:'AL-4',assetId:'AT-0031',kind:'silence',at:'2026-11-07T13:00:00Z',status:'open'},
{id:'AL-5',assetId:'AT-0008',kind:'fiscal',at:'2026-11-11T13:00:00Z',status:'open'},
{id:'AL-6',assetId:'AT-0060',kind:'permanence',at:'2026-10-27T13:00:00Z',status:'open'},
{id:'AL-7',assetId:'AT-0017',kind:'permanence',at:'2026-10-29T13:00:00Z',status:'open'}],events:[
{id:'EV-1',assetId:'AT-0042',title:'Abertura não autorizada do lacre',kind:'critical',occurredAt:'2026-11-12T12:52:00Z',receivedAt:'2026-11-12T12:52:05Z'},
{id:'EV-2',assetId:'AT-0008',title:'Retorno físico confirmado',kind:'success',occurredAt:'2026-11-11T13:00:00Z',receivedAt:'2026-11-11T13:00:00Z'},
{id:'EV-3',assetId:'AT-0017',title:'Bateria abaixo de 20%',kind:'warning',occurredAt:'2026-11-10T13:00:00Z',receivedAt:'2026-11-10T13:00:01Z'},
{id:'EV-4',assetId:'AT-0055',title:'Posição válida registrada',kind:'info',occurredAt:'2026-11-10T13:00:00Z',receivedAt:'2026-11-10T13:00:01Z'},
{id:'EV-5',assetId:'AT-0031',title:'Última comunicação recebida',kind:'info',occurredAt:'2026-10-24T13:00:00Z',receivedAt:'2026-10-24T13:00:01Z'}]});}
