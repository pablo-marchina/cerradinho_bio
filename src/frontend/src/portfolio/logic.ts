import type { Criticality, Persona, PortfolioAsset, PortfolioState, Stage, Supplier } from './types.ts';

const DAY = 86400000;

export const STAGES: { id: Stage; label: string; short: string; who: Persona | 'sistema'; hint: string }[] = [
  { id: 'planejamento', label: 'Planejamento', short: 'Planejar', who: 'pcm', hint: 'PCM cota oficina, preço e prazo' },
  { id: 'aguardando_nf', label: 'Aguardando NF', short: 'NF', who: 'fiscal', hint: 'Fiscal emite a NF de remessa' },
  { id: 'aguardando_coleta', label: 'Aguardando coleta', short: 'Coleta', who: 'pcm', hint: 'PCM aciona a logística' },
  { id: 'ida', label: 'Indo para a oficina', short: 'Ida', who: 'sistema', hint: 'Transportadora a caminho' },
  { id: 'oficina', label: 'Em reparo', short: 'Oficina', who: 'pcm', hint: 'PCM acompanha e cobra' },
  { id: 'retorno', label: 'Voltando', short: 'Volta', who: 'sistema', hint: 'A caminho da usina' },
  { id: 'na_usina', label: 'Na usina sem baixa', short: 'Baixa', who: 'fiscal', hint: 'Fiscal lança a NF de retorno' },
  { id: 'encerrado', label: 'Encerrado', short: 'Fim', who: 'sistema', hint: 'Ciclo fechado e TAG liberada' },
];

export const stageIndex = (s: Stage) => STAGES.findIndex(x => x.id === s);
export const stageLabel = (s: Stage) => STAGES[stageIndex(s)].label;
export const OUTSIDE: Stage[] = ['ida', 'oficina', 'retorno'];
export const isOutside = (a: PortfolioAsset) => OUTSIDE.includes(a.stage);
export const isOpen = (a: PortfolioAsset) => a.stage !== 'encerrado';

export const criticalityLabel: Record<Criticality, string> = { alta: 'Alta', media: 'Média', baixa: 'Baixa' };
export const personaLabel: Record<Persona, string> = { pcm: 'PCM', fiscal: 'Fiscal', logistica: 'Logística' };
export const personaName: Record<Persona, string> = { pcm: 'Geraldo', fiscal: 'Clarice', logistica: 'Izidio' };

const diff = (from: string, to: string) => Math.floor((Date.parse(to) - Date.parse(from)) / DAY);

export const supplierOf = (a: PortfolioAsset, suppliers: Supplier[]) => suppliers.find(s => s.id === a.supplierId) ?? null;

/** Prazo legal da suspensão do ICMS: 270 dias dentro de GO e 360 fora (kick-off). */
export const legalLimitDays = (supplier: Supplier | null) => (supplier?.uf === 'GO' ? 270 : 360);

/** Dias fora contados a partir da emissão da NF de remessa (RF012). */
export const daysSinceNf = (a: PortfolioAsset, now: string) => (a.remittance ? Math.max(0, diff(a.remittance.issuedAt, a.returnInvoice?.issuedAt ?? now)) : null);

export function legalRemaining(a: PortfolioAsset, now: string, suppliers: Supplier[]) {
  const d = daysSinceNf(a, now);
  if (d === null || a.stage === 'encerrado') return null;
  return legalLimitDays(supplierOf(a, suppliers)) - d;
}

/** Dias até a data prometida pela oficina (negativo = atrasado). */
export const dueRemaining = (a: PortfolioAsset, now: string) => (a.dueAt ? diff(now, a.dueAt) : null);

export const isOverdue = (a: PortfolioAsset, now: string) => {
  const r = dueRemaining(a, now);
  return (a.stage === 'ida' || a.stage === 'oficina') && r !== null && r < 0;
};

export const isUrgent = (a: PortfolioAsset, now: string, suppliers: Supplier[]) => {
  const r = dueRemaining(a, now);
  const legal = legalRemaining(a, now, suppliers);
  return !isOverdue(a, now) && (((a.stage === 'ida' || a.stage === 'oficina') && r !== null && r <= 7) || (legal !== null && legal <= 30));
};

export const isNoSignal = (a: PortfolioAsset, now: string) =>
  isOutside(a) && !!a.tag && (!a.lastSignalAt || diff(a.lastSignalAt, now) > 8);

export const isCritical = (a: PortfolioAsset, now: string, suppliers: Supplier[]) => {
  if (!isOpen(a)) return false;
  const legal = legalRemaining(a, now, suppliers);
  return a.seal === 'violado' || (isOverdue(a, now) && a.criticality === 'alta') || (legal !== null && legal <= 15);
};

export const isNotified = (a: PortfolioAsset, now: string) => !!a.lastFollowUpAt && diff(a.lastFollowUpAt, now) <= 3;

