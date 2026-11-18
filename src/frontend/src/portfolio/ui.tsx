import type { LucideIcon } from 'lucide-react';
import { ClipboardList, FileText, PackageCheck, Truck, Wrench, Undo2, Factory, CircleCheck, Cog } from 'lucide-react';
import { Link, useLocation } from 'react-router-dom';
import {
  STAGES, dateBr, deskCount, dueRemaining, flagsOf, isOverdue, legalRemaining, ownerOf, personaLabel, personaName, stageIndex,
} from './logic';
import { usePortfolio } from './store';
import type { Persona, PortfolioAsset, Stage } from './types';

// Regra de cor (Guia de Estilos v2, seções 03 e 09):
// Folha = dentro do prazo, Pequi = atenção, Guará = alerta. Só em dados.
// Pessoas e etapas não têm cor própria: usam semente, pedra e areia.

export const stageIcon: Record<Stage, LucideIcon> = {
  planejamento: ClipboardList,
  aguardando_nf: FileText,
  aguardando_coleta: PackageCheck,
  ida: Truck,
  oficina: Wrench,
  retorno: Undo2,
  na_usina: Factory,
  encerrado: CircleCheck,
};

export type Tone = 'ok' | 'warn' | 'alert' | 'none';

/** Situação principal do ativo, em uma palavra, com a cor de status do guia. */
export function statusOf(a: PortfolioAsset, now: string, suppliers: Parameters<typeof flagsOf>[2]): { tone: Tone; text: string } {
  if (a.stage === 'encerrado') return { tone: 'none', text: 'Encerrado' };
  if (a.seal === 'violado') return { tone: 'alert', text: 'Lacre rompido' };
  const legal = legalRemaining(a, now, suppliers);
  if (legal !== null && legal <= 15) return { tone: 'alert', text: 'Prazo fiscal no fim' };
  if (isOverdue(a, now)) return { tone: 'alert', text: `Prazo vencido há ${-(dueRemaining(a, now) ?? 0)} d` };
  const f = flagsOf(a, now, suppliers);
  if (f.includes('urgente')) return { tone: 'warn', text: 'Perto do prazo' };
  if (f.includes('parado')) return { tone: 'warn', text: 'Parado' };
  if (f.includes('sem_sinal')) return { tone: 'none', text: 'Sem sinal' };
  if (a.stage === 'na_usina') return { tone: 'warn', text: 'Aguardando baixa' };
  return { tone: 'ok', text: 'No prazo' };
}

export function StatusText({ asset }: { asset: PortfolioAsset }) {
  const { state } = usePortfolio();
  const s = statusOf(asset, state.now, state.suppliers);
  return <span className={`status-text t-${s.tone}`}><i aria-hidden="true" />{s.text}</span>;
}

export function Avatar({ p, size = 'md' }: { p: Persona; size?: 'sm' | 'md' }) {
  return <span className={`avatar-p ${size}`} aria-hidden="true">{personaName[p][0]}</span>;
}

export function PersonaSwitcher() {
  const { persona: stored, setPersona, state } = usePortfolio();
  const path = useLocation().pathname;
  const persona: Persona = path === '/' ? 'pcm' : path.startsWith('/fiscal') ? 'fiscal' : path.startsWith('/logistica') ? 'logistica' : stored;
  const items: Persona[] = ['pcm', 'fiscal', 'logistica'];
  return (
    <nav className="persona-switch" aria-label="Visão por pessoa">
      {items.map(p => (
        <Link key={p} to={p === 'pcm' ? '/' : `/${p}`} onClick={() => setPersona(p)} className={`persona-tab ${persona === p ? 'on' : ''}`} aria-current={persona === p ? 'page' : undefined}>
          <Avatar p={p} size="sm" />
          <span>{personaName[p]} <small>{personaLabel[p]}</small></span>
          <b className="desk-count mono" title="Ativos na mesa">{deskCount(state.assets, p)}</b>
        </Link>
      ))}
    </nav>
  );
}

/** Esteira do processo: círculos ligados por pontilhado (guia, "Um ciclo por nota fiscal"). */
export function ProcessRail({ counts, active, onPick, hide = [] }: { counts: Record<Stage, number>; active?: Stage | null; onPick?: (s: Stage | null) => void; hide?: Stage[] }) {
  const stages = STAGES.filter(s => !hide.includes(s.id));
  return (
    <ol className="rail" style={{ ['--n' as string]: stages.length }}>
      {stages.map(s => {
        const Icon = stageIcon[s.id];
        const on = active === s.id;
        return (
          <li key={s.id} className={`rail-stop ${on ? 'on' : ''} ${counts[s.id] === 0 ? 'zero' : ''}`}>
            <button type="button" onClick={() => onPick?.(on ? null : s.id)} aria-pressed={on}>
              <span className="rail-dot"><Icon size={22} strokeWidth={2.2} aria-hidden="true" /></span>
              <span className="rail-count mono">{counts[s.id]}</span>
              <span className="rail-label">{s.label}</span>
              <span className="rail-who">{s.who === 'sistema' ? 'Sistema' : personaName[s.who]}</span>
            </button>
          </li>
        );
      })}
    </ol>
  );
}

