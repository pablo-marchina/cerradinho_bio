import { useMemo, useState } from 'react';
import { Link } from 'react-router-dom';
import { Plus, X, ArrowRight, Siren, CalendarX, Hourglass, PhoneCall, BellRing, WifiOff, type LucideIcon } from 'lucide-react';
import { usePortfolio } from '@/portfolio/store';
import { MapView } from '@/portfolio/MapView';
import { DeadlineCalendar } from '@/portfolio/Calendar';
import { AssetThumb, Avatar, DueBar, Figure, MiniJourney, ProcessRail, StageLabel, StatusText, statusOf } from '@/portfolio/ui';
import {
  STAGES, flagsOf, isCritical, isNoSignal, isNotified, isOpen, isOutside, isOverdue, isUrgent, needsCall, nextAction, ownerOf, personaLabel, personaName, sortByAttention, stageCounts, stageIndex, supplierOf, type Flag,
} from '@/portfolio/logic';
import type { PortfolioAsset, Stage } from '@/portfolio/types';

type Pick = { kind: 'stage'; stage: Stage } | { kind: 'flag'; flag: Flag } | null;

const flagTitle: Partial<Record<Flag, string>> = {
  critico: 'Críticos',
  atrasado: 'Prazo vencido',
  urgente: 'Perto do prazo',
  ligar: 'Precisam de ligação',
  notificado: 'Oficina já cobrada',
  sem_sinal: 'Sem sinal da TAG',
};
const flagHelp: Partial<Record<Flag, string>> = {
  critico: 'Lacre rompido, atraso em ativo de criticidade alta ou prazo fiscal com 15 dias ou menos.',
  atrasado: 'Passaram da data prometida pela oficina.',
  urgente: 'A data prometida vence em até 7 dias ou o prazo fiscal em até 30.',
  ligar: 'Atrasados ou vencendo em 5 dias, sem cobrança registrada nos últimos 3 dias.',
  notificado: 'Você cobrou a oficina nos últimos 3 dias.',
  sem_sinal: 'A TAG não se comunica há mais de 8 dias.',
};

