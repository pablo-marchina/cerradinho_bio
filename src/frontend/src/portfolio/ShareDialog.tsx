import { useEffect, useRef, useState } from 'react';
import { Copy, Check, Plus, X, Link2 } from 'lucide-react';
import { usePortfolio } from './store';
import type { PortfolioAsset } from './types';

/** Contatos da empresa (fictícios). Num sistema real, viria do diretório da empresa. */
export const CONTACTS = [
  { id: 'c1', name: 'Clarice', role: 'Analista fiscal', email: 'fiscal@empresa.exemplo' },
  { id: 'c2', name: 'Izidio', role: 'Analista de logística', email: 'logistica@empresa.exemplo' },
  { id: 'c3', name: 'Coordenação da área', role: 'Coordenador da área requisitante', email: 'coordenacao.manutencao@empresa.exemplo' },
  { id: 'c4', name: 'Encarregado de turno', role: 'Manutenção · turno A', email: 'turno.manutencao@empresa.exemplo' },
  { id: 'c5', name: 'Almoxarifado', role: 'Peças sobressalentes', email: 'almoxarifado@empresa.exemplo' },
];

const VIEWS = [
  { id: 'trajetoria', label: 'Trajetória e prazos', help: 'Etapa atual, com quem está e data prometida.', on: true },
  { id: 'mapa', label: 'Última localização', help: 'Posição válida da TAG, com data.', on: true },
  { id: 'nf', label: 'NF e valores', help: 'Número da NF, valor do bem e orçamento.', on: false },
  { id: 'oficina', label: 'Contato da oficina', help: 'Nome, e-mail e telefone do fornecedor.', on: false },
];

/**
 * Compartilhar o acompanhamento de um ativo com alguém da empresa.
 * Sem backend, o envio abre o e-mail com o link e o resumo; o registro fica no histórico do ativo.
 */
export function ShareDialog({ asset, onClose }: { asset: PortfolioAsset; onClose: () => void }) {
  const { dispatch } = usePortfolio();
  const ref = useRef<HTMLDialogElement>(null);
  const [picked, setPicked] = useState<string[]>([]);
  const [extra, setExtra] = useState<string[]>([]);
  const [email, setEmail] = useState('');
  const [views, setViews] = useState(Object.fromEntries(VIEWS.map(v => [v.id, v.on])) as Record<string, boolean>);
  const [expires, setExpires] = useState('retorno');
  const [copied, setCopied] = useState(false);
  const link = `${location.origin}/ativos/${asset.id}?acompanhar=1`;

  useEffect(() => {
    ref.current?.showModal();
  }, []);

  const recipients = [...CONTACTS.filter(c => picked.includes(c.id)).map(c => c.email), ...extra];
  const names = [...CONTACTS.filter(c => picked.includes(c.id)).map(c => c.name), ...extra];
  const send = () => {
    if (!recipients.length) return;
    const shown = VIEWS.filter(v => views[v.id]).map(v => v.label.toLowerCase()).join(', ');
    const body = [`Compartilho o acompanhamento de ${asset.name} (${asset.id}, ${asset.tag ?? 'sem TAG'}).`, '', `Link: ${link}`, `O que dá para ver: ${shown}.`, `Validade: ${expires === 'retorno' ? 'até o ativo voltar' : expires === '7d' ? '7 dias' : '30 dias'}.`].join('\n');
    window.location.href = `mailto:${recipients.join(',')}?subject=${encodeURIComponent(`Acompanhamento: ${asset.name} (${asset.id})`)}&body=${encodeURIComponent(body)}`;
    dispatch({ type: 'share', id: asset.id, names });
    onClose();
  };
  const addEmail = () => {
    const e = email.trim().toLowerCase();
    if (/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(e) && !extra.includes(e)) setExtra([...extra, e]);
    setEmail('');
  };

  return (
    <dialog ref={ref} className="share" onClose={onClose} aria-labelledby="share-title">
      <div className="share-head">
        <div>
          <h2 id="share-title">Compartilhar acompanhamento</h2>
          <p className="muted">Quem receber vê a trajetória de {asset.name}, sem acesso aos outros ativos.</p>
        </div>
        <button type="button" className="icon-btn" onClick={onClose} aria-label="Fechar"><X size={20} /></button>
      </div>

      <div className="share-block">
        <p className="share-label">Link de acompanhamento</p>
        <div className="share-link">
          <Link2 size={18} aria-hidden="true" />
          <span className="mono grow">{link}</span>
          <button type="button" className="btn btn-secondary" onClick={() => { void navigator.clipboard?.writeText(link); setCopied(true); }}>
            {copied ? <><Check size={16} /> Copiado</> : <><Copy size={16} /> Copiar</>}
          </button>
        </div>
      </div>

      <div className="share-block">
        <p className="share-label">Enviar para</p>
        <ul className="share-people">
          {CONTACTS.map(c => (
            <li key={c.id}>
              <label>
                <input type="checkbox" checked={picked.includes(c.id)} onChange={e => setPicked(e.target.checked ? [...picked, c.id] : picked.filter(x => x !== c.id))} />
                <span className="avatar-p">{c.name[0]}</span>
                <span className="grow"><b>{c.name}</b><span className="muted">{c.role} · {c.email}</span></span>
              </label>
            </li>
          ))}
          {extra.map(e => (
            <li key={e}>
              <label>
                <input type="checkbox" checked onChange={() => setExtra(extra.filter(x => x !== e))} />
                <span className="avatar-p">{e[0].toUpperCase()}</span>
                <span className="grow"><b>{e}</b><span className="muted">Adicionado por e-mail</span></span>
              </label>
            </li>
          ))}
        </ul>
        <form className="share-add" onSubmit={ev => { ev.preventDefault(); addEmail(); }}>
          <Plus size={18} aria-hidden="true" />
          <input className="input" type="email" value={email} onChange={e => setEmail(e.target.value)} placeholder="Adicionar alguém por e-mail" aria-label="Adicionar e-mail" />
          <button type="submit" className="btn btn-secondary">Adicionar</button>
        </form>
      </div>

      <div className="share-block">
        <p className="share-label">O que podem ver</p>
        <ul className="share-views">
          {VIEWS.map(v => (
            <li key={v.id}>
              <span className="grow"><b>{v.label}</b><span className="muted">{v.help}</span></span>
              <button type="button" role="switch" aria-checked={views[v.id]} aria-label={v.label} className={`switch ${views[v.id] ? 'on' : ''}`} onClick={() => setViews({ ...views, [v.id]: !views[v.id] })}><i /></button>
            </li>
          ))}
        </ul>
      </div>

      <div className="share-block">
        <label className="share-label" htmlFor="expira">O link vale</label>
        <select id="expira" className="input" value={expires} onChange={e => setExpires(e.target.value)}>
          <option value="retorno">Até o ativo voltar para a usina</option>
          <option value="7d">7 dias</option>
          <option value="30d">30 dias</option>
        </select>
      </div>

      <div className="share-foot">
        <span className="muted">O e-mail abre no seu programa de e-mail. O link real depende do backend.</span>
        <div className="row-gap">
          <button type="button" className="btn btn-secondary" onClick={onClose}>Cancelar</button>
          <button type="button" className="btn btn-primary" disabled={!recipients.length} onClick={send}>
            {recipients.length ? `Enviar para ${names.length === 1 ? names[0] : `${names.length} pessoas`}` : 'Escolha alguém'}
          </button>
        </div>
      </div>
    </dialog>
  );
}
