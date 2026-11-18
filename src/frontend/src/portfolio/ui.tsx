import type { LucideIcon } from 'lucide-react';
import {
  ClipboardList, FileText, PackageCheck, Truck, Wrench, Undo2, Factory, CircleCheck, ShieldAlert, Clock, Siren, WifiOff, Phone, BellRing, Hourglass, Cog, CalendarClock,
} from 'lucide-react';
import { Link } from 'react-router-dom';
import { PLANT } from './seed';
import { STAGES, dateBr, deskCount, dueRemaining, flagLabel, flagsOf, isOverdue, nextAction, ownerOf, personaLabel, personaName, stageIndex, type Flag } from './logic';
import { usePortfolio } from './store';
import type { Persona, PortfolioAsset, Stage, Supplier } from './types';

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

export const flagIcon: Record<Flag, LucideIcon> = {
  critico: Siren,
  legal: CalendarClock,
  atrasado: Clock,
  urgente: Hourglass,
  sem_sinal: WifiOff,
  ligar: Phone,
  notificado: BellRing,
  parado: ShieldAlert,
};
export const flagTone: Record<Flag, string> = {
  critico: 'danger', legal: 'danger', atrasado: 'danger', urgente: 'warning', sem_sinal: 'neutral', ligar: 'warning', notificado: 'success', parado: 'warning',
};

export function PersonaSwitcher() {
  const { persona, setPersona, state } = usePortfolio();
  const items: { id: Persona; name: string; role: string }[] = [
    { id: 'pcm', name: 'Geraldo', role: 'PCM' },
    { id: 'fiscal', name: 'Clarice', role: 'Fiscal' },
    { id: 'logistica', name: 'Izidio', role: 'Logística' },
  ];
  return (
    <div className="persona-switch" role="group" aria-label="Visão por pessoa">
      {items.map(p => (
        <Link
          key={p.id}
          to={p.id === 'pcm' ? '/' : `/${p.id}`}
          onClick={() => setPersona(p.id)}
          className={`persona-pill ${persona === p.id ? 'on' : ''}`}
          aria-pressed={persona === p.id}
        >
          <span className={`persona-dot p-${p.id}`}>
            {p.name[0]}
            <i className="desk-badge" aria-label={`${deskCount(state.assets, p.id)} ativos na mesa`}>{deskCount(state.assets, p.id)}</i>
          </span>
          <span>
            <b>{p.role}</b>
            <small>{p.name}</small>
          </span>
        </Link>
      ))}
    </div>
  );
}

/** Esteira do processo: cada etapa com a contagem de ativos e quem age nela. */
export function StageTrack({ counts, active, onPick, hide = [] }: { counts: Record<Stage, number>; active?: Stage | null; onPick?: (s: Stage | null) => void; hide?: Stage[] }) {
  const stages = STAGES.filter(s => !hide.includes(s.id));
  const max = Math.max(1, ...stages.map(s => counts[s.id]));
  return (
    <ol className="stage-track">
      {stages.map((s, i) => {
        const Icon = stageIcon[s.id];
        const c = counts[s.id];
        return (
          <li key={s.id} className={`stage ${active === s.id ? 'on' : ''} ${c === 0 ? 'empty' : ''}`}>
            <button type="button" onClick={() => onPick?.(active === s.id ? null : s.id)} aria-pressed={active === s.id} title={s.hint}>
              <span className="stage-icon" style={{ ['--fill' as string]: `${Math.round((c / max) * 100)}%` }}>
                <Icon size={20} aria-hidden="true" />
              </span>
              <span className="stage-count mono">{c}</span>
              <span className="stage-label">{s.label}</span>
              <span className={`stage-who who-${s.who}`}>{s.who === 'sistema' ? 'Sistema' : personaLabel[s.who]}</span>
            </button>
            {i < stages.length - 1 && <span className="stage-arrow" aria-hidden="true" />}
          </li>
        );
      })}
    </ol>
  );
}

