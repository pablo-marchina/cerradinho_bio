import { useState } from 'react';
import { Link } from 'react-router-dom';
import { Bar, BarChart, CartesianGrid, Legend, Line, ComposedChart, ResponsiveContainer, Tooltip, XAxis, YAxis, Cell } from 'recharts';
import { PlayCircle, Truck, Timer, Target, Landmark, Hourglass, Download } from 'lucide-react';
import { usePortfolio } from '@/portfolio/store';
import { AssetThumb, Avatar, StageLabel, Figure } from '@/portfolio/ui';
import { handoffStuckDays, ownerOf, personaName, sortByAttention, icmsAtRisk, isOpen, isOutside, moneyShort, personaLabel, STAGES, stageDwell, stageIndex, supplierOf, supplierStats } from '@/portfolio/logic';
import type { PortfolioState } from '@/portfolio/types';

const C = { guara: '#c8232c', pequi: '#d89b2b', folha: '#3f7d4e', pedra: '#6b6460', areia: '#d6d1cc' };

function monthly(state: PortfolioState) {
  const now = new Date(state.now);
  const months = Array.from({ length: 6 }, (_, i) => new Date(now.getFullYear(), now.getMonth() - 5 + i, 1));
  return months.map(m => {
    const key = (iso: string | null) => (iso ? new Date(iso).getFullYear() === m.getFullYear() && new Date(iso).getMonth() === m.getMonth() : false);
    return {
      mes: m.toLocaleDateString('pt-BR', { month: 'short' }).replace('.', ''),
      saidas: state.assets.filter(a => key(a.departedAt)).length,
      retornos: state.assets.filter(a => key(a.arrivedPlantAt)).length,
    };
  });
}

function csv(state: PortfolioState) {
  const head = ['ativo', 'nome', 'om', 'tag', 'etapa', 'oficina', 'uf', 'nf_remessa', 'emissao_nf', 'saida', 'chegada_oficina', 'prazo_prometido', 'chegada_usina', 'nf_retorno', 'encerrado'];
  const rows = state.assets.map(a => {
    const s = supplierOf(a, state.suppliers);
    return [a.id, a.name, a.om, a.tag ?? '', a.stage, s?.name ?? '', s?.uf ?? '', a.remittance?.number ?? '', a.remittance?.issuedAt ?? '', a.departedAt ?? '', a.arrivedSupplierAt ?? '', a.dueAt ?? '', a.arrivedPlantAt ?? '', a.returnInvoice?.number ?? '', a.closedAt ?? ''];
  });
  const text = [head, ...rows].map(r => r.map(v => `"${String(v).replace(/"/g, '""')}"`).join(';')).join('\n');
  const url = URL.createObjectURL(new Blob(['﻿' + text], { type: 'text/csv;charset=utf-8' }));
  const link = document.createElement('a');
  link.href = url;
  link.download = 'ciclos-guaratracking-demonstracao.csv';
  link.click();
  URL.revokeObjectURL(url);
}

function useLogistics() {
  const { state } = usePortfolio();
  const open = state.assets.filter(isOpen);
  const stats = supplierStats(state).sort((a, b) => (b.avgRepairDays ?? 0) - (a.avgRepairDays ?? 0));
  const finished = stats.reduce((s, x) => s + x.finished, 0);
  const onTime = finished ? Math.round(stats.reduce((s, x) => s + ((x.onTimeRate ?? 0) * x.finished) / 100, 0) / finished * 100) : 0;
  const avgRepair = finished ? Math.round(stats.reduce((s, x) => s + (x.avgRepairDays ?? 0) * x.finished, 0) / finished) : 0;
  const dwell = stageDwell(state);
  const worst = [...dwell].sort((a, b) => b.avg * Math.max(1, b.now) - a.avg * Math.max(1, a.now))[0];
  const stuck = open.map(a => ({ a, d: handoffStuckDays(a, state.now) })).filter(x => x.d > 2).sort((x, y) => y.d - x.d);
  const icms = open.reduce((s, a) => s + icmsAtRisk(a), 0);
  return { state, open, stats, finished, onTime, avgRepair, dwell, worst, stuck, icms };
}

