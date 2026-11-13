import { Activity } from 'lucide-react';
import { Link } from 'react-router-dom';
import type { Event } from '@/services/types';
import { formatDate } from '@/services/domain';
export function EventTimeline({events,compact=false}:{events:Event[];compact?:boolean}){const sorted=[...events].sort((a,b)=>Date.parse(b.occurredAt)-Date.parse(a.occurredAt));return sorted.length?<ol className="timeline">{sorted.map(e=><li key={e.id}><span className={`event-dot dot-${e.kind}`} aria-hidden="true"/><div><strong>{e.title}</strong><div className="small"><Link to={`/equipamentos/${e.assetId}`} className="asset-link">{e.assetId}</Link> · {e.tag??"TAG desconhecida"} · {formatDate(e.occurredAt)}</div>{!compact&&<div className="small muted">Recebido: {formatDate(e.receivedAt)} · {e.cycleId??"Ciclo não informado"} · Simulado</div>}</div></li>)}</ol>:<div className="empty"><Activity size={22}/>Nenhum evento registrado.</div>;}
