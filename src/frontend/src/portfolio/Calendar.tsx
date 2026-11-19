import { useMemo, useState } from 'react';
import { Link } from 'react-router-dom';
import { ChevronLeft, ChevronRight } from 'lucide-react';
import { usePortfolio } from './store';
import { dateBr, isOpen, legalLimitDays, supplierOf } from './logic';
import type { PortfolioAsset } from './types';
import type { Tone } from './ui';

type Kind = 'retorno' | 'fiscal';
type Due = { asset: PortfolioAsset; kind: Kind; date: Date; tone: Tone };

const DAY = 86400000;
const key = (d: Date) => `${d.getFullYear()}-${d.getMonth()}-${d.getDate()}`;
const WEEK = ['dom', 'seg', 'ter', 'qua', 'qui', 'sex', 'sáb'];

/**
 * Calendário de prazos: data prometida pela oficina (retorno) e data limite da suspensão do ICMS (fiscal).
 * A cor segue o status do guia: folha no prazo, pequi a 7 dias ou menos, guará vencido.
 */
export function DeadlineCalendar({ kinds = ['retorno', 'fiscal'] }: { kinds?: Kind[] }) {
  const { state } = usePortfolio();
  const now = new Date(state.now);
  const [month, setMonth] = useState(new Date(now.getFullYear(), now.getMonth(), 1));
  const [picked, setPicked] = useState<string | null>(key(now));

  const dues = useMemo(() => {
    const out: Due[] = [];
    for (const a of state.assets.filter(isOpen)) {
      if (kinds.includes('retorno') && a.dueAt && (a.stage === 'ida' || a.stage === 'oficina')) {
        const d = new Date(a.dueAt);
        const left = Math.floor((d.getTime() - now.getTime()) / DAY);
        out.push({ asset: a, kind: 'retorno', date: d, tone: left < 0 ? 'alert' : left <= 7 ? 'warn' : 'ok' });
      }
      if (kinds.includes('fiscal') && a.remittance) {
        const d = new Date(Date.parse(a.remittance.issuedAt) + legalLimitDays(supplierOf(a, state.suppliers)) * DAY);
        const left = Math.floor((d.getTime() - now.getTime()) / DAY);
        out.push({ asset: a, kind: 'fiscal', date: d, tone: left < 0 ? 'alert' : left <= 30 ? 'alert' : left <= 90 ? 'warn' : 'ok' });
      }
    }
    return out;
  }, [state, kinds]);

  const byDay = useMemo(() => {
    const m = new Map<string, Due[]>();
    for (const d of dues) m.set(key(d.date), [...(m.get(key(d.date)) ?? []), d]);
    return m;
  }, [dues]);

  const first = new Date(month);
  const start = new Date(first.getTime() - first.getDay() * DAY);
  const cells = Array.from({ length: 42 }, (_, i) => new Date(start.getFullYear(), start.getMonth(), start.getDate() + i));
  const overdue = dues.filter(d => d.kind === 'retorno' && d.date < now).length;
  const pickedList = picked ? byDay.get(picked) ?? [] : [];
  const mName = month.toLocaleDateString('pt-BR', { month: 'long' });
  const title = `${mName[0].toUpperCase()}${mName.slice(1)} de ${month.getFullYear()}`;
  const pickedDate = picked ? cells.find(d => key(d) === picked) ?? now : null;

  return (
    <div className="cal">
      <div className="cal-head">
        <div className="row-gap">
          <button type="button" className="icon-btn" aria-label="Mês anterior" onClick={() => setMonth(new Date(month.getFullYear(), month.getMonth() - 1, 1))}><ChevronLeft size={20} /></button>
          <h3 className="cal-title">{title}</h3>
          <button type="button" className="icon-btn" aria-label="Próximo mês" onClick={() => setMonth(new Date(month.getFullYear(), month.getMonth() + 1, 1))}><ChevronRight size={20} /></button>
          <button type="button" className="btn btn-ghost" onClick={() => { setMonth(new Date(now.getFullYear(), now.getMonth(), 1)); setPicked(key(now)); }}>Hoje</button>
        </div>
        <span className="cal-legend">
          {kinds.includes('retorno') && <span><i className="lg-retorno" /> Retorno prometido pela oficina</span>}
          {kinds.includes('fiscal') && <span><i className="lg-fiscal" /> Limite fiscal (270 ou 360 dias)</span>}
          <span><i className="lg-t-ok" /> no prazo <i className="lg-t-warn" /> perto <i className="lg-t-alert" /> vencido</span>
        </span>
      </div>
      {overdue > 0 && kinds.includes('retorno') && <p className="t-alert cal-note">{overdue} retornos já passaram da data prometida e aparecem nos dias em que venceram.</p>}
      <div className="cal-grid" role="grid" aria-label={`Prazos de ${title}`}>
        {WEEK.map(w => <span key={w} className="cal-wd" role="columnheader">{w}</span>)}
        {cells.map(d => {
          const k = key(d);
          const list = byDay.get(k) ?? [];
          const out = d.getMonth() !== month.getMonth();
          const today = k === key(now);
          return (
            <button
              key={k}
              type="button"
              role="gridcell"
              className={`cal-day ${out ? 'out' : ''} ${today ? 'today' : ''} ${picked === k ? 'on' : ''}`}
              onClick={() => setPicked(k)}
              aria-label={`${dateBr(d.toISOString())}: ${list.length} prazos`}
            >
              <span className="cal-num mono">{d.getDate()}</span>
              <span className="cal-marks">
                {list.slice(0, 4).map((x, i) => <i key={i} className={`mk-${x.kind} t-${x.tone}`} />)}
                {list.length > 4 && <em>+{list.length - 4}</em>}
              </span>
            </button>
          );
        })}
      </div>
      <div className="cal-list">
        <h4>{pickedDate ? `Prazos em ${dateBr(pickedDate.toISOString())}` : 'Escolha um dia'}</h4>
        {!pickedList.length && <p className="muted">Nenhum prazo neste dia.</p>}
        <ul>
          {pickedList.map((x, i) => (
            <li key={i}>
              <Link to={`/ativos/${x.asset.id}`}>
                <i className={`mk-${x.kind} t-${x.tone}`} aria-hidden="true" />
                <span className="grow"><b>{x.asset.name}</b> <span className="muted mono">{x.asset.id}</span></span>
                <span className="muted">{x.kind === 'retorno' ? `Retorno · ${supplierOf(x.asset, state.suppliers)?.name ?? ''}` : 'Limite fiscal do ICMS'}</span>
              </Link>
            </li>
          ))}
        </ul>
      </div>
    </div>
  );
}
