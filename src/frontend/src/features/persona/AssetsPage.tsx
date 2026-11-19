import { useMemo, useState } from 'react';
import { Link } from 'react-router-dom';
import { Plus } from 'lucide-react';
import { usePortfolio } from '@/portfolio/store';
import { Avatar } from '@/portfolio/ui';
import { STAGES, isOpen, ownerOf, personaName, sortByAttention, supplierOf } from '@/portfolio/logic';
import type { Persona, Stage } from '@/portfolio/types';
import { AssetRows } from './PcmHome';

/** Trajetórias: todos os ativos com a linha das 8 paradas e com quem cada um está. */
export function AssetsPage() {
  const { state } = usePortfolio();
  const [who, setWho] = useState<Persona | 'todos'>('todos');
  const [stage, setStage] = useState<Stage | ''>('');
  const [closed, setClosed] = useState(false);
  const [q, setQ] = useState('');
  const list = useMemo(() => {
    let xs = state.assets.filter(a => (closed ? true : isOpen(a)));
    if (who !== 'todos') xs = xs.filter(a => ownerOf(a) === who);
    if (stage) xs = xs.filter(a => a.stage === stage);
    const t = q.trim().toLowerCase();
    if (t) xs = xs.filter(a => [a.name, a.id, a.tag ?? '', a.om, a.remittance?.number ?? '', supplierOf(a, state.suppliers)?.name ?? ''].some(v => v.toLowerCase().includes(t)));
    return sortByAttention(xs, state.now, state.suppliers);
  }, [state, who, stage, closed, q]);
  const [limit, setLimit] = useState(25);

  return (
    <div className="pp">
      <header className="pp-hero">
        <div>
          <p className="eyebrow">Trajetórias</p>
          <h1>Acompanhe cada ativo do envio ao retorno</h1>
          <p className="lead">Cada linha mostra as 8 paradas do ativo, onde ele está e com quem. Clique para abrir a trajetória completa.</p>
        </div>
        <Link className="btn btn-primary btn-lg" to="/ativos/novo"><Plus size={20} /> Novo envio</Link>
      </header>
      <section className="pp-section">
        <div className="filters">
          <div className="seg-tabs" role="group" aria-label="Com quem">
            {(['todos', 'pcm', 'fiscal', 'logistica'] as const).map(p => (
              <button key={p} type="button" className={who === p ? 'on' : ''} onClick={() => setWho(p)} aria-pressed={who === p}>
                {p === 'todos' ? 'Todos' : <><Avatar p={p} size="sm" /> {personaName[p]}</>}
              </button>
            ))}
          </div>
          <select className="input" value={stage} onChange={e => setStage(e.target.value as Stage | '')} aria-label="Etapa">
            <option value="">Todas as etapas</option>
            {STAGES.map(s => <option key={s.id} value={s.id}>{s.label}</option>)}
          </select>
          <input className="input grow" value={q} onChange={e => setQ(e.target.value)} placeholder="Buscar ativo, TAG, OM, NF ou oficina" aria-label="Buscar" />
          <label className="check"><input type="checkbox" checked={closed} onChange={e => setClosed(e.target.checked)} /> Incluir encerrados</label>
        </div>
        <p className="muted">{list.length} ativos</p>
        <AssetRows assets={list.slice(0, limit)} journey />
        {list.length > limit && <button type="button" className="btn btn-secondary more" onClick={() => setLimit(limit + 25)}>Mostrar mais</button>}
      </section>
    </div>
  );
}
