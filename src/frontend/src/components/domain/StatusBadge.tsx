import { AlertTriangle, CheckCircle2, CircleHelp, Clock, ShieldAlert, Info } from 'lucide-react';
import type { ReactNode } from 'react';
import type { Seal } from '@/services/types';
export type Tone='critical'|'warning'|'success'|'neutral'|'unknown'|'info';
const icons={critical:ShieldAlert,warning:AlertTriangle,success:CheckCircle2,neutral:Clock,unknown:CircleHelp,info:Info};
export function StatusBadge({tone='neutral',children}:{tone?:Tone;children:ReactNode}){const Icon=icons[tone];return <span className={`status status-${tone}`}><Icon size={14} aria-hidden="true"/>{children}</span>;}
export function SealState({seal}:{seal:Seal}){const variants:Record<Seal,{label:string;tone:Tone}>={intact:{label:'Íntegro',tone:'success'},violated:{label:'Violado',tone:'critical'},authorized:{label:'Retirada autorizada',tone:'info'},reapply:{label:'Aguardando reaplicação',tone:'warning'},unknown:{label:'Indeterminado',tone:'unknown'}};const {label,tone}=variants[seal];return <StatusBadge tone={tone}>{label}</StatusBadge>;}
