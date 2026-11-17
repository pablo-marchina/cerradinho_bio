export type Seal = 'intact' | 'violated' | 'authorized' | 'reapply' | 'unknown';
export type AlertKind = 'seal' | 'overdue' | 'permanence' | 'silence' | 'battery' | 'fiscal';
export type AlertStatus = 'open' | 'acknowledged' | 'in_progress' | 'resolved';
export type Asset = { cycleId?: string; description?: string; authorization?: boolean; authorizationExpiresAt?:string; batteryLatched?: boolean; gnss?: 'fix'|'no_fix'|'unknown'; gnssNoFixSince?: string; gnssNoFixCount?: number; gnssNoFixReason?: string; monitoringStartedAt?: string; connectionReason?: 'lwt'|'sleep'|'connected'|'unknown'; connectionAt?: string; overflow?: number; confirmed?: number; id: string; name: string; tag: string; supplier: string; owner: string; nf: string; returnNf: string | null; departedAt: string; dueAt: string; returnedAt: string | null; closedAt: string | null; seal: Seal; battery: number | null; batteryFault: boolean; connection: 'connected' | 'disconnected' | 'unknown'; lastContact: string | null; queue: number | null; queueReportedAt: string | null; positions?: {label:string;lat:number;lng:number;at:string}[]; position: { label: string; lat: number; lng: number; at: string } | null; intervalDays: number; permanenceLimit: number; };
export type Alert = { id: string; assetId: string; kind: AlertKind; at: string; status: AlertStatus; actions?: {status:AlertStatus;actor:string;reason:string;at:string}[]; };
export type Event = { id: string; assetId: string; title: string; occurredAt: string; receivedAt: string; cycleId?:string; tag?:string; kind: 'critical' | 'info' | 'success' | 'warning' };
export type Snapshot = { asOf: string; assets: Asset[]; alerts: Alert[]; events: Event[]; commands?: CommandRecord[]; archived?: ArchivedCycle[]; };
export type Filter = 'attention' | 'all' | 'active' | 'overdue' | 'seal' | 'silence' | 'permanence' | 'fiscal';

export type CommandType = 'open'|'remittance'|'authorize'|'return_nf'|'physical_return'|'reapply'|'close';
export type CommandStatus = 'created'|'published'|'received'|'executed'|'refused'|'timeout'|'expired';
export type CommandInput = {assetId:string;type:CommandType;payload:Record<string,string>;scenario:'normal'|'refused'|'timeout'|'expired'};
export type CommandRecord = CommandInput & {id:string;tag:string;cycleId:string;resultCycleId?:string;issuedAt:string;expiresAt:string;status:CommandStatus;reason:string;steps:{status:CommandStatus;at:string}[]};
export type ArchivedCycle = {cycleId:string;asset:Asset;events:Event[];alerts:Alert[]};
export type Scenario = 'violate'|'open_seal'|'close_seal'|'battery19'|'battery21'|'battery26'|'battery_fault'|'no_fix'|'fix'|'offline'|'sleep'|'reconnect'|'overflow'|'late_event'|'expire_authorization'|'advance_day'|'advance_14';