/** Gargalos: onde o processo trava e quem precisa ser cobrado. */
export function LogisticsHome() {
  const { open, dwell, worst, stuck } = useLogistics();
  const [all, setAll] = useState(false);
  return (
    <div className="pp">
      <header className="pp-hero">
        <div>
          <p className="eyebrow">Gargalos do processo</p>
          <h1>Onde o processo está travando</h1>
          <p className="lead muted">O maior gargalo agora é <b>{worst.label.toLowerCase()}</b>: {worst.now} ativos parados ali, com média de {String(worst.avg).replace('.', ',')} dias na etapa.</p>
        </div>
      </header>
      <section className="figs" aria-label="Indicadores">
        <Figure icon={Hourglass} label="Passagens paradas entre áreas" value={stuck.length} tone={stuck.length ? 'warn' : 'ok'} />
        <Figure icon={Timer} label="Média na etapa mais lenta" value={`${String(worst.avg).replace('.', ',')} d`} tone="alert" />
        <Figure icon={Truck} label="Ativos fora da usina" value={open.filter(isOutside).length} />
      </section>
      <section className="pp-section" aria-labelledby="dwell">
          <div className="pp-head row-between"><h2 id="dwell">Tempo médio em cada etapa</h2><span className="muted small">Barras em dias; vermelho é o maior gargalo</span></div>
          <div className="chart">
            <ResponsiveContainer width="100%" height={260}>
              <BarChart data={dwell} layout="vertical" margin={{ left: 24, right: 16 }}>
                <CartesianGrid strokeDasharray="3 3" horizontal={false} />
                <XAxis type="number" unit=" d" />
                <YAxis type="category" dataKey="label" width={150} />
                <Tooltip formatter={(v: number) => [`${v} dias`, 'Média']} />
                <Bar dataKey="avg" radius={[0, 6, 6, 0]}>
                  {dwell.map(d => <Cell key={d.stage} fill={d.stage === worst.stage ? C.guara : C.pedra} />)}
                </Bar>
              </BarChart>
            </ResponsiveContainer>
          </div>
        </section>
      <section className="pp-section" aria-labelledby="stuck">
        <div className="pp-head row-between"><h2 id="stuck">Passagens paradas entre áreas</h2><span className="muted small">Quem precisa ser cobrado</span></div>
        {!stuck.length && <p className="muted empty">Nenhuma passagem parada.</p>}
        <ul className="stuck-list">
          {(all ? stuck : stuck.slice(0, 6)).map(({ a, d }) => {
            const who = ownerOf(a) ?? 'pcm';
            return (
              <li key={a.id}>
                <Link to={`/ativos/${a.id}`}>
                  <Avatar p={who} />
                  <span className="grow"><b>{a.name}</b><span className="small muted mono"> {a.id}</span><br /><span className="small">{STAGES[stageIndex(a.stage)].label} há {d} dias · cobrar {personaName[who]} ({personaLabel[who]})</span></span>
                </Link>
              </li>
            );
          })}
        </ul>
        {stuck.length > 6 && <button type="button" className="btn btn-secondary more" onClick={() => setAll(!all)}>{all ? 'Mostrar menos' : `Ver as outras ${stuck.length - 6}`}</button>}
      </section>
    </div>
  );
}

