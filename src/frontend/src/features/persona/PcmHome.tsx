import { CleanDesk } from '@/portfolio/Guara';
import { useMemo, useState } from 'react';
import { Link } from 'react-router-dom';
import { Plus, X, ArrowRight, ChevronRight, Workflow, Inbox, CalendarDays, Map as MapIcon, Siren, CalendarX, Hourglass, PhoneCall, BellRing, WifiOff, type LucideIcon } from 'lucide-react';
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

      <section className="pp-block" aria-labelledby="sit">
        <div className="block-head">
          <span className="block-icon"><Siren size={24} /></span>
          <div>
            <h2 id="sit">Situação dos ativos</h2>
            <p>Clique num quadro para ver quais ativos estão nessa situação.</p>
          </div>
        </div>
        <div className="sit-grid" role="group" aria-label="Situação dos ativos">
          {figures.map(f => {
            const on = pick?.kind === 'flag' && pick.flag === f.flag;
            const Icon = f.icon;
            return (
              <button key={f.flag} type="button" className={`sit ${on ? 'on' : ''} tone-${f.tone}`} aria-pressed={on} onClick={() => setPick(on ? null : { kind: 'flag', flag: f.flag })}>
                <span className="sit-top">
                  <span className={`fig-icon i-${f.tone}`}><Icon size={22} strokeWidth={2.2} aria-hidden="true" /></span>
                  <span className={`sit-value mono t-${f.tone}`}>{f.value}</span>
                </span>
                <span className="sit-label">{flagTitle[f.flag]}</span>
                <span className="sit-cta">{on ? 'Fechar lista' : 'Ver ativos'} <ChevronRight size={18} /></span>
              </button>
            );
          })}
        </div>
        {pick?.kind === 'flag' && <PickPanel pick={pick} assets={picked} onClose={() => setPick(null)} />}
      </section>

      <section className="pp-block" aria-labelledby="proc">
        <div className="block-head">
          <span className="block-icon"><Workflow size={24} /></span>
          <div>
            <h2 id="proc">Etapas do processo</h2>
            <p>Quantos ativos estão em cada etapa e com quem. Clique numa etapa para ver a lista.</p>
          </div>
        </div>
        <ProcessRail counts={stageCounts(open)} active={pick?.kind === 'stage' ? pick.stage : null} onPick={s => setPick(s ? { kind: 'stage', stage: s } : null)} hide={['encerrado']} />
        {pick?.kind === 'stage' && <PickPanel pick={pick} assets={picked} onClose={() => setPick(null)} />}
      </section>

      <section className="pp-block" aria-labelledby="mesa">
        <div className="block-head row-between">
          <div className="row-gap nowrap">
            <span className="block-icon"><Inbox size={24} /></span>
            <div>
              <h2 id="mesa">Na sua mesa <span className="muted mono">{mine.length}</span></h2>
              <p>Ativos parados com você, do mais urgente. Clique para abrir a ficha.</p>
            </div>
          </div>
          <div className="row-gap">
            <Link to="/prazos" className="btn btn-secondary"><CalendarDays size={18} /> Calendário de prazos</Link>
            <Link to="/onde-estao" className="btn btn-secondary"><MapIcon size={18} /> Onde estão</Link>
          </div>
        </div>
        {!mine.length && <CleanDesk who="pcm" />}
        <ol className="desk">
          {mine.slice(0, 8).map(a => (
            <li key={a.id}>
              <Link to={`/ativos/${a.id}`}>
                <AssetThumb asset={a} size={44} />
                <span className="grow">
                  <b>{a.name}</b>
                  <span className="muted mono">{a.id} · {supplierOf(a, suppliers)?.name ?? 'oficina a definir'}</span>
                </span>
                <StatusText asset={a} />
                <span className="desk-next">{nextAction(a, now).label} <ArrowRight size={16} /></span>
              </Link>
            </li>
          ))}
        </ol>
        {mine.length > 8 && <Link to="/ativos" className="btn btn-secondary more">Ver os outros {mine.length - 8} na aba Trajetórias</Link>}
      </section>
    </div>
  );
}

/** Onde estão: mapa da região da usina, oficinas e ativos na estrada. */
export function PcmMapa() {
  const { state } = usePortfolio();
  const open = state.assets.filter(isOpen);
  return (
    <div className="pp">
      <header className="pp-hero">
        <div>
          <p className="eyebrow">Onde estão</p>
          <h1>Mapa dos ativos fora da usina</h1>
          <p className="lead muted">Usina em Chapadão do Céu (GO), oficinas com quantos ativos estão em reparo e ativos na estrada. Clique numa oficina ou num caminhão.</p>
        </div>
      </header>
      <MapView assets={open} height={560} />
      <p className="map-legend">
        <span><i className="lg-plant" /> Usina</span>
        <span><i className="lg-sup" /> Oficina, com nº em reparo</span>
        <span><i className="lg-truck" /> Ativo na estrada</span>
        <span><i className="lg-alert" /> Com alerta</span>
      </p>
    </div>
  );
}

/** Calendário de prazos do PCM. */
export function PcmPrazos() {
  return (
    <div className="pp">
      <header className="pp-hero">
        <div>
          <p className="eyebrow">Calendário de prazos</p>
          <h1>O que vence e quando</h1>
          <p className="lead muted">Datas prometidas pelas oficinas e limites fiscais da suspensão do ICMS. Clique num dia para ver os ativos.</p>
        </div>
      </header>
      <DeadlineCalendar />
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