/** Número grande com rótulo, sem caixa: usado nas faixas de situação. */
export function Figure({ value, label, tone = 'none', active, onClick, icon: Icon }: { value: number | string; label: string; tone?: Tone; active?: boolean; onClick?: () => void; icon?: LucideIcon }) {
  const inner = (
    <>
      <span className="fig-top">
        {Icon && <span className={`fig-icon i-${tone}`}><Icon size={22} strokeWidth={2.2} aria-hidden="true" /></span>}
        <span className={`fig-value mono t-${tone} ${String(value).length > 5 ? 'long' : ''}`}>{value}</span>
      </span>
      <span className="fig-label">{label}</span>
    </>
  );
  return onClick ? (
    <button type="button" className={`fig ${active ? 'on' : ''}`} onClick={onClick} aria-pressed={active}>{inner}</button>
  ) : <div className="fig">{inner}</div>;
}

export function StageLabel({ stage }: { stage: Stage }) {
  const Icon = stageIcon[stage];
  return <span className="stage-label"><Icon size={18} aria-hidden="true" /> {STAGES[stageIndex(stage)].label}</span>;
}

export function AssetThumb({ asset, size = 44 }: { asset: PortfolioAsset; size?: number }) {
  if (asset.photo) return <img className="thumb" src={asset.photo} alt={`Foto de ${asset.name}`} style={{ width: size, height: size }} />;
  return (
    <span className="thumb thumb-empty" style={{ width: size, height: size }} aria-hidden="true">
      <Cog size={Math.round(size * 0.48)} />
    </span>
  );
}

/** Barra do prazo combinado: verde no prazo, pequi perto, guará vencido. */
export function DueBar({ asset, now }: { asset: PortfolioAsset; now: string }) {
  const r = dueRemaining(asset, now);
  if (r === null || !asset.agreedDays || !['ida', 'oficina'].includes(asset.stage)) return <span className="muted">{asset.agreedDays ? `${asset.agreedDays} dias combinados` : 'Sem prazo'}</span>;
  const used = asset.agreedDays - r;
  const pct = Math.max(4, Math.min(100, (used / asset.agreedDays) * 100));
  const late = isOverdue(asset, now);
  const tone = late ? 'alert' : r <= 7 ? 'warn' : 'ok';
  return (
    <span className="due">
      <span className={`due-bar b-${tone}`}><span style={{ width: `${late ? 100 : pct}%` }} /></span>
      <span className={`due-text t-${tone}`}>{late ? `${-r} d de atraso` : r === 0 ? 'vence hoje' : `faltam ${r} d`}</span>
    </span>
  );
}

/** Com quem o ativo está agora. */
export function DeskOwner({ asset }: { asset: PortfolioAsset }) {
  const p = ownerOf(asset);
  if (!p) return <span className="desk-owner">Ciclo encerrado</span>;
  return (
    <span className="desk-owner">
      <Avatar p={p} size="sm" />
      <span>Com <b>{personaName[p]}</b> · {personaLabel[p]}</span>
    </span>
  );
}

/** Trajetória completa do ativo, com o dono de cada parada. */
export function Journey({ stage, dates, here }: { stage: Stage; dates?: (string | null)[]; here?: string }) {
  const idx = stageIndex(stage);
  return (
    <ol className="journey" aria-label="Trajetória do ativo">
      {STAGES.map((s, i) => {
        const Icon = stageIcon[s.id];
        const st = i < idx ? 'done' : i === idx ? 'now' : 'todo';
        return (
          <li key={s.id} className={`stop ${st}`} aria-current={st === 'now' ? 'step' : undefined}>
            <span className="stop-flag">{st === 'now' ? here ?? 'Agora' : ''}</span>
            <span className="stop-dot"><Icon size={28} aria-hidden="true" /></span>
            <span className="stop-label">{s.label}</span>
            <span className="stop-who">{s.who === 'sistema' ? 'Sistema' : personaName[s.who]}</span>
            <span className="stop-date mono">{st === 'done' && dates?.[i] ? dateBr(dates[i]) : ''}</span>
          </li>
        );
      })}
    </ol>
  );
}

/** Trajetória compacta, para listas: oito pontos e o atual destacado. */
export function MiniJourney({ stage }: { stage: Stage }) {
  const idx = stageIndex(stage);
  return (
    <span className="mini-journey" aria-label={`Etapa ${idx + 1} de ${STAGES.length}: ${STAGES[idx].label}`}>
      {STAGES.map((s, i) => <i key={s.id} className={i < idx ? 'd' : i === idx ? 'n' : ''} title={s.label} />)}
    </span>
  );
}
