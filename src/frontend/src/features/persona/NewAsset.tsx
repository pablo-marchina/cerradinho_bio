import { useMemo, useState } from 'react';
import { Link, useNavigate } from 'react-router-dom';
import { ArrowLeft, ArrowRight, Camera, Check, Tag, Search, Plus, Repeat } from 'lucide-react';
import { usePortfolio } from '@/portfolio/store';
import { legalLimitDays, money, supplierStats, dateBr } from '@/portfolio/logic';
import { AssetThumb, Avatar } from '@/portfolio/ui';
import type { Criticality, PortfolioAsset } from '@/portfolio/types';

/** Reduz a foto para no máximo 480 px, para caber no armazenamento local. */
function resize(file: File): Promise<string> {
  return new Promise((resolve, reject) => {
    const img = new Image();
    img.onload = () => {
      const scale = Math.min(1, 480 / Math.max(img.width, img.height));
      const c = document.createElement('canvas');
      c.width = Math.round(img.width * scale);
      c.height = Math.round(img.height * scale);
      c.getContext('2d')!.drawImage(img, 0, 0, c.width, c.height);
      resolve(c.toDataURL('image/jpeg', 0.8));
      URL.revokeObjectURL(img.src);
    };
    img.onerror = reject;
    img.src = URL.createObjectURL(file);
  });
}

const AREAS = ['Extração', 'Caldeira', 'Destilaria', 'Utilidades', 'Fábrica de açúcar', 'Logística interna'];
const STEPS = [
  { id: 'origem', title: 'Qual ativo vai sair?', short: 'Ativo' },
  { id: 'foto', title: 'Tire uma foto do ativo', short: 'Foto' },
  { id: 'defeito', title: 'O que aconteceu e quanto ele faz falta?', short: 'Defeito' },
  { id: 'oficina', title: 'Para qual oficina ele vai?', short: 'Oficina' },
  { id: 'combinado', title: 'O que foi combinado com a oficina?', short: 'Combinado' },
  { id: 'tag', title: 'Prenda uma TAG e aplique o lacre', short: 'TAG e lacre' },
  { id: 'revisar', title: 'Confira e envie para o Fiscal', short: 'Enviar' },
] as const;

/**
 * Novo envio guiado: uma pergunta por tela, com o resumo do envio sempre visível ao lado.
 * Termina com o pedido de NF de remessa, que passa o ativo para a mesa da Clarice (Fiscal).
 */
