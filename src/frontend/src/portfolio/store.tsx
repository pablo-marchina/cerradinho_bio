import { createContext, useContext, useEffect, useMemo, useReducer, useState, type ReactNode } from 'react';
import { createPortfolio, PLANT } from './seed';
import { cfopFor, supplierOf } from './logic';
import type { HistoryEntry, Invoice, Persona, PortfolioAsset, PortfolioState } from './types';

type Action =
  | { type: 'create'; asset: PortfolioAsset; requestNf: boolean }
  | { type: 'plan'; id: string; supplierId: string; quote: number; agreedDays: number }
  | { type: 'request_nf'; id: string }
  | { type: 'issue_nf'; id: string; invoice: Invoice }
  | { type: 'pickup'; id: string }
  | { type: 'advance'; id: string }
  | { type: 'follow_up'; id: string; note: string }
  | { type: 'return_nf'; id: string; invoice: Invoice }
  | { type: 'share'; id: string; names: string[] }
  | { type: 'read_notices'; to: Persona }
  | { type: 'reset' };

const KEY = 'guaratracking.portfolio.v1';
const nowIso = () => new Date().toISOString();

function log(a: PortfolioAsset, who: string, text: string, tone?: HistoryEntry['tone']): PortfolioAsset {
  return { ...a, history: [{ at: nowIso(), who, text, tone }, ...a.history] };
}

function notice(state: PortfolioState, to: Persona, a: PortfolioAsset, text: string): PortfolioState {
  return { ...state, notices: [{ id: `NT-${Date.now()}-${state.notices.length}`, to, at: nowIso(), assetId: a.id, text, read: false }, ...state.notices] };
}

function reducer(state: PortfolioState, action: Action): PortfolioState {
  if (action.type === 'reset') return createPortfolio();
  if (action.type === 'read_notices') return { ...state, notices: state.notices.map(n => (n.to === action.to ? { ...n, read: true } : n)) };
  if (action.type === 'create') {
    let next: PortfolioState = {
      ...state,
      assets: [action.asset, ...state.assets],
      freeTags: state.freeTags.filter(t => t !== action.asset.tag),
    };
    if (action.requestNf) next = notice(next, 'fiscal', action.asset, `PCM pediu a NF de remessa de ${action.asset.name} (${action.asset.id}).`);
    return next;
  }
  const current = state.assets.find(a => a.id === action.id);
  if (!current) return state;
  const supplier = supplierOf(current, state.suppliers);
  let a = current;
  let next = state;
  const now = nowIso();
  switch (action.type) {
    case 'plan':
      a = log({ ...a, supplierId: action.supplierId, quote: action.quote, agreedDays: action.agreedDays }, 'Geraldo (PCM)', 'Oficina, valor e prazo combinados registrados.');
      break;
    case 'request_nf':
      a = log({ ...a, stage: 'aguardando_nf', nfRequestedAt: now }, 'Geraldo (PCM)', 'NF de remessa solicitada ao Fiscal.');
      next = notice(next, 'fiscal', a, `PCM pediu a NF de remessa de ${a.name} (${a.id}).`);
      break;
    case 'issue_nf':
      a = log({ ...a, stage: 'aguardando_coleta', remittance: action.invoice }, 'Clarice (Fiscal)', `NF de remessa ${action.invoice.number} vinculada ao ativo (CFOP ${action.invoice.cfop}).`, 'success');
      next = notice(next, 'pcm', a, `NF de remessa de ${a.name} (${a.id}) emitida. Pode acionar a logística.`);
      break;
    case 'pickup':
      a = log({ ...a, pickupRequestedAt: now }, 'Geraldo (PCM)', 'Logística acionada para a coleta.');
      next = notice(next, 'logistica', a, `Coleta de ${a.name} (${a.id}) para ${supplier?.city ?? 'a oficina'}.`);
      break;
    case 'advance': {
      // Simula o que a TAG e a logística informariam em campo.
      if (a.stage === 'aguardando_coleta')
        a = log({ ...a, stage: 'ida', departedAt: now, pickupRequestedAt: a.pickupRequestedAt ?? now, position: { lat: PLANT.lat, lng: PLANT.lng, label: 'Saindo da usina', at: now }, lastSignalAt: now }, 'Logística', 'Coletado pela transportadora. Prazo correndo.');
      else if (a.stage === 'ida')
        a = log({ ...a, stage: 'oficina', arrivedSupplierAt: now, dueAt: new Date(Date.now() + (a.agreedDays ?? 30) * 86400000).toISOString(), position: supplier ? { lat: supplier.lat, lng: supplier.lng, label: `${supplier.city}/${supplier.uf}`, at: now } : a.position, lastSignalAt: now }, 'Sistema', `Chegada registrada em ${supplier?.city ?? 'oficina'}.`);
      else if (a.stage === 'oficina') a = log({ ...a, stage: 'retorno', returnStartedAt: now, lastSignalAt: now }, 'Sistema', 'Saiu da oficina a caminho da usina.');
      else if (a.stage === 'retorno') {
        a = log({ ...a, stage: 'na_usina', arrivedPlantAt: now, position: { lat: PLANT.lat, lng: PLANT.lng, label: PLANT.label, at: now }, lastSignalAt: now }, 'Sistema', 'Chegou na usina. Aguardando NF de retorno.', 'warning');
        next = notice(next, 'fiscal', a, `${a.name} (${a.id}) chegou na usina sem NF de retorno.`);
      }
      break;
    }
    case 'share':
      a = log({ ...a, watchers: Array.from(new Set([...(a.watchers ?? []), ...action.names])) }, 'Geraldo (PCM)', `Acompanhamento compartilhado com ${action.names.join(', ')}.`);
      break;
    case 'follow_up':
      a = log({ ...a, lastFollowUpAt: now }, 'Geraldo (PCM)', action.note || 'Oficina cobrada sobre o prazo.');
      break;
    case 'return_nf': {
      const freed = a.tag;
      a = log({ ...a, stage: 'encerrado', returnInvoice: action.invoice, closedAt: now, tag: null, seal: 'sem_lacre', battery: null, lastSignalAt: null }, 'Clarice (Fiscal)', `NF de retorno ${action.invoice.number} lançada. Ciclo encerrado${freed ? ` e ${freed} liberada` : ''}.`, 'success');
      next = { ...notice(next, 'pcm', a, `${a.name} (${a.id}) encerrado pelo Fiscal.`), freeTags: freed ? [freed, ...next.freeTags] : next.freeTags };
      break;
    }
  }
  return { ...next, now, assets: next.assets.map(x => (x.id === a.id ? a : x)) };
}