/** Oficinas: desempenho de cada fornecedor e volume ao longo dos meses. */
export function LogisticsOficinas() {
  const { state, stats, finished, onTime, avgRepair, icms } = useLogistics();
  return (
    <div className="pp">
      <header className="pp-hero">
        <div>
          <p className="eyebrow">Oficinas</p>
          <h1>Desempenho das oficinas</h1>
          <p className="lead muted">Base para renovar ou trocar de fornecedor e para o relatório à diretoria.</p>
        </div>
        <button type="button" className="btn btn-secondary btn-lg" onClick={() => csv(state)}><Download size={18} /> Exportar ciclos (CSV para o BI)</button>
      </header>
      <section className="figs" aria-label="Indicadores">
        <Figure icon={Timer} label="Reparo médio" value={`${avgRepair} d`} />
        <Figure icon={Target} label={`Devolvidos no prazo (${finished} ciclos)`} value={`${onTime}%`} tone={onTime < 70 ? 'warn' : 'ok'} />
        <Figure icon={Landmark} label="ICMS suspenso (estimado)" value={moneyShort(icms)} />
      </section>
      <section className="pp-section" aria-labelledby="ofic">
        <div className="pp-head row-between"><h2 id="ofic">Desempenho das oficinas</h2><span className="muted small">Base para renovar ou trocar de fornecedor</span></div>
        <div className="rows-wrap">
          <table className="rows">
            <thead><tr><th>Oficina</th><th>Na oficina</th><th>Atrasados</th><th>Reparo médio</th><th>No prazo</th><th></th></tr></thead>
            <tbody>
              {stats.map(x => (
                <tr key={x.supplier.id}>
                  <td><b>{x.supplier.name}</b><br /><span className="small muted">{x.supplier.city}/{x.supplier.uf}</span></td>
                  <td className="mono">{x.open}</td>
                  <td className={`mono ${x.overdue ? 'text-danger' : ''}`}>{x.overdue}</td>
                  <td className="mono">{x.avgRepairDays === null ? '—' : `${x.avgRepairDays} d`}</td>
                  <td>
                    {x.onTimeRate === null ? '—' : (
                      <span className="due">
                        <span className={`due-bar ${x.onTimeRate < 60 ? 'late' : x.onTimeRate < 80 ? 'soon' : 'ok'}`}><span style={{ width: `${x.onTimeRate}%` }} /></span>
                        <span className="small mono">{x.onTimeRate}%</span>
                      </span>
                    )}
                  </td>
                  <td><a className="btn btn-ghost small" href={`mailto:${x.supplier.email}?subject=${encodeURIComponent('Prazos dos reparos em andamento')}`}>E-mail</a></td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </section>
      <section className="pp-section" aria-labelledby="fluxo">
          <div className="pp-head row-between"><h2 id="fluxo">Saídas e retornos por mês</h2></div>
          <div className="chart">
            <ResponsiveContainer width="100%" height={260}>
              <ComposedChart data={monthly(state)}>
                <CartesianGrid strokeDasharray="3 3" vertical={false} />
                <XAxis dataKey="mes" />
                <YAxis allowDecimals={false} />
                <Tooltip />
                <Legend />
                <Bar dataKey="saidas" name="Saídas" fill={C.pequi} radius={[6, 6, 0, 0]} />
                <Line dataKey="retornos" name="Retornos" stroke={C.folha} strokeWidth={3} dot />
              </ComposedChart>
            </ResponsiveContainer>
          </div>
        </section>
    </div>
  );
}

/** Transporte: coletas pedidas pelo PCM e ativos na estrada. */
export function LogisticsTransporte() {
  return (
    <div className="pp">
      <header className="pp-hero">
        <div>
          <p className="eyebrow">Transporte</p>
          <h1>Coletas e ativos na estrada</h1>
          <p className="lead muted">O que o PCM pediu para coletar e o que está indo ou voltando das oficinas.</p>
        </div>
      </header>
      <LogisticsDesk />
    </div>
  );
}

/** Mesa da Logística: coletas pedidas pelo PCM e ativos em trânsito. */
function LogisticsDesk() {
  const { state, dispatch } = usePortfolio();
  const mine = sortByAttention(state.assets.filter(a => ownerOf(a) === 'logistica'), state.now, state.suppliers);
  const label = { aguardando_coleta: 'Simular coleta', ida: 'Simular chegada na oficina', retorno: 'Simular chegada na usina' } as Record<string, string>;
  return (
    <section className="pp-section" aria-labelledby="mesa-log">
      <div className="pp-head row-between"><h2 id="mesa-log">Na sua mesa <span className="muted mono">({mine.length})</span></h2><span className="muted small">Coletas pedidas e ativos em trânsito</span></div>
      {!mine.length && <p className="muted empty">Nada com você agora.</p>}
      <ul className="desk-list">
        {mine.map(a => (
          <li key={a.id}>
            <AssetThumb asset={a} size={44} />
            <Link to={`/ativos/${a.id}`} className="grow"><b>{a.name}</b><span className="small muted mono">{a.id} · {supplierOf(a, state.suppliers)?.city ?? ''}</span></Link>
            <StageLabel stage={a.stage} />
            <button type="button" className="btn btn-secondary" onClick={() => dispatch({ type: 'advance', id: a.id })}><PlayCircle size={16} /> {label[a.stage]}</button>
          </li>
        ))}
      </ul>
    </section>
  );
}
