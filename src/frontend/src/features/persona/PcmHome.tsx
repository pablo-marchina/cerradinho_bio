import { useMemo, useState } from 'react';
import { Link, useNavigate } from 'react-router-dom';
import { Siren, Clock, Hourglass, WifiOff, Phone, BellRing, Wrench, Truck, Undo2, Plus, FileText, Factory } from 'lucide-react';
import { usePortfolio } from '@/portfolio/store';
import { AssetThumb, DueBar, FlagChips, NextActionTag, RegionMap, StagePill, StageTrack, Tile } from '@/portfolio/ui';
import {
  flagsOf, isCritical, isNoSignal, isNotified, isOpen, isOutside, isOverdue, isUrgent, needsCall, ownerOf, sortByAttention, stageCounts, supplierOf, type Flag,
} from '@/portfolio/logic';
import type { PortfolioAsset, Stage } from '@/portfolio/types';

type TileFilter = Flag | 'oficina' | 'ida' | 'retorno' | null;

export function PcmHome() {
  const { state } = usePortfolio();
  const nav = useNavigate();
  const { now, suppliers } = state;
  const open = useMemo(() => state.assets.filter(isOpen), [state.assets]);
  const [stage, setStage] = useState<Stage | null>(null);
  const [tile, setTile] = useState<TileFilter>(null);
  const [q, setQ] = useState('');

  const count = (fn: (a: PortfolioAsset) => boolean) => open.filter(fn).length;
  const tiles: { id: TileFilter; label: string; value: number; icon: typeof Siren; tone: string; hint: string }[] = [
    { id: 'critico', label: 'Críticos', value: count(a => isCritical(a, now, suppliers)), icon: Siren, tone: 'danger', hint: 'Lacre violado, atraso em item crítico ou prazo legal no fim' },
    { id: 'atrasado', label: 'Acima do prazo', value: count(a => isOverdue(a, now)), icon: Clock, tone: 'danger', hint: 'Passaram da data combinada com a oficina' },
    { id: 'urgente', label: 'Urgentes', value: count(a => isUrgent(a, now, suppliers)), icon: Hourglass, tone: 'warning', hint: 'Vencem em até 7 dias ou prazo legal em 30' },
    { id: 'ligar', label: 'Precisam de ligação', value: count(a => needsCall(a, now)), icon: Phone, tone: 'warning', hint: 'Atrasados ou perto do prazo sem cobrança recente' },
    { id: 'notificado', label: 'Já cobrados', value: count(a => isOutside(a) && isNotified(a, now)), icon: BellRing, tone: 'success', hint: 'Oficina cobrada nos últimos 3 dias' },
    { id: 'sem_sinal', label: 'Sem sinal', value: count(a => isNoSignal(a, now)), icon: WifiOff, tone: 'neutral', hint: 'TAG sem comunicação há mais de 8 dias' },
  ];
  const where: { id: TileFilter; label: string; value: number; icon: typeof Siren }[] = [
    { id: 'ida', label: 'Indo para a oficina', value: count(a => a.stage === 'ida'), icon: Truck },
    { id: 'oficina', label: 'Na oficina', value: count(a => a.stage === 'oficina'), icon: Wrench },
    { id: 'retorno', label: 'Voltando', value: count(a => a.stage === 'retorno'), icon: Undo2 },
  ];

  const list = useMemo(() => {
    let xs = open;
    if (stage) xs = xs.filter(a => a.stage === stage);
    if (tile === 'ida' || tile === 'oficina' || tile === 'retorno') xs = xs.filter(a => a.stage === tile);
    else if (tile) xs = xs.filter(a => flagsOf(a, now, suppliers).includes(tile));
    if (q.trim()) {
      const t = q.trim().toLowerCase();
      xs = xs.filter(a => [a.name, a.id, a.tag ?? '', a.om, a.remittance?.number ?? '', supplierOf(a, suppliers)?.name ?? ''].some(v => v.toLowerCase().includes(t)));
    }
    return sortByAttention(xs, now, suppliers);
  }, [open, stage, tile, q, now, suppliers]);
  const [limit, setLimit] = useState(12);
  const myNotices = state.notices.filter(n => n.to === 'pcm' && !n.read).slice(0, 3);

  return (
    <div className="persona-page">
      <header className="persona-hero">
        <div>
          <p className="eyebrow">Visão do PCM · Geraldo</p>
          <h1>{open.length} ativos em manutenção externa</h1>
          <p className="muted">
            {count(a => isCritical(a, now, suppliers))} críticos, {count(a => isOverdue(a, now))} acima do prazo e {count(a => needsCall(a, now))} esperando uma ligação sua.
          </p>
        </div>
        <Link className="btn btn-primary" to="/ativos/novo">
          <Plus size={18} /> Novo envio
        </Link>
      </header>

      {myNotices.length > 0 && (
        <div className="notice-strip">
          {myNotices.map(n => (
            <Link key={n.id} to={`/ativos/${n.assetId}`} className="notice">
              <FileText size={16} aria-hidden="true" /> {n.text}
            </Link>
          ))}
        </div>
      )}

      <section className="card section-card" aria-labelledby="pipe">
        <div className="section-head">
          <h2 id="pipe">Onde está cada ativo no processo</h2>
          <span className="muted small">Clique numa etapa para filtrar a lista</span>
        </div>
        <StageTrack counts={stageCounts(open)} active={stage} onPick={setStage} hide={['encerrado']} />
      </section>

      <section className="tiles" aria-label="Situação dos ativos">
        {tiles.map(t => (
          <Tile key={t.id} icon={t.icon} label={t.label} value={t.value} hint={t.hint} tone={t.tone} active={tile === t.id} onClick={() => setTile(tile === t.id ? null : t.id)} />
        ))}
      </section>

      <div className="split">
        <section className="card section-card" aria-labelledby="mapa">
          <div className="section-head">
            <h2 id="mapa">Onde estão agora</h2>
            <span className="legend">
              <i className="lg-go" /> indo <i className="lg-back" /> voltando <i className="lg-sup" /> na oficina
            </span>
          </div>
          <RegionMap assets={open} suppliers={suppliers} onPick={id => nav(`/ativos/${id}`)} />
          <div className="where">
            {where.map(w => (
              <button key={w.id} type="button" className={`where-item ${tile === w.id ? 'on' : ''}`} onClick={() => setTile(tile === w.id ? null : w.id)}>
                <w.icon size={18} aria-hidden="true" />
                <b className="mono">{w.value}</b> {w.label}
              </button>
            ))}
            <span className="where-item static">
              <Factory size={18} aria-hidden="true" />
              <b className="mono">{count(a => a.stage === 'na_usina')}</b> na usina sem baixa
            </span>
          </div>
        </section>

        <section className="card section-card" aria-labelledby="fila">
          <div className="section-head">
            <h2 id="fila">Na sua mesa <span className="muted mono">({open.filter(a => ownerOf(a) === 'pcm').length})</span></h2>
            <span className="muted small">Ativos parados com você, do mais urgente</span>
          </div>
          <ol className="queue">
            {sortByAttention(open.filter(a => ownerOf(a) === 'pcm'), now, suppliers)
              .slice(0, 6)
              .map(a => (
                <li key={a.id}>
                  <Link to={`/ativos/${a.id}`}>
                    <AssetThumb asset={a} size={36} />
                    <span className="queue-main">
                      <b>{a.name}</b>
                      <span className="small muted mono">{a.id} · {supplierOf(a, suppliers)?.name ?? 'sem oficina'}</span>
                      <FlagChips asset={a} />
                    </span>
                    <NextActionTag asset={a} />
                  </Link>
                </li>
              ))}
          </ol>
        </section>
      </div>

      <section className="card section-card" aria-labelledby="todos">
        <div className="section-head">
          <h2 id="todos">
            Ativos {stage || tile ? <span className="muted">· filtrado</span> : null} <span className="muted mono">({list.length})</span>
          </h2>
          <div className="row-gap">
            <input className="input" value={q} onChange={e => setQ(e.target.value)} placeholder="Buscar ativo, TAG, OM, NF ou oficina" aria-label="Buscar na carteira" />
            {(stage || tile || q) && (
              <button type="button" className="btn btn-ghost" onClick={() => { setStage(null); setTile(null); setQ(''); }}>
                Limpar filtros
              </button>
            )}
          </div>
        </div>
        <AssetTable assets={list.slice(0, limit)} />
        {list.length > limit && (
          <button type="button" className="btn btn-secondary more" onClick={() => setLimit(limit + 20)}>
            Mostrar mais {Math.min(20, list.length - limit)}
          </button>
        )}
      </section>
    </div>
  );
}