export function NewAsset() {
  const { state, dispatch } = usePortfolio();
  const nav = useNavigate();
  const [step, setStep] = useState(0);
  const [mode, setMode] = useState<'novo' | 'existente' | null>(null);
  const [f, setF] = useState({
    sourceId: '', name: '', code: '', om: '', area: 'Extração', problem: '', criticality: '' as Criticality | '', photo: null as string | null,
    supplierId: '', quote: '', days: '30', tag: '', seal: false,
  });
  const [q, setQ] = useState('');
  const [error, setError] = useState('');
  const set = <K extends keyof typeof f>(k: K, v: (typeof f)[K]) => setF(x => ({ ...x, [k]: v }));
  const supplier = state.suppliers.find(s => s.id === f.supplierId) ?? null;
  const stats = useMemo(() => supplierStats(state), [state]);
  const promised = f.days ? new Date(Date.now() + (Number(f.days) + 2) * 86400000).toISOString() : null;
  const closedAssets = state.assets.filter(a => a.stage === 'encerrado' && [a.name, a.id, a.om].some(v => v.toLowerCase().includes(q.toLowerCase())));

  const validate = () => {
    const s = STEPS[step].id;
    if (s === 'origem' && (!mode || (mode === 'existente' && !f.sourceId) || (mode === 'novo' && (!f.name.trim() || !f.om.trim())))) return mode === 'existente' ? 'Escolha o ativo na lista.' : 'Informe o nome do ativo e a OM do SAP.';
    if (s === 'defeito' && (!f.problem.trim() || !f.criticality)) return 'Descreva o defeito e escolha a criticidade.';
    if (s === 'oficina' && !f.supplierId) return 'Escolha a oficina.';
    if (s === 'combinado' && (!(Number(f.quote) > 0) || !(Number(f.days) > 0))) return 'Informe o valor e o prazo de reparo combinados.';
    if (s === 'tag' && (!f.tag || !f.seal)) return 'Escolha a TAG e confirme o lacre.';
    return '';
  };
  const next = () => {
    const e = validate();
    setError(e);
    if (!e) setStep(Math.min(step + 1, STEPS.length - 1));
  };
  const back = () => { setError(''); setStep(Math.max(0, step - 1)); };

  const finish = (requestNf: boolean) => {
    const now = new Date().toISOString();
    const id = f.code.trim().toUpperCase() || `AT-${String(Date.now()).slice(-4)}`;
    const asset: PortfolioAsset = {
      id, name: f.name.trim(), om: f.om.trim().toUpperCase(), area: f.area, owner: 'Geraldo (PCM)', criticality: f.criticality || 'media', photo: f.photo, tag: f.tag, sealApplied: true,
      stage: requestNf ? 'aguardando_nf' : 'planejamento', supplierId: f.supplierId, quote: Number(f.quote), agreedDays: Number(f.days), problem: f.problem.trim(),
      createdAt: now, nfRequestedAt: requestNf ? now : null, remittance: null, pickupRequestedAt: null, departedAt: null, arrivedSupplierAt: null, dueAt: null, returnStartedAt: null,
      arrivedPlantAt: null, returnInvoice: null, closedAt: null, seal: 'integro', battery: 100, lastSignalAt: now, position: null, lastFollowUpAt: null,
      history: [
        ...(requestNf ? [{ at: now, who: 'Geraldo (PCM)', text: 'NF de remessa solicitada ao Fiscal.' }] : []),
        { at: now, who: 'Geraldo (PCM)', text: `Envio cadastrado: ${supplier?.name}, ${money(Number(f.quote))}, ${f.days} dias. ${f.tag} vinculada e lacre aplicado.` },
      ],
    };
    dispatch({ type: 'create', asset, requestNf });
    nav(`/ativos/${id}`);
  };

  const s = STEPS[step];
  return (
    <div className="pp wizard">
      <Link to="/" className="back"><ArrowLeft size={18} /> Painel</Link>
      <div className="wz">
        <aside className="wz-steps" aria-label="Passos do envio">
          <p className="eyebrow">Novo envio</p>
          <ol>
            {STEPS.map((x, i) => (
              <li key={x.id} className={i < step ? 'done' : i === step ? 'now' : ''}>
                <span className="wz-dot">{i < step ? <Check size={16} /> : i + 1}</span>
                <span>{x.short}</span>
              </li>
            ))}
          </ol>
          <div className="wz-after">
            <p className="eyebrow">Depois do envio</p>
            <p><Avatar p="fiscal" size="sm" /> Clarice emite a NF</p>
            <p><Avatar p="pcm" size="sm" /> Você aciona a coleta</p>
            <p><Avatar p="logistica" size="sm" /> Izidio acompanha o transporte</p>
          </div>
        </aside>

        <section className="wz-main" aria-labelledby="wz-title">
          <p className="muted">Passo {step + 1} de {STEPS.length}</p>
          <h1 id="wz-title">{s.title}</h1>

          {s.id === 'origem' && (
            <div className="wz-body">
              <div className="choice-row">
                <button type="button" className={`choice ${mode === 'novo' ? 'on' : ''}`} onClick={() => setMode('novo')}>
                  <Plus size={26} /><b>Ativo novo</b><span>Primeira vez que ele sai para manutenção.</span>
                </button>
                <button type="button" className={`choice ${mode === 'existente' ? 'on' : ''}`} onClick={() => setMode('existente')}>
                  <Repeat size={26} /><b>Já foi cadastrado</b><span>Ele já teve um ciclo encerrado. Os dados vêm prontos.</span>
                </button>
              </div>
              {mode === 'novo' && (
                <div className="form-grid">
                  <label>Nome do ativo<input className="input" autoFocus value={f.name} onChange={e => set('name', e.target.value)} placeholder="Ex.: Bomba centrífuga da moenda" /></label>
                  <label>OM no SAP<input className="input mono" value={f.om} onChange={e => set('om', e.target.value.toUpperCase())} placeholder="OM-0000000" /></label>
                  <label>Patrimônio ou código (opcional)<input className="input mono" value={f.code} onChange={e => set('code', e.target.value.toUpperCase())} placeholder="AT-0000" /></label>
                  <label>Área requisitante<select className="input" value={f.area} onChange={e => set('area', e.target.value)}>{AREAS.map(x => <option key={x}>{x}</option>)}</select></label>
                </div>
              )}
              {mode === 'existente' && (
                <div className="pick-existing">
                  <label className="search-line"><Search size={18} /><input className="input" value={q} onChange={e => setQ(e.target.value)} placeholder="Buscar por nome, código ou OM" aria-label="Buscar ativo cadastrado" /></label>
                  <ul>
                    {closedAssets.slice(0, 6).map(a => (
                      <li key={a.id}>
                        <button type="button" className={f.sourceId === a.id ? 'on' : ''} onClick={() => setF(x => ({ ...x, sourceId: a.id, name: a.name, om: a.om, area: a.area, code: '', photo: a.photo }))}>
                          <AssetThumb asset={a} size={40} />
                          <span className="grow"><b>{a.name}</b><span className="muted mono">{a.id} · {a.area} · último retorno {dateBr(a.closedAt)}</span></span>
                          {f.sourceId === a.id && <Check size={20} />}
                        </button>
                      </li>
                    ))}
                  </ul>
                  {f.sourceId && <label>Nova OM no SAP<input className="input mono" value={f.om} onChange={e => set('om', e.target.value.toUpperCase())} /></label>}
                </div>
              )}
            </div>
          )}

          {s.id === 'foto' && (
            <div className="wz-body">
              <p className="lead">A foto ajuda a conferir, na volta, se é o mesmo ativo. No celular, abre a câmera.</p>
              <label className="photo-drop big">
                {f.photo ? <img src={f.photo} alt="Foto do ativo" /> : <><Camera size={40} /><b>Tirar ou escolher foto</b><span className="muted">JPG ou PNG</span></>}
                <input type="file" accept="image/*" capture="environment" onChange={async e => { const file = e.target.files?.[0]; if (file) set('photo', await resize(file)); }} />
              </label>
              {f.photo && <button type="button" className="btn btn-ghost" onClick={() => set('photo', null)}>Trocar foto</button>}
              <p className="muted">Pode pular este passo e adicionar a foto depois.</p>
            </div>
          )}

          {s.id === 'defeito' && (
            <div className="wz-body">
              <label className="big-label">Defeito relatado<textarea className="input" rows={3} autoFocus value={f.problem} onChange={e => set('problem', e.target.value)} placeholder="Ex.: rolamento travado, vibração excessiva" /></label>
              <fieldset className="choice-row">
                <legend className="big-label">Quanto ele faz falta para a operação?</legend>
                {([['alta', 'Alta', 'Para a moagem ou não tem reserva.'], ['media', 'Média', 'Tem reserva, mas por pouco tempo.'], ['baixa', 'Baixa', 'Pode esperar sem impacto.']] as const).map(([v, l, d]) => (
                  <label key={v} className={`choice ${f.criticality === v ? 'on' : ''}`}>
                    <input type="radio" name="crit" checked={f.criticality === v} onChange={() => set('criticality', v)} />
                    <b>{l}</b><span>{d}</span>
                  </label>
                ))}
              </fieldset>
            </div>
          )}

          {s.id === 'oficina' && (
            <div className="wz-body">
              <p className="lead">Compare o histórico de cada oficina antes de escolher.</p>
              <ul className="supplier-pick">
                {state.suppliers.map(x => {
                  const st = stats.find(y => y.supplier.id === x.id);
                  return (
                    <li key={x.id}>
                      <button type="button" className={f.supplierId === x.id ? 'on' : ''} onClick={() => set('supplierId', x.id)}>
                        <span className="grow"><b>{x.name}</b><span className="muted">{x.city}/{x.uf} · prazo fiscal {legalLimitDays(x)} dias</span></span>
                        <span className="sp-stat"><b className="mono">{st?.avgRepairDays ?? '—'}{st?.avgRepairDays ? ' d' : ''}</b><span className="muted">reparo médio</span></span>
                        <span className="sp-stat"><b className={`mono t-${st?.onTimeRate == null ? 'none' : st.onTimeRate >= 80 ? 'ok' : st.onTimeRate >= 60 ? 'warn' : 'alert'}`}>{st?.onTimeRate ?? '—'}{st?.onTimeRate != null ? '%' : ''}</b><span className="muted">no prazo</span></span>
                        <span className="sp-stat"><b className="mono">{st?.open ?? 0}</b><span className="muted">lá agora</span></span>
                      </button>
                    </li>
                  );
                })}
              </ul>
            </div>
          )}

          {s.id === 'combinado' && (
            <div className="wz-body">
              <div className="form-grid">
                <label className="big-label">Valor do reparo (R$)<input className="input mono big" inputMode="numeric" autoFocus value={f.quote} onChange={e => set('quote', e.target.value.replace(/\D/g, ''))} placeholder="0" /></label>
                <label className="big-label">Prazo de reparo (dias)<input className="input mono big" inputMode="numeric" value={f.days} onChange={e => set('days', e.target.value.replace(/\D/g, ''))} /></label>
              </div>
              {supplier && promised && (
                <p className="lead">Com a coleta em até 2 dias, a {supplier.name} deve devolver por volta de <b>{dateBr(promised)}</b>. O alerta de "perto do prazo" aparece 7 dias antes.</p>
              )}
              {supplier && <p className="muted">Contato: {supplier.contact} · {supplier.email} · {supplier.phone}</p>}
            </div>
          )}

          {s.id === 'tag' && (
            <div className="wz-body">
              <p className="lead">Escolha uma TAG livre, prenda no ativo e feche o lacre. A TAG passa a contar o tempo fora e avisa se o lacre abrir.</p>
              <div className="tag-grid">
                {state.freeTags.map(t => (
                  <button key={t} type="button" className={`tag-option ${f.tag === t ? 'on' : ''}`} onClick={() => set('tag', t)} aria-pressed={f.tag === t}>
                    <Tag size={18} /> <span className="mono">{t}</span>
                  </button>
                ))}
                {!state.freeTags.length && <p className="muted">Nenhuma TAG livre. Encerre um ciclo para liberar uma.</p>}
              </div>
              <label className="check big"><input type="checkbox" checked={f.seal} onChange={e => set('seal', e.target.checked)} /> Lacre aplicado e fechado</label>
            </div>
          )}

          {s.id === 'revisar' && (
            <div className="wz-body">
              <p className="lead">Ao enviar, o ativo sai da sua mesa e vai para a da <b>Clarice (Fiscal)</b> emitir a NF de remessa. Quando a NF sair, ele volta para você acionar a coleta.</p>
            </div>
          )}

          {error && <p role="alert" className="form-error">{error}</p>}
          <div className="wz-nav">
            {step > 0 && <button type="button" className="btn btn-secondary btn-lg" onClick={back}><ArrowLeft size={18} /> Voltar</button>}
            {s.id !== 'revisar' && <button type="button" className="btn btn-primary btn-lg" onClick={next}>{s.id === 'foto' && !f.photo ? 'Pular' : 'Continuar'} <ArrowRight size={18} /></button>}
            {s.id === 'revisar' && (
              <>
                <button type="button" className="btn btn-primary btn-lg" onClick={() => finish(true)}>Enviar para a Clarice emitir a NF</button>
                <button type="button" className="btn btn-ghost" onClick={() => finish(false)}>Salvar sem pedir NF</button>
              </>
            )}
          </div>
        </section>

        <aside className="wz-summary" aria-label="Resumo do envio">
          <p className="eyebrow">Resumo do envio</p>
          <div className="ticket">
            {f.photo ? <img src={f.photo} alt="" className="ticket-photo" /> : <div className="ticket-photo empty"><Camera size={28} /></div>}
            <dl>
              <div><dt>Ativo</dt><dd>{f.name || '—'}</dd></div>
              <div><dt>OM</dt><dd className="mono">{f.om || '—'}</dd></div>
              <div><dt>Criticidade</dt><dd>{f.criticality ? { alta: 'Alta', media: 'Média', baixa: 'Baixa' }[f.criticality] : '—'}</dd></div>
              <div><dt>Oficina</dt><dd>{supplier ? `${supplier.name}, ${supplier.city}/${supplier.uf}` : '—'}</dd></div>
              <div><dt>Combinado</dt><dd>{f.quote ? `${money(Number(f.quote))} · ${f.days} dias` : '—'}</dd></div>
              <div><dt>TAG</dt><dd className="mono">{f.tag ? `${f.tag}${f.seal ? ' · lacrada' : ''}` : '—'}</dd></div>
              <div><dt>NF de remessa</dt><dd>Será emitida pelo Fiscal</dd></div>
            </dl>
          </div>
        </aside>
      </div>
    </div>
  );
}