export function Tile({ icon: Icon, label, value, hint, tone = 'neutral', active, onClick }: { icon: LucideIcon; label: string; value: number | string; hint?: string; tone?: string; active?: boolean; onClick?: () => void }) {
  return (
    <button type="button" className={`tile tone-${tone} ${active ? 'on' : ''}`} onClick={onClick} aria-pressed={active}>
      <span className="tile-icon"><Icon size={18} aria-hidden="true" /></span>
      <span className="tile-value mono">{value}</span>
      <span className="tile-label">{label}</span>
      {hint && <span className="tile-hint">{hint}</span>}
    </button>
  );
}

export function FlagChips({ asset }: { asset: PortfolioAsset }) {
  const { state } = usePortfolio();
  if (asset.stage === 'encerrado') return null;
  const flags = flagsOf(asset, state.now, state.suppliers);
  if (!flags.length) return <span className="chip tone-success">Em dia</span>;
  return (
    <span className="chips">
      {flags.map(f => {
        const Icon = flagIcon[f];
        return (
          <span key={f} className={`chip tone-${flagTone[f]}`}>
            <Icon size={13} aria-hidden="true" /> {flagLabel[f]}
          </span>
        );
      })}
    </span>
  );
}

export function StagePill({ stage }: { stage: Stage }) {
  const Icon = stageIcon[stage];
  return (
    <span className={`stage-pill s-${stage}`}>
      <Icon size={14} aria-hidden="true" /> {STAGES[stageIndex(stage)].label}
    </span>
  );
}

export function AssetThumb({ asset, size = 44 }: { asset: PortfolioAsset; size?: number }) {
  if (asset.photo) return <img className="thumb" src={asset.photo} alt={`Foto de ${asset.name}`} width={size} height={size} />;
  return (
    <span className="thumb thumb-empty" style={{ width: size, height: size }} aria-hidden="true">
      <Cog size={size * 0.5} />
    </span>
  );
}

/** Barra de prazo: tempo de reparo consumido em relação ao combinado. */
export function DueBar({ asset, now }: { asset: PortfolioAsset; now: string }) {
  const r = dueRemaining(asset, now);
  if (r === null || !asset.agreedDays || !['ida', 'oficina'].includes(asset.stage)) return <span className="muted small">{asset.agreedDays ? `${asset.agreedDays} dias combinados` : 'Sem prazo'}</span>;
  const used = asset.agreedDays - r;
  const pct = Math.max(0, Math.min(100, (used / asset.agreedDays) * 100));
  const late = isOverdue(asset, now);
  return (
    <span className="due">
      <span className={`due-bar ${late ? 'late' : r <= 7 ? 'soon' : ''}`}>
        <span style={{ width: `${late ? 100 : pct}%` }} />
      </span>
      <span className={`small ${late ? 'text-danger' : ''}`}>{late ? `${-r} d de atraso` : r === 0 ? 'vence hoje' : `faltam ${r} d`}</span>
    </span>
  );
}

export function NextActionTag({ asset }: { asset: PortfolioAsset }) {
  const { state } = usePortfolio();
  const n = nextAction(asset, state.now);
  return (
    <span className={`next who-${n.persona}`}>
      <b>{personaLabel[n.persona]}</b> {n.label}
    </span>
  );
}

