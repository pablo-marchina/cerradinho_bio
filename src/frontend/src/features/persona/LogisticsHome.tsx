import { Link } from 'react-router-dom';
import { Bar, BarChart, CartesianGrid, Legend, Line, ComposedChart, ResponsiveContainer, Tooltip, XAxis, YAxis, Cell } from 'recharts';
import { Truck, Timer, Target, Landmark, Hourglass, Download } from 'lucide-react';
import { usePortfolio } from '@/portfolio/store';
import { StageTrack, Tile } from '@/portfolio/ui';
import { handoffStuckDays, icmsAtRisk, isOpen, isOutside, moneyShort, personaLabel, STAGES, stageCounts, stageDwell, stageIndex, supplierOf, supplierStats } from '@/portfolio/logic';
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

export function LogisticsHome() {
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

  return (
    <div className="persona-page">
      <header className="persona-hero">
        <div>
          <p className="eyebrow">Visão da Logística · Izidio</p>
          <h1>Onde o processo está travando</h1>
          <p className="muted">
            O maior gargalo agora é <b>{worst.label.toLowerCase()}</b>: {worst.now} ativos parados ali, com média de {worst.avg} dias na etapa.
          </p>
        </div>
        <button type="button" className="btn btn-secondary" onClick={() => csv(state)}><Download size={16} /> Exportar ciclos (CSV para o BI)</button>
      </header>

      <section className="tiles" aria-label="Indicadores">
        <Tile icon={Truck} label="Ativos fora da usina" value={open.filter(isOutside).length} hint={`${open.length} ciclos abertos`} />
        <Tile icon={Timer} label="Reparo médio" value={`${avgRepair} d`} hint="Da chegada à saída da oficina" />
        <Tile icon={Target} label="Devolvidos no prazo" value={`${onTime}%`} tone={onTime < 70 ? 'warning' : 'success'} hint={`${finished} ciclos concluídos`} />
        <Tile icon={Landmark} label="ICMS suspenso" value={moneyShort(icms)} hint="Estimativa de 12%" />
        <Tile icon={Hourglass} label="Passagens paradas" value={stuck.length} tone={stuck.length ? 'warning' : 'success'} hint="Entre áreas há mais de 2 dias" />
      </section>

      <section className="card section-card">
        <div className="section-head"><h2>Esteira do processo</h2></div>
        <StageTrack counts={stageCounts(open)} hide={['encerrado']} />
      </section>

      <div className="split">
        <section className="card section-card" aria-labelledby="dwell">
          <div className="section-head"><h2 id="dwell">Tempo médio em cada etapa</h2><span className="muted small">Barras em dias; vermelho é o maior gargalo</span></div>
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

        <section className="card section-card" aria-labelledby="fluxo">
          <div className="section-head"><h2 id="fluxo">Saídas e retornos por mês</h2></div>
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

      <section className="card section-card" aria-labelledby="ofic">
        <div className="section-head"><h2 id="ofic">Desempenho das oficinas</h2><span className="muted small">Base para renovar ou trocar de fornecedor</span></div>
        <div className="table-wrap">
          <table className="asset-table">
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

      <section className="card section-card" aria-labelledby="stuck">
        <div className="section-head"><h2 id="stuck">Passagens paradas entre áreas</h2><span className="muted small">Quem precisa ser cobrado</span></div>
        {!stuck.length && <p className="muted empty">Nenhuma passagem parada.</p>}
        <ul className="stuck-list">
          {stuck.map(({ a, d }) => {
            const who = STAGES[stageIndex(a.stage)].who;
            return (
              <li key={a.id}>
                <Link to={`/ativos/${a.id}`}>
                  <span className={`persona-dot p-${who === 'sistema' ? 'logistica' : who}`}>{who === 'sistema' ? 'L' : personaLabel[who][0]}</span>
                  <span className="grow"><b>{a.name}</b><span className="small muted mono"> {a.id}</span><br /><span className="small">{STAGES[stageIndex(a.stage)].label} há {d} dias · cobrar {who === 'sistema' ? 'transportadora' : personaLabel[who]}</span></span>
                </Link>
              </li>
            );
          })}
        </ul>
      </section>
    </div>
  );
}
