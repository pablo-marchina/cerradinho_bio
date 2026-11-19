import { CleanDesk } from '@/portfolio/Guara';
import { useMemo, useState } from 'react';
import { Link } from 'react-router-dom';
import { Plus, X, ArrowRight, ChevronRight, Workflow, Inbox, CalendarDays, Map as MapIcon, Siren, CalendarX, WifiOff, ShieldAlert, Landmark } from 'lucide-react';
import { usePortfolio } from '@/portfolio/store';
import { MapView } from '@/portfolio/MapView';
import { DeadlineCalendar } from '@/portfolio/Calendar';
import { AssetThumb, Avatar, DueBar, MiniJourney, ProcessRail, StageLabel, StatusText, statusOf } from '@/portfolio/ui';
import {
  STAGES, isNoSignal, isOpen, isOverdue, legalLimitDays, legalRemaining, needsCall, nextAction, ownerOf, personaLabel, personaName, sortByAttention, stageCounts, stageIndex, supplierOf,
} from '@/portfolio/logic';
import type { PortfolioAsset, Stage } from '@/portfolio/types';

type RiskId = 'lacre' | 'atrasado' | 'fiscal' | 'sem_sinal';
type Pick = { kind: 'stage'; stage: Stage } | { kind: 'risk'; id: RiskId } | null;

const DAY = 86400000;

/** Os riscos que não podem passar despercebidos (kick-off: lacre imediato, prazo da oficina e prazo fiscal). */
function riskDefs(now: string, suppliers: Parameters<typeof legalRemaining>[2]) {
  return [
    { id: 'lacre' as RiskId, label: 'Lacre rompido', help: 'Abertura do lacre sem autorização registrada. Confira com a oficina.', icon: ShieldAlert, fn: (a: PortfolioAsset) => a.seal === 'violado' },
    { id: 'atrasado' as RiskId, label: 'Prazo vencido com a oficina', help: 'Passaram da data prometida pela oficina.', icon: CalendarX, fn: (a: PortfolioAsset) => isOverdue(a, now) },
    { id: 'fiscal' as RiskId, label: 'Prazo fiscal em até 30 dias', help: 'Perto do limite da suspensão do ICMS (270 dias em GO, 360 fora).', icon: Landmark, fn: (a: PortfolioAsset) => { const r = legalRemaining(a, now, suppliers); return r !== null && r <= 30; } },
    { id: 'sem_sinal' as RiskId, label: 'Sem sinal da TAG', help: 'A TAG não se comunica há mais de 8 dias. A posição pode estar desatualizada.', icon: WifiOff, fn: (a: PortfolioAsset) => isNoSignal(a, now) },
  ];
}

/** Ação da mesa: o lacre rompido pede conferência antes de qualquer cobrança. */
const deskAction = (a: PortfolioAsset, now: string) => (a.seal === 'violado' ? 'Ver lacre' : nextAction(a, now).label);

function greeting(now: string) {
  const h = Number(new Date(now).toLocaleString('pt-BR', { hour: '2-digit', hour12: false, timeZone: 'America/Sao_Paulo' }));
  return h < 12 ? 'Bom dia' : h < 18 ? 'Boa tarde' : 'Boa noite';
}