function load(): PortfolioState {
  try {
    const raw = localStorage.getItem(KEY);
    if (raw) {
      const parsed = JSON.parse(raw) as PortfolioState;
      return { ...parsed, now: nowIso() };
    }
  } catch {
    /* sem armazenamento: começa do zero */
  }
  return createPortfolio();
}

type Ctx = {
  state: PortfolioState;
  dispatch: (a: Action) => void;
  persona: Persona;
  setPersona: (p: Persona) => void;
};
const PortfolioContext = createContext<Ctx | null>(null);

export function PortfolioProvider({ children }: { children: ReactNode }) {
  const [state, dispatch] = useReducer(reducer, undefined, load);
  const [persona, setPersonaState] = useState<Persona>(() => {
    try {
      const p = localStorage.getItem(`${KEY}.persona`);
      return p === 'fiscal' || p === 'logistica' ? p : 'pcm';
    } catch {
      return 'pcm';
    }
  });
  useEffect(() => {
    try {
      localStorage.setItem(KEY, JSON.stringify(state));
    } catch {
      /* ignora */
    }
  }, [state]);
  const value = useMemo(
    () => ({
      state,
      dispatch,
      persona,
      setPersona: (p: Persona) => {
        setPersonaState(p);
        try {
          localStorage.setItem(`${KEY}.persona`, p);
        } catch {
          /* ignora */
        }
      },
    }),
    [state, persona],
  );
  return <PortfolioContext.Provider value={value}>{children}</PortfolioContext.Provider>;
}

export function usePortfolio() {
  const ctx = useContext(PortfolioContext);
  if (!ctx) throw new Error('PortfolioProvider ausente');
  return ctx;
}

export function makeInvoice(kind: 'remessa' | 'retorno', a: PortfolioAsset, state: PortfolioState, number: string, value: number, extra: Partial<Invoice> = {}): Invoice {
  return { number, series: '1', issuedAt: nowIso(), cfop: cfopFor(kind, supplierOf(a, state.suppliers)), value, ...extra };
}
