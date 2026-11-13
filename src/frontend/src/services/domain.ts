import type { Asset, Alert, AlertKind, Filter, Snapshot } from './types.ts';
const DAY = 86_400_000;
export function hasCycle(a: Asset) { return a.cycleId !== 'Sem ciclo'; }
export function daysBetween(from: string, to: string) { return Math.max(0, Math.floor((Date.parse(to) - Date.parse(from))/DAY)); }
export function daysOutside(a: Asset, now: string) { return daysBetween(a.departedAt, a.returnedAt ?? now); }
export function overdue(a: Asset, now: string) { return !a.returnedAt && !a.closedAt && Date.parse(now) > Date.parse(a.dueAt); }
export function lateDays(a: Asset, now: string) { return overdue(a, now) ? Math.ceil((Date.parse(now)-Date.parse(a.dueAt))/DAY) : 0; }
export function prolonged(a: Asset, now: string) { return !a.returnedAt && !a.closedAt && daysOutside(a, now) >= a.permanenceLimit; }
export function silent(a: Asset, now: string) { return !a.returnedAt && !a.closedAt && (a.lastContact === null || Date.parse(now) - Date.parse(a.lastContact) > a.intervalDays * 2 * DAY); }
export function fiscalPending(a: Asset) { return !!a.returnedAt && !a.closedAt; }
export function unresolved(alerts: Alert[], id: string, kind: AlertKind) { return alerts.some(x=>x.assetId===id && x.kind===kind && x.status!=='resolved'); }
export function nearDue(a: Asset, now: string) { const left=Date.parse(a.dueAt)-Date.parse(now); return !a.returnedAt && !a.closedAt && left>=0 && left<=7*DAY; }
export function priority(a: Asset, s: Snapshot) { if(unresolved(s.alerts,a.id,'seal') || a.seal==='violated') return 0; if(overdue(a,s.asOf)) return 1; if(fiscalPending(a)) return 2; if(prolonged(a,s.asOf)) return 3; if(nearDue(a,s.asOf)) return 4; if(silent(a,s.asOf)) return 5; if(a.batteryFault || (a.battery!==null && a.battery<20)) return 6; return 7; }
export function filterAssets(s: Snapshot, filter: Filter, search='') { const q=search.trim().toLocaleLowerCase('pt-BR'); return s.assets.filter(a=>!q || [a.id,a.name,a.tag,a.supplier,a.nf].some(v=>v.toLocaleLowerCase('pt-BR').includes(q))).filter(a=>{switch(filter){case 'attention':return priority(a,s)<7;case 'active':return !a.closedAt&&!a.returnedAt;case 'overdue':return overdue(a,s.asOf);case 'seal':return unresolved(s.alerts,a.id,'seal') || a.seal==='violated';case 'silence':return silent(a,s.asOf);case 'permanence':return prolonged(a,s.asOf);case 'fiscal':return fiscalPending(a);default:return true;}}).sort((a,b)=>priority(a,s)-priority(b,s)||a.id.localeCompare(b.id)); }
export function formatDate(value:string|null) { return value ? new Intl.DateTimeFormat('pt-BR',{dateStyle:'short',timeStyle:'medium',timeZone:'America/Sao_Paulo'}).format(new Date(value)):'Não informado'; }
export function age(value:string|null,now:string) { if(!value)return 'Sem informação'; const diff=Math.max(0,Date.parse(now)-Date.parse(value)); const d=Math.floor(diff/DAY);const h=Math.floor(diff/3600000); return d?`há ${d} ${d===1?'dia':'dias'}`:h?`há ${h} h`:`há ${Math.floor(diff/60000)} min`; }
export const alertLabels:Record<AlertKind,string>={seal:'Violação de lacre',overdue:'Prazo vencido',permanence:'Permanência prolongada',silence:'Sem comunicação esperada',battery:'Bateria baixa',fiscal:'Retorno fiscal pendente'};