/** Precisa de ligação/cobrança: atrasado ou perto do prazo e sem cobrança recente. */
export const needsCall = (a: PortfolioAsset, now: string) => {
  if (a.stage !== 'oficina' && a.stage !== 'ida') return false;
  const r = dueRemaining(a, now);
  return (isOverdue(a, now) || (r !== null && r <= 5)) && !isNotified(a, now);
};

/** Etapas que dependem de outra área e estão paradas há mais de 2 dias. */
export function handoffStuckDays(a: PortfolioAsset, now: string) {
  if (a.stage === 'aguardando_nf' && a.nfRequestedAt) return diff(a.nfRequestedAt, now);
  if (a.stage === 'aguardando_coleta' && a.remittance) return diff(a.pickupRequestedAt ?? a.remittance.issuedAt, now);
  if (a.stage === 'na_usina' && a.arrivedPlantAt) return diff(a.arrivedPlantAt, now);
  return 0;
}

export type Flag = 'critico' | 'legal' | 'atrasado' | 'urgente' | 'sem_sinal' | 'ligar' | 'notificado' | 'parado';

export function flagsOf(a: PortfolioAsset, now: string, suppliers: Supplier[]): Flag[] {
  const f: Flag[] = [];
  if (isCritical(a, now, suppliers)) f.push('critico');
  const legal = legalRemaining(a, now, suppliers);
  if (legal !== null && legal <= 30) f.push('legal');
  if (isOverdue(a, now)) f.push('atrasado');
  if (isUrgent(a, now, suppliers)) f.push('urgente');
  if (isNoSignal(a, now)) f.push('sem_sinal');
  if (needsCall(a, now)) f.push('ligar');
  else if (isNotified(a, now) && isOutside(a)) f.push('notificado');
  if (handoffStuckDays(a, now) > 2) f.push('parado');
  return f;
}

export const flagLabel: Record<Flag, string> = {
  critico: 'Crítico',
  legal: 'Prazo fiscal no fim',
  atrasado: 'Acima do prazo',
  urgente: 'Urgente',
  sem_sinal: 'Sem sinal',
  ligar: 'Cobrar oficina',
  notificado: 'Oficina cobrada',
  parado: 'Parado em outra área',
};

/** Ordem de atenção: crítico, atrasado, urgente, cobrar, sem sinal, parado; depois por prazo. */
export function attentionScore(a: PortfolioAsset, now: string, suppliers: Supplier[]) {
  const f = flagsOf(a, now, suppliers);
  const weights: Record<Flag, number> = { critico: 1000, legal: 300, atrasado: 400, urgente: 200, ligar: 120, sem_sinal: 100, parado: 80, notificado: 0 };
  const base = f.reduce((s, x) => s + weights[x], 0);
  const r = dueRemaining(a, now);
  return base + (r === null ? 0 : Math.max(0, 60 - r)) + (a.criticality === 'alta' ? 30 : a.criticality === 'media' ? 10 : 0);
}

export function sortByAttention(assets: PortfolioAsset[], now: string, suppliers: Supplier[]) {
  return [...assets].sort((x, y) => attentionScore(y, now, suppliers) - attentionScore(x, now, suppliers));
}

export type NextAction = { persona: Persona; label: string; key: 'plan' | 'request_nf' | 'issue_nf' | 'pickup' | 'follow_up' | 'confirm_arrival' | 'return_nf' | 'none' };

export function nextAction(a: PortfolioAsset, now: string): NextAction {
  switch (a.stage) {
    case 'planejamento':
      return a.supplierId && a.quote && a.agreedDays
        ? { persona: 'pcm', label: 'Pedir NF ao Fiscal', key: 'request_nf' }
        : { persona: 'pcm', label: 'Definir oficina, valor e prazo', key: 'plan' };
    case 'aguardando_nf':
      return { persona: 'fiscal', label: 'Emitir NF de remessa', key: 'issue_nf' };
    case 'aguardando_coleta':
      return { persona: 'pcm', label: a.pickupRequestedAt ? 'Coleta solicitada' : 'Acionar logística', key: a.pickupRequestedAt ? 'none' : 'pickup' };
    case 'ida':
    case 'oficina':
      return needsCall(a, now) ? { persona: 'pcm', label: 'Cobrar a oficina', key: 'follow_up' } : { persona: 'pcm', label: 'Registrar contato', key: 'follow_up' };
    case 'retorno':
      return { persona: 'pcm', label: 'Confirmar chegada', key: 'confirm_arrival' };
    case 'na_usina':
      return { persona: 'fiscal', label: 'Lançar NF de retorno', key: 'return_nf' };
    default:
      return { persona: 'pcm', label: 'Ciclo encerrado', key: 'none' };
  }
}