export function PcmHome() {
  const { state } = usePortfolio();
  const { now, suppliers } = state;
  const open = useMemo(() => state.assets.filter(isOpen), [state.assets]);
  const [pick, setPick] = useState<Pick>(null);
  const [q, setQ] = useState('');
  const count = (fn: (a: PortfolioAsset) => boolean) => open.filter(fn).length;

  const figures: { flag: Flag; value: number; tone: 'alert' | 'warn' | 'ok' | 'none'; icon: LucideIcon }[] = [
    { flag: 'critico', value: count(a => isCritical(a, now, suppliers)), tone: 'alert', icon: Siren },
    { flag: 'atrasado', value: count(a => isOverdue(a, now)), tone: 'alert', icon: CalendarX },
    { flag: 'urgente', value: count(a => isUrgent(a, now, suppliers)), tone: 'warn', icon: Hourglass },
    { flag: 'ligar', value: count(a => needsCall(a, now)), tone: 'warn', icon: PhoneCall },
    { flag: 'notificado', value: count(a => isOutside(a) && isNotified(a, now)), tone: 'ok', icon: BellRing },
    { flag: 'sem_sinal', value: count(a => isNoSignal(a, now)), tone: 'none', icon: WifiOff },
  ];

  const picked = useMemo(() => {
    if (!pick) return [];
    const xs = pick.kind === 'stage' ? open.filter(a => a.stage === pick.stage) : open.filter(a => flagsOf(a, now, suppliers).includes(pick.flag));
    return sortByAttention(xs, now, suppliers);
  }, [pick, open, now, suppliers]);

  const list = useMemo(() => {
    const t = q.trim().toLowerCase();
    const xs = t ? open.filter(a => [a.name, a.id, a.tag ?? '', a.om, a.remittance?.number ?? '', supplierOf(a, suppliers)?.name ?? ''].some(v => v.toLowerCase().includes(t))) : open;
    return sortByAttention(xs, now, suppliers);
  }, [open, q, now, suppliers]);
  const [limit, setLimit] = useState(10);
  const mine = sortByAttention(open.filter(a => ownerOf(a) === 'pcm'), now, suppliers);

  return (
    <div className="pp">
      <header className="pp-hero">
        <div>
          <p className="eyebrow">Painel do PCM</p>
          <h1>{open.length} ativos em manutenção externa</h1>
          <p className="lead">
            <span className="t-alert">{count(a => isCritical(a, now, suppliers))} críticos</span>, {count(a => isOverdue(a, now))} com prazo vencido e {count(a => needsCall(a, now))} esperando uma ligação sua.
          </p>
        </div>
        <Link className="btn btn-primary btn-lg" to="/ativos/novo"><Plus size={20} /> Novo envio</Link>
      </header>

      <section className="pp-section" aria-labelledby="proc">
        <div className="pp-head">
          <h2 id="proc">Onde está cada ativo no processo</h2>
          <p className="muted">Clique numa etapa para ver os ativos que estão nela e com quem estão.</p>
        </div>
        <ProcessRail counts={stageCounts(open)} active={pick?.kind === 'stage' ? pick.stage : null} onPick={s => setPick(s ? { kind: 'stage', stage: s } : null)} hide={['encerrado']} />
        <div className="figs" role="group" aria-label="Situação dos ativos">
          {figures.map(f => (
            <Figure key={f.flag} icon={f.icon} value={f.value} label={flagTitle[f.flag]!} tone={f.tone} active={pick?.kind === 'flag' && pick.flag === f.flag} onClick={() => setPick(pick?.kind === 'flag' && pick.flag === f.flag ? null : { kind: 'flag', flag: f.flag })} />
          ))}
        </div>
        {pick && <PickPanel pick={pick} assets={picked} onClose={() => setPick(null)} />}
      </section>

      <div className="pp-cols">
        <section className="pp-section" aria-labelledby="mapa">
          <div className="pp-head">
            <h2 id="mapa">Onde estão agora</h2>
            <p className="muted">Usina em Chapadão do Céu (GO), oficinas e ativos na estrada. Clique numa oficina para ver os detalhes.</p>
          </div>
          <MapView assets={open} />
          <p className="map-legend">
            <span><i className="lg-plant" /> Usina</span>
            <span><i className="lg-sup" /> Oficina, com nº em reparo</span>
            <span><i className="lg-truck" /> Ativo na estrada</span>
            <span><i className="lg-alert" /> Com alerta</span>
          </p>
        </section>

        <section className="pp-section" aria-labelledby="mesa">
          <div className="pp-head">
            <h2 id="mesa">Na sua mesa <span className="muted mono">{mine.length}</span></h2>
            <p className="muted">Ativos parados com você, do mais urgente.</p>
          </div>
          <ol className="desk">
            {mine.slice(0, 7).map(a => (
              <li key={a.id}>
                <Link to={`/ativos/${a.id}`}>
                  <AssetThumb asset={a} size={44} />
                  <span className="grow">
                    <b>{a.name}</b>
                    <span className="muted mono">{a.id} · {supplierOf(a, suppliers)?.name ?? 'oficina a definir'}</span>
                    <StatusText asset={a} />
                  </span>
                  <span className="desk-next">{nextAction(a, now).label} <ArrowRight size={16} /></span>
                </Link>
              </li>
            ))}
          </ol>
        </section>
      </div>

      <section className="pp-section" aria-labelledby="cal">
        <div className="pp-head">
          <h2 id="cal">Calendário de prazos</h2>
          <p className="muted">Datas prometidas pelas oficinas e limites fiscais da suspensão do ICMS.</p>
        </div>
        <DeadlineCalendar />
      </section>

      <section className="pp-section" aria-labelledby="todos">
        <div className="pp-head row-between">
          <h2 id="todos">Todos os ativos <span className="muted mono">{list.length}</span></h2>
          <div className="row-gap">
            <input className="input" value={q} onChange={e => setQ(e.target.value)} placeholder="Buscar ativo, TAG, OM, NF ou oficina" aria-label="Buscar ativo" />
            <Link to="/ativos" className="btn btn-secondary">Ver trajetórias</Link>
          </div>
        </div>
        <AssetRows assets={list.slice(0, limit)} />
        {list.length > limit && <button type="button" className="btn btn-secondary more" onClick={() => setLimit(limit + 20)}>Mostrar mais {Math.min(20, list.length - limit)}</button>}
      </section>
    </div>
  );
}

