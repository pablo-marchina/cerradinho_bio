import {useState} from 'react';
import {Package,Clock,ShieldAlert,WifiOff,ArrowRight,RefreshCw} from 'lucide-react';
import {Link} from 'react-router-dom';
import {PageHeader} from '@/components/layout/PageHeader';
import {KpiCard} from '@/components/domain/KpiCard';
import {PriorityTable} from '@/components/domain/PriorityTable';
import {EventTimeline} from '@/components/domain/EventTimeline';
import {DataState} from '@/components/domain/DataState';
import {Button,Card} from '@/components/ui/primitives';
import {filterAssets,formatDate} from '@/services/domain';
import {useSnapshot} from '@/services/useSnapshot';
import {isMqttMode,isPhysicalEsp32} from '@/services/runtime';
import type {Filter} from '@/services/types';

const filters=[{id:'active',label:'Em manutenção',description:'Ciclos em andamento',icon:Package,tone:'neutral'},{id:'overdue',label:'Em atraso',description:'Prazo combinado vencido',icon:Clock,tone:'warning'},{id:'seal',label:'Lacre violado',description:'Ativos com violação',icon:ShieldAlert,tone:'critical'},{id:'silence',label:'Sem atualização',description:'Intervalo esperado excedido',icon:WifiOff,tone:'warning'}] as const;

export function Dashboard(){
  const q=useSnapshot();
  const [filter,setFilter]=useState<Filter>('attention');
  if(!q.data)return <><PageHeader title="Visão geral" description="Acompanhamento de equipamentos em manutenção externa."/><DataState loading={q.isPending} error={q.error} retry={()=>void q.refetch()}/></>;
  const s=q.data,critical=filterAssets(s,'seal'),list=filterAssets(s,filter);
  const source=isMqttMode?(isPhysicalEsp32?'ESP32 físico':'dispositivo simulado via MQTT'):'cenário de demonstração';
  return <><PageHeader title="Visão geral" description="Identifique os equipamentos que precisam de atenção e acompanhe cada ciclo." action={<Button onClick={()=>void q.refetch()} disabled={q.isFetching}><RefreshCw size={16}/> Atualizar</Button>}/><p className="scenario-date">Atualização do painel: {formatDate(s.asOf)} · Fonte: {source}</p>{critical.length>0&&<div className="critical-strip"><ShieldAlert size={24} aria-hidden="true"/><div><strong>{critical.length} {critical.length===1?'equipamento com violação de lacre':'equipamentos com violação de lacre'}</strong><p>Verifique as ocorrências e registre o tratamento.</p></div><Link to="/alertas?tipo=seal" className="btn btn-secondary">Ver alertas <ArrowRight size={16}/></Link></div>}<div className="kpi-grid">{filters.map(item=><KpiCard key={item.id} label={item.label} value={filterAssets(s,item.id).length} description={item.description} icon={item.icon} tone={item.tone} active={filter===item.id} onClick={()=>setFilter(filter===item.id?'attention':item.id)}/>)}</div><div className="secondary-indicators"><button onClick={()=>setFilter(filter==='permanence'?'attention':'permanence')} aria-pressed={filter==='permanence'}><strong>{filterAssets(s,'permanence').length}</strong> acima do limiar de permanência</button><button onClick={()=>setFilter(filter==='fiscal'?'attention':'fiscal')} aria-pressed={filter==='fiscal'}><strong>{filterAssets(s,'fiscal').length}</strong> com baixa fiscal pendente</button></div><Card className="section-card"><div className="section-heading"><div><h2>{filter==='attention'?'Equipamentos que exigem atenção':'Equipamentos filtrados'}</h2><p>{list.length} resultado(s) · Prioridade operacional</p></div><div className="actions">{filter!=='attention'&&<Button variant="ghost" onClick={()=>setFilter('attention')}>Limpar filtro</Button>}<Link className="btn btn-secondary" to="/equipamentos">Todos os equipamentos <ArrowRight size={16}/></Link></div></div><PriorityTable assets={list} snapshot={s}/></Card><Card className="section-card"><div className="section-heading"><div><h2>Eventos recentes</h2><p>Mais recentes primeiro, pelo horário disponível.</p></div></div><EventTimeline events={s.events} compact/></Card></>;
}
