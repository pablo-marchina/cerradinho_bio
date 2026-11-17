import type { LucideIcon } from 'lucide-react';
import { ArrowUpRight } from 'lucide-react';
import type { Tone } from './StatusBadge';
export function KpiCard({label,value,description,icon:Icon,tone='neutral',active,onClick}:{label:string;value:number;description:string;icon:LucideIcon;tone?:Tone;active:boolean;onClick:()=>void}){return <button type="button" className={`card kpi ${active?'kpi-active':''}`} aria-pressed={active} onClick={onClick}><div className="kpi-top"><span>{label}</span><span className={`kpi-icon status-${tone}`}><Icon size={19} aria-hidden="true"/></span></div><div className="kpi-value">{String(value).padStart(2,'0')}</div><div className="kpi-bottom"><span>{description}</span><ArrowUpRight size={16} aria-hidden="true"/></div></button>;}