function PickPanel({ pick, assets, onClose }: { pick: NonNullable<Pick>; assets: PortfolioAsset[]; onClose: () => void }) {
  const stage = pick.kind === 'stage' ? STAGES[stageIndex(pick.stage)] : null;
  return (
    <div className="pick" role="region" aria-live="polite" aria-label="Ativos selecionados">
      <div className="pick-head">
        <div>
          <h3>{stage ? stage.label : flagTitle[pick.kind === 'flag' ? pick.flag : 'critico']} <span className="muted mono">{assets.length}</span></h3>
          <p className="muted">
            {stage ? (
              <>{stage.hint}. {stage.who !== 'sistema' && <>Fica na mesa de <b>{personaName[stage.who]}</b> ({personaLabel[stage.who]}).</>}</>
            ) : flagHelp[pick.kind === 'flag' ? pick.flag : 'critico']}
          </p>
        </div>
        <button type="button" className="icon-btn" onClick={onClose} aria-label="Fechar"><X size={20} /></button>
      </div>
      {assets.length ? <AssetRows assets={assets.slice(0, 12)} /> : <p className="muted">Nenhum ativo aqui agora.</p>}
      {assets.length > 12 && <p className="muted">E mais {assets.length - 12}. Use a busca em "Todos os ativos".</p>}
    </div>
  );
}

export function AssetRows({ assets, journey = false }: { assets: PortfolioAsset[]; journey?: boolean }) {
  const { state } = usePortfolio();
  return (
    <div className="rows-wrap">
      <table className="rows">
        <thead>
          <tr>
            <th>Ativo</th>
            <th>{journey ? 'Trajetória' : 'Etapa'}</th>
            <th>Com quem</th>
            <th>Oficina</th>
            <th>Prazo</th>
            <th>Situação</th>
          </tr>
        </thead>
        <tbody>
          {assets.map(a => {
            const s = supplierOf(a, state.suppliers);
            const o = ownerOf(a);
            return (
              <tr key={a.id} className={statusOf(a, state.now, state.suppliers).tone === 'alert' ? 'row-alert' : ''}>
                <td>
                  <Link to={`/ativos/${a.id}`} className="asset-cell">
                    <AssetThumb asset={a} size={44} />
                    <span><b>{a.name}</b><span className="muted mono">{a.id} · {a.tag ?? 'sem TAG'}</span></span>
                  </Link>
                </td>
                <td>{journey ? <><MiniJourney stage={a.stage} /><span className="muted block">{STAGES[stageIndex(a.stage)].label}</span></> : <StageLabel stage={a.stage} />}</td>
                <td>{o ? <span className="who"><Avatar p={o} size="sm" /> {personaName[o]}</span> : <span className="muted">—</span>}</td>
                <td>{s ? <>{s.name}<span className="muted block">{s.city}/{s.uf}</span></> : <span className="muted">A definir</span>}</td>
                <td><DueBar asset={a} now={state.now} /></td>
                <td><StatusText asset={a} /></td>
              </tr>
            );
          })}
        </tbody>
      </table>
    </div>
  );
}