/** CFOP da remessa e do retorno para conserto: 5.915/5.916 dentro do estado, 6.915/6.916 fora. */
export const cfopFor = (kind: 'remessa' | 'retorno', supplier: Supplier | null, plantUf = 'GO') =>
  `${supplier && supplier.uf !== plantUf ? '6' : '5'}.${kind === 'remessa' ? '915' : '916'}`;

/** NF: letras maiúsculas, dígitos, hífen, ponto e barra, até 15 caracteres (mesma regra do firmware, RF033). */
export const normalizeNf = (value: string) => value.toUpperCase().replace(/[^A-Z0-9./-]/g, '').slice(0, 15);
export const validNf = (value: string) => /^[A-Z0-9./-]{1,15}$/.test(value);

/** Exibição anonimizada: só os 4 últimos caracteres. */
export const maskNf = (value: string | null | undefined) => (value ? `NF ••${value.slice(-4)}` : '—');

/** ICMS suspenso em risco, estimado em 12% do valor da NF (ilustrativo, não é cálculo fiscal). */
export const icmsAtRisk = (a: PortfolioAsset) => (a.remittance ? Math.round(a.remittance.value * 0.12) : 0);

export function stageCounts(assets: PortfolioAsset[]) {
  const counts = Object.fromEntries(STAGES.map(s => [s.id, 0])) as Record<Stage, number>;
  for (const a of assets) counts[a.stage]++;
  return counts;
}

export function supplierStats(state: PortfolioState) {
  const { now, suppliers } = state;
  return suppliers
    .map(s => {
      const mine = state.assets.filter(a => a.supplierId === s.id && a.remittance);
      const finished = mine.filter(a => a.arrivedPlantAt && a.arrivedSupplierAt);
      const repairDays = finished.map(a => diff(a.arrivedSupplierAt!, a.returnStartedAt ?? a.arrivedPlantAt!));
      const onTime = finished.filter(a => !a.dueAt || Date.parse(a.returnStartedAt ?? a.arrivedPlantAt!) <= Date.parse(a.dueAt)).length;
      return {
        supplier: s,
        open: mine.filter(a => isOutside(a)).length,
        overdue: mine.filter(a => isOverdue(a, now)).length,
        finished: finished.length,
        avgRepairDays: repairDays.length ? Math.round(repairDays.reduce((x, y) => x + y, 0) / repairDays.length) : null,
        onTimeRate: finished.length ? Math.round((onTime / finished.length) * 100) : null,
      };
    })
    .filter(x => x.open + x.finished > 0);
}

/** Tempo médio (dias) que os ativos ficaram ou estão em cada etapa. */
export function stageDwell(state: PortfolioState) {
  const pairs: [Stage, (a: PortfolioAsset) => string | null, (a: PortfolioAsset) => string | null][] = [
    ['aguardando_nf', a => a.nfRequestedAt, a => a.remittance?.issuedAt ?? null],
    ['aguardando_coleta', a => a.remittance?.issuedAt ?? null, a => a.departedAt],
    ['ida', a => a.departedAt, a => a.arrivedSupplierAt],
    ['oficina', a => a.arrivedSupplierAt, a => a.returnStartedAt],
    ['retorno', a => a.returnStartedAt, a => a.arrivedPlantAt],
    ['na_usina', a => a.arrivedPlantAt, a => a.returnInvoice?.issuedAt ?? null],
  ];
  return pairs.map(([stage, start, end]) => {
    const values = state.assets
      .map(a => {
        const s = start(a);
        if (!s) return null;
        const e = end(a) ?? (a.stage === stage ? state.now : null);
        return e ? Math.max(0, diff(s, e)) : null;
      })
      .filter((x): x is number => x !== null);
    return { stage, label: stageLabel(stage), avg: values.length ? Math.round((values.reduce((x, y) => x + y, 0) / values.length) * 10) / 10 : 0, now: state.assets.filter(a => a.stage === stage).length };
  });
}

export const money = (v: number | null | undefined) =>
  v === null || v === undefined ? '—' : v.toLocaleString('pt-BR', { style: 'currency', currency: 'BRL', maximumFractionDigits: 0 });
export const moneyShort = (v: number) => (v >= 1e6 ? `R$ ${(v / 1e6).toLocaleString('pt-BR', { maximumFractionDigits: 1 })} mi` : v >= 1e3 ? `R$ ${Math.round(v / 1e3)} mil` : money(v));
export const dateBr = (iso: string | null | undefined) => (iso ? new Date(iso).toLocaleDateString('pt-BR', { timeZone: 'America/Sao_Paulo' }) : '—');
export const ago = (iso: string | null | undefined, now: string) => {
  if (!iso) return 'nunca';
  const d = diff(iso, now);
  if (d <= 0) {
    const h = Math.floor((Date.parse(now) - Date.parse(iso)) / 3600000);
    return h <= 0 ? 'agora' : `há ${h} h`;
  }
  return d === 1 ? 'ontem' : `há ${d} dias`;
};