export function AssetTable({ assets }: { assets: PortfolioAsset[] }) {
  const { state } = usePortfolio();
  if (!assets.length) return <p className="muted empty">Nenhum ativo com esse filtro.</p>;
  return (
    <div className="table-wrap">
      <table className="asset-table">
        <thead>
          <tr>
            <th>Ativo</th>
            <th>Etapa</th>
            <th>Oficina</th>
            <th>Prazo</th>
            <th>Situação</th>
            <th>Próxima ação</th>
          </tr>
        </thead>
        <tbody>
          {assets.map(a => {
            const s = supplierOf(a, state.suppliers);
            return (
              <tr key={a.id}>
                <td>
                  <Link to={`/ativos/${a.id}`} className="asset-cell">
                    <AssetThumb asset={a} size={36} />
                    <span>
                      <b>{a.name}</b>
                      <span className="small muted mono">{a.id} · {a.tag ?? 'sem TAG'}</span>
                    </span>
                  </Link>
                </td>
                <td><StagePill stage={a.stage} /></td>
                <td>{s ? <><span>{s.name}</span><br /><span className="small muted">{s.city}/{s.uf}</span></> : <span className="muted">A definir</span>}</td>
                <td><DueBar asset={a} now={state.now} /></td>
                <td><FlagChips asset={a} /></td>
                <td><NextActionTag asset={a} /></td>
              </tr>
            );
          })}
        </tbody>
      </table>
    </div>
  );
}