export function PcmHome() {
  const { state } = usePortfolio();
  const { now, suppliers } = state;
  const open = useMemo(() => state.assets.filter(isOpen), [state.assets]);
  const [pick, setPick] = useState<Pick>(null);
  const risks = riskDefs(now, suppliers);
  const mainRisks = risks.filter(r => r.id !== 'sem_sinal');
  const noSignal = risks.find(r => r.id === 'sem_sinal')!;

  const picked = useMemo(() => {
    if (!pick) return [];
    const xs = pick.kind === 'stage' ? open.filter(a => a.stage === pick.stage) : open.filter(risks.find(r => r.id === pick.id)!.fn);
    return sortByAttention(xs, now, suppliers);
  }, [pick, open, now, suppliers]);

  const mine = sortByAttention(open.filter(a => ownerOf(a) === 'pcm'), now, suppliers);
  const today = mine.filter(a => statusOf(a, now, suppliers).tone === 'alert' || statusOf(a, now, suppliers).tone === 'warn' || needsCall(a, now));
  const seals = mine.filter(a => a.seal === 'violado').length;

  // Agenda dos próximos 7 dias: data prometida pela oficina e limite fiscal.
  const week = useMemo(() => {
    const t0 = Date.parse(now);
    const items: { at: number; asset: PortfolioAsset; kind: 'oficina' | 'fiscal' }[] = [];
    for (const a of open) {
      if (a.dueAt && (a.stage === 'ida' || a.stage === 'oficina')) {
        const d = Date.parse(a.dueAt);
        if (d >= t0 && d <= t0 + 7 * DAY) items.push({ at: d, asset: a, kind: 'oficina' });
      }
      if (a.remittance) {
        const d = Date.parse(a.remittance.issuedAt) + legalLimitDays(supplierOf(a, suppliers)) * DAY;
        if (d >= t0 && d <= t0 + 7 * DAY) items.push({ at: d, asset: a, kind: 'fiscal' });
      }
    }
    return items.sort((x, y) => x.at - y.at);
  }, [open, now, suppliers]);

  return (
    <div className="pp">
      <header className="pp-hero">
        <div>
          <p className="eyebrow">Painel do PCM · {open.length} ativos em manutenção externa</p>
          <h1>{greeting(now)}, Geraldo.</h1>
          <p className="lead">
            {today.length ? (
              <><b>{today.length} {today.length === 1 ? 'ativo precisa' : 'ativos precisam'} de você hoje</b>{seals ? <>, <span className="t-alert">{seals} com lacre rompido</span></> : null}.</>
            ) : 'Nenhum ativo precisa de você hoje.'}
          </p>
        </div>
        <Link className="btn btn-primary btn-lg" to="/ativos/novo"><Plus size={20} /> Novo envio</Link>
      </header>

      <section className="pp-block" aria-labelledby="mesa">
        <div className="block-head">
          <span className="block-icon"><Inbox size={24} /></span>
          <div>
            <h2 id="mesa">Na sua mesa <span className="muted mono">{mine.length}</span></h2>
            <p>O que pede uma ação sua, do mais urgente. Cada linha já traz o próximo passo.</p>
          </div>
        </div>
        {!mine.length && <CleanDesk who="pcm" />}
        <ol className="desk">
          {mine.slice(0, 7).map(a => (
            <li key={a.id}>
              <Link to={`/ativos/${a.id}`}>
                <AssetThumb asset={a} size={48} />
                <span className="grow">
                  <b>{a.name}</b>
                  <span className="muted mono">{a.id} · {supplierOf(a, suppliers)?.name ?? 'oficina a definir'}</span>
                </span>
                <StatusText asset={a} />
                <span className="desk-next">{deskAction(a, now)} <ArrowRight size={16} /></span>
              </Link>
            </li>
          ))}
        </ol>
        {mine.length > 7 && <Link to="/ativos" className="btn btn-secondary more">Ver os outros {mine.length - 7} em Trajetórias</Link>}
      </section>

      <div className="pp-two">
        <section className="pp-block" aria-labelledby="riscos">
          <div className="block-head">
            <span className="block-icon"><Siren size={24} /></span>
            <div>
              <h2 id="riscos">Riscos</h2>
              <p>O que não pode passar despercebido. Clique para ver a lista.</p>
            </div>
          </div>
          <div className="risk-list" role="group" aria-label="Riscos">
            {mainRisks.map(r => {
              const n = open.filter(r.fn).length;
              const on = pick?.kind === 'risk' && pick.id === r.id;
              const tone = n ? 'alert' : 'ok';
              return (
                <button key={r.id} type="button" className={`risk ${on ? 'on' : ''} tone-${tone}`} aria-pressed={on} onClick={() => setPick(on ? null : { kind: 'risk', id: r.id })}>
                  <span className={`fig-icon i-${tone}`}><r.icon size={22} strokeWidth={2.2} aria-hidden="true" /></span>
                  <span className="risk-label">{r.label}</span>
                  <span className={`risk-value mono t-${tone}`}>{n}</span>
                  <ChevronRight size={20} aria-hidden="true" />
                </button>
              );
            })}
          </div>
        </section>

        <section className="pp-block" aria-labelledby="semana">
          <div className="block-head">
            <span className="block-icon"><CalendarDays size={24} /></span>
            <div>
              <h2 id="semana">Próximos 7 dias</h2>
              <p>O que vence esta semana.</p>
            </div>
          </div>
          {!week.length && <p className="muted">Nada vence nos próximos 7 dias.</p>}
          <ol className="week">
            {week.slice(0, 6).map((w, i) => (
              <li key={i}>
                <Link to={`/ativos/${w.asset.id}`}>
                  <span className="week-day"><b>{new Date(w.at).toLocaleDateString('pt-BR', { weekday: 'short', timeZone: 'America/Sao_Paulo' }).replace('.', '')}</b><span className="mono">{new Date(w.at).toLocaleDateString('pt-BR', { day: '2-digit', month: '2-digit', timeZone: 'America/Sao_Paulo' })}</span></span>
                  <span className="grow"><b>{w.asset.name}</b><span className="muted">{w.kind === 'oficina' ? `Prazo da ${supplierOf(w.asset, suppliers)?.name ?? 'oficina'}` : 'Limite fiscal do ICMS'}</span></span>
                </Link>
              </li>
            ))}
          </ol>
          <Link to="/prazos" className="btn btn-secondary more-left"><CalendarDays size={18} /> Calendário completo</Link>
        </section>
      </div>
      {pick?.kind === 'risk' && pick.id !== 'sem_sinal' && <PickPanel title={risks.find(r => r.id === pick.id)!.label} help={risks.find(r => r.id === pick.id)!.help} assets={picked} onClose={() => setPick(null)} />}

      <section className="pp-block compact" aria-labelledby="proc">
        <div className="block-head row-between">
          <div className="row-gap nowrap">
            <span className="block-icon"><Workflow size={24} /></span>
            <div>
              <h2 id="proc">Etapas do processo</h2>
              <p>Onde estão os {open.length} ativos. Clique numa etapa para ver a lista.</p>
            </div>
          </div>
          <div className="row-gap">
            {open.filter(noSignal.fn).length > 0 && (
              <button type="button" className="signal-note" onClick={() => setPick({ kind: 'risk', id: 'sem_sinal' })}>
                <WifiOff size={18} /> {open.filter(noSignal.fn).length} sem sinal da TAG
              </button>
            )}
            <Link to="/onde-estao" className="btn btn-secondary"><MapIcon size={18} /> Ver no mapa</Link>
          </div>
        </div>
        <ProcessRail counts={stageCounts(open)} active={pick?.kind === 'stage' ? pick.stage : null} onPick={st => setPick(st ? { kind: 'stage', stage: st } : null)} hide={['encerrado']} />
        {pick?.kind === 'risk' && pick.id === 'sem_sinal' && <PickPanel title={noSignal.label} help={noSignal.help} assets={picked} onClose={() => setPick(null)} />}
        {pick?.kind === 'stage' && (
          <PickPanel
            title={STAGES[stageIndex(pick.stage)].label}
            help={`${STAGES[stageIndex(pick.stage)].hint}.${STAGES[stageIndex(pick.stage)].who !== 'sistema' ? ` Fica na mesa de ${personaName[STAGES[stageIndex(pick.stage)].who as 'pcm']} (${personaLabel[STAGES[stageIndex(pick.stage)].who as 'pcm']}).` : ''}`}
            assets={picked}
            onClose={() => setPick(null)}
          />
        )}
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

function PickPanel({ title, help, assets, onClose }: { title: string; help: string; assets: PortfolioAsset[]; onClose: () => void }) {
  return (
    <div className="pick" role="region" aria-live="polite" aria-label={title}>
      <div className="pick-head">
        <div>
          <h3>{title} <span className="muted mono">{assets.length}</span></h3>
          <p className="muted">{help}</p>
        </div>
        <button type="button" className="icon-btn" onClick={onClose} aria-label="Fechar"><X size={20} /></button>
      </div>
      {assets.length ? <AssetRows assets={assets.slice(0, 12)} /> : <p className="muted">Nenhum ativo aqui agora.</p>}
      {assets.length > 12 && <p className="muted">E mais {assets.length - 12}. Veja todos em Trajetórias.</p>}
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