/** Mapa esquemático (SVG, sem tiles externos): usina, oficinas e ativos fora. */
export function RegionMap({ assets, suppliers, onPick, height = 300 }: { assets: PortfolioAsset[]; suppliers: Supplier[]; onPick?: (id: string) => void; height?: number }) {
  const pts = [...suppliers.map(s => [s.lat, s.lng]), [PLANT.lat, PLANT.lng]];
  const lats = pts.map(p => p[0]);
  const lngs = pts.map(p => p[1]);
  const [minLat, maxLat, minLng, maxLng] = [Math.min(...lats) - 0.9, Math.max(...lats) + 1.1, Math.min(...lngs) - 1.2, Math.max(...lngs) + 1.4];
  const W = 640;
  const H = height;
  const x = (lng: number) => ((lng - minLng) / (maxLng - minLng)) * W;
  const y = (lat: number) => ((maxLat - lat) / (maxLat - minLat)) * H;
  const bySupplier = new Map<string, number>();
  for (const a of assets) if (a.stage === 'oficina' && a.supplierId) bySupplier.set(a.supplierId, (bySupplier.get(a.supplierId) ?? 0) + 1);
  return (
    <svg className="region-map" viewBox={`0 0 ${W} ${H}`} role="img" aria-label="Mapa esquemático da usina, das oficinas e dos ativos em rota">
      <defs>
        <pattern id="grid" width="32" height="32" patternUnits="userSpaceOnUse">
          <path d="M 32 0 L 0 0 0 32" fill="none" className="map-gridline" />
        </pattern>
      </defs>
      <rect width={W} height={H} fill="url(#grid)" className="map-bg" />
      {suppliers.map(s => (
        <line key={`l-${s.id}`} x1={x(PLANT.lng)} y1={y(PLANT.lat)} x2={x(s.lng)} y2={y(s.lat)} className="map-route" />
      ))}
      {suppliers.map(s => {
        const c = bySupplier.get(s.id) ?? 0;
        return (
          <g key={s.id} transform={`translate(${x(s.lng)},${y(s.lat)})`}>
            <circle r={8 + Math.sqrt(c) * 4} className="map-supplier" />
            <text y={4} textAnchor="middle" className="map-count">{c}</text>
            <text y={-14 - Math.sqrt(c) * 4} textAnchor="middle" className="map-label">{s.city}/{s.uf}</text>
          </g>
        );
      })}
      {assets
        .filter(a => (a.stage === 'ida' || a.stage === 'retorno') && a.position)
        .map(a => (
          <g key={a.id} transform={`translate(${x(a.position!.lng)},${y(a.position!.lat)})`} className="map-truck" onClick={() => onPick?.(a.id)}>
            <title>{`${a.name} (${a.id}) · ${a.stage === 'ida' ? 'indo' : 'voltando'}`}</title>
            <circle r={6} className={a.stage === 'ida' ? 'map-go' : 'map-back'} />
          </g>
        ))}
      <g transform={`translate(${x(PLANT.lng)},${y(PLANT.lat)})`}>
        <rect x={-11} y={-11} width={22} height={22} rx={5} className="map-plant" />
        <text y={4} textAnchor="middle" className="map-plant-t">U</text>
        <text y={28} textAnchor="middle" className="map-label">Usina</text>
      </g>
    </svg>
  );
}

/** Com quem está: avatar e nome da pessoa dona da etapa atual. */
export function DeskOwner({ asset }: { asset: PortfolioAsset }) {
  const p = ownerOf(asset);
  if (!p) return <span className="desk-owner done">Ciclo encerrado</span>;
  return (
    <span className="desk-owner">
      <span className={`persona-dot p-${p}`}>{personaName[p][0]}</span>
      <span>Está com <b>{personaName[p]}</b> · {personaLabel[p]}</span>
    </span>
  );
}

/** Trajetória do ativo: todas as paradas, quem cuida de cada uma e onde ele está agora. */
export function Journey({ stage, dates, here }: { stage: Stage; dates?: (string | null)[]; here?: string }) {
  const idx = stageIndex(stage);
  return (
    <ol className="journey card" aria-label="Trajetória do ativo">
      {STAGES.map((s, i) => {
        const Icon = stageIcon[s.id];
        const st = i < idx ? 'done' : i === idx ? 'now' : 'todo';
        const who = s.who === 'sistema' ? null : s.who;
        return (
          <li key={s.id} className={`stop ${st}`} aria-current={st === 'now' ? 'step' : undefined}>
            {st === 'now' && <span className="you-are-here">{here ?? 'Agora'}</span>}
            <span className="stop-dot"><Icon size={18} aria-hidden="true" /></span>
            <span className="stop-label">{s.label}</span>
            {who ? (
              <span className="stop-who"><span className={`persona-dot mini p-${who}`}>{personaName[who][0]}</span>{personaName[who]}</span>
            ) : <span className="stop-who muted">Sistema</span>}
            <span className="small muted">{st === 'done' && dates?.[i] ? dateBr(dates[i]) : s.hint}</span>
          </li>
        );
      })}
    </ol>
  );
}
