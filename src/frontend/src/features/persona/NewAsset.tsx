import { useState } from 'react';
import { Link, useNavigate } from 'react-router-dom';
import { ArrowLeft, Camera, Tag, Building2, ClipboardCheck, Check } from 'lucide-react';
import { usePortfolio } from '@/portfolio/store';
import { legalLimitDays, money } from '@/portfolio/logic';
import { Journey } from '@/portfolio/ui';
import type { Criticality, PortfolioAsset } from '@/portfolio/types';

const steps = [
  { label: 'Ativo e foto', icon: Camera },
  { label: 'TAG e lacre', icon: Tag },
  { label: 'Oficina e prazo', icon: Building2 },
  { label: 'Revisar e pedir NF', icon: ClipboardCheck },
];

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

export function NewAsset() {
  const { state, dispatch } = usePortfolio();
  const nav = useNavigate();
  const [step, setStep] = useState(0);
  const [f, setF] = useState({
    name: '', code: '', om: '', area: 'Extração', problem: '', criticality: 'media' as Criticality, photo: null as string | null,
    tag: state.freeTags[0] ?? '', seal: false, supplierId: '', quote: '', days: '30',
  });
  const [error, setError] = useState('');
  const set = (k: keyof typeof f, v: unknown) => setF(x => ({ ...x, [k]: v }));
  const supplier = state.suppliers.find(s => s.id === f.supplierId) ?? null;

  const check = () => {
    if (step === 0 && (!f.name.trim() || !f.om.trim())) return 'Informe o nome do ativo e a OM.';
    if (step === 1 && (!f.tag || !f.seal)) return 'Escolha a TAG e confirme que o lacre foi aplicado.';
    if (step === 2 && (!f.supplierId || !(Number(f.quote) > 0) || !(Number(f.days) > 0))) return 'Escolha a oficina e informe orçamento e prazo.';
    return '';
  };
  const next = () => {
    const e = check();
    setError(e);
    if (!e) setStep(step + 1);
  };
  const finish = (requestNf: boolean) => {
    const now = new Date().toISOString();
    const id = f.code.trim().toUpperCase() || `AT-${String(Date.now()).slice(-4)}`;
    const asset: PortfolioAsset = {
      id, name: f.name.trim(), om: f.om.trim().toUpperCase(), area: f.area, owner: 'Geraldo (PCM)', criticality: f.criticality, photo: f.photo, tag: f.tag, sealApplied: true,
      stage: requestNf ? 'aguardando_nf' : 'planejamento', supplierId: f.supplierId, quote: Number(f.quote), agreedDays: Number(f.days), problem: f.problem.trim() || 'Sem descrição',
      createdAt: now, nfRequestedAt: requestNf ? now : null, remittance: null, pickupRequestedAt: null, departedAt: null, arrivedSupplierAt: null, dueAt: null, returnStartedAt: null,
      arrivedPlantAt: null, returnInvoice: null, closedAt: null, seal: 'integro', battery: 100, lastSignalAt: now, position: null, lastFollowUpAt: null,
      history: [
        ...(requestNf ? [{ at: now, who: 'Geraldo (PCM)', text: 'NF de remessa solicitada ao Fiscal.' }] : []),
        { at: now, who: 'Geraldo (PCM)', text: `Ativo cadastrado com foto, ${f.tag} vinculada e lacre aplicado.` },
      ],
    };
    dispatch({ type: 'create', asset, requestNf });
    nav(`/ativos/${id}`);
  };

  return (
    <div className="persona-page narrow">
      <Link to="/" className="back"><ArrowLeft size={16} /> Painel</Link>
      <h1>Novo envio para manutenção</h1>
      <p className="muted">Todo ativo segue a mesma trajetória. Cada parada tem um dono, e o ativo aparece na mesa dessa pessoa até ela resolver. Você está montando a primeira parada.</p>
      <Journey stage="planejamento" here="Você está aqui" />
      <ol className="wizard-steps">
        {steps.map((s, i) => (
          <li key={s.label} className={i < step ? 'done' : i === step ? 'now' : ''}>
            <span>{i < step ? <Check size={14} /> : <s.icon size={14} />}</span> {s.label}
          </li>
        ))}
      </ol>
      <section className="card section-card">
        {step === 0 && (
          <div className="gt-form">
            <div className="photo-row">
              <label className="photo-drop">
                {f.photo ? <img src={f.photo} alt="Foto do ativo" /> : <><Camera size={28} /><span>Tirar ou escolher foto</span></>}
                <input type="file" accept="image/*" capture="environment" onChange={async e => { const file = e.target.files?.[0]; if (file) set('photo', await resize(file)); }} />
              </label>
              <div className="form-grid grow">
                <label>Nome do ativo<input className="input" value={f.name} onChange={e => set('name', e.target.value)} placeholder="Ex.: Bomba centrífuga da moenda" /></label>
                <label>Código ou patrimônio<input className="input mono" value={f.code} onChange={e => set('code', e.target.value.toUpperCase())} placeholder="AT-0000 (opcional)" /></label>
                <label>Ordem de manutenção (SAP)<input className="input mono" value={f.om} onChange={e => set('om', e.target.value.toUpperCase())} placeholder="OM-0000000" /></label>
                <label>Área requisitante<select className="input" value={f.area} onChange={e => set('area', e.target.value)}>{['Extração', 'Caldeira', 'Destilaria', 'Utilidades', 'Fábrica de açúcar', 'Logística interna'].map(x => <option key={x}>{x}</option>)}</select></label>
              </div>
            </div>
            <label>Defeito relatado<input className="input" value={f.problem} onChange={e => set('problem', e.target.value)} placeholder="Ex.: rolamento travado" /></label>
            <fieldset className="seg">
              <legend>Criticidade para a operação</legend>
              {(['alta', 'media', 'baixa'] as Criticality[]).map(c => (
                <label key={c} className={f.criticality === c ? 'on' : ''}><input type="radio" name="crit" checked={f.criticality === c} onChange={() => set('criticality', c)} /> {c === 'media' ? 'Média' : c[0].toUpperCase() + c.slice(1)}</label>
              ))}
            </fieldset>
          </div>
        )}
        {step === 1 && (
          <div className="gt-form">
            <p>Escolha uma TAG livre, prenda no ativo e aplique o lacre.</p>
            <div className="tag-grid">
              {state.freeTags.map(t => (
                <button key={t} type="button" className={`tag-option ${f.tag === t ? 'on' : ''}`} onClick={() => set('tag', t)}>
                  <Tag size={16} /> <span className="mono">{t}</span>
                </button>
              ))}
              {!state.freeTags.length && <p className="muted">Nenhuma TAG livre. Encerre um ciclo para liberar uma.</p>}
            </div>
            <label className="check"><input type="checkbox" checked={f.seal} onChange={e => set('seal', e.target.checked)} /> Lacre aplicado e fechado</label>
          </div>
        )}
        {step === 2 && (
          <div className="gt-form form-grid">
            <label>Oficina<select className="input" value={f.supplierId} onChange={e => set('supplierId', e.target.value)}><option value="">Escolha</option>{state.suppliers.map(s => <option key={s.id} value={s.id}>{s.name} · {s.city}/{s.uf}</option>)}</select></label>
            <label>Orçamento (R$)<input className="input mono" inputMode="numeric" value={f.quote} onChange={e => set('quote', e.target.value.replace(/\D/g, ''))} /></label>
            <label>Prazo de reparo combinado (dias)<input className="input mono" inputMode="numeric" value={f.days} onChange={e => set('days', e.target.value.replace(/\D/g, ''))} /></label>
            {supplier && <p className="small muted span-2">Contato: {supplier.contact} · {supplier.email} · {supplier.phone}. Prazo legal da remessa: {legalLimitDays(supplier)} dias ({supplier.uf === 'GO' ? 'dentro de GO' : 'fora de GO'}).</p>}
          </div>
        )}
        {step === 3 && (
          <div className="handoff">
            <span className="persona-dot p-fiscal">C</span>
            <p>Ao confirmar, o ativo sai da sua mesa e vai para a da <b>Clarice (Fiscal)</b> emitir a NF de remessa. Quando a NF sair, ele volta para você acionar a logística.</p>
          </div>
        )}
        {step === 3 && (
          <div className="review">
            {f.photo && <img src={f.photo} alt="Foto do ativo" className="review-photo" />}
            <dl className="facts">
              <div><dt>Ativo</dt><dd>{f.name}</dd></div>
              <div><dt>OM</dt><dd className="mono">{f.om}</dd></div>
              <div><dt>TAG</dt><dd className="mono">{f.tag} · lacre aplicado</dd></div>
              <div><dt>Oficina</dt><dd>{supplier?.name} · {supplier?.city}/{supplier?.uf}</dd></div>
              <div><dt>Orçamento</dt><dd className="mono">{money(Number(f.quote))}</dd></div>
              <div><dt>Prazo</dt><dd>{f.days} dias de reparo</dd></div>
            </dl>
          </div>
        )}
        {error && <p role="alert" className="form-error">{error}</p>}
        <div className="row-gap wizard-nav">
          {step > 0 && <button className="btn btn-secondary" type="button" onClick={() => setStep(step - 1)}>Voltar</button>}
          {step < 3 && <button className="btn btn-primary" type="button" onClick={next}>Continuar</button>}
          {step === 3 && (
            <>
              <button className="btn btn-primary" type="button" onClick={() => finish(true)}>Cadastrar e pedir NF ao Fiscal</button>
              <button className="btn btn-ghost" type="button" onClick={() => finish(false)}>Só cadastrar</button>
            </>
          )}
        </div>
      </section>
    </div>
  );
}
