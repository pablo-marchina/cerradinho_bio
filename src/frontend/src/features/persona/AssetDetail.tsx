import { useState } from 'react';
import { Link, useParams } from 'react-router-dom';
import { ArrowLeft, Mail, Phone, MapPin, BatteryMedium, ShieldCheck, ShieldAlert, Radio, Building2, Receipt, CalendarClock, PlayCircle } from 'lucide-react';
import { usePortfolio } from '@/portfolio/store';
import { AssetThumb, Avatar, DeskOwner, DueBar, StatusText, Journey, StageLabel } from '@/portfolio/ui';
import {
  ago, criticalityLabel, dateBr, daysSinceNf, dueRemaining, icmsAtRisk, legalLimitDays, legalRemaining, money, nextAction, personaLabel, personaName, supplierOf,
} from '@/portfolio/logic';
import { NfForm } from './NfForm';
import type { PortfolioAsset } from '@/portfolio/types';

const sealText = { integro: 'Íntegro', violado: 'Violado', autorizado: 'Aberto com autorização', sem_lacre: 'Sem lacre' } as const;

export function AssetDetail() {
  const { id } = useParams();
  const { state } = usePortfolio();
  const a = state.assets.find(x => x.id === id);
  if (!a) return (
    <div className="pp">
      <h1>Ativo não encontrado</h1>
      <Link to="/">Voltar ao painel</Link>
    </div>
  );
  const s = supplierOf(a, state.suppliers);
  const legal = legalRemaining(a, state.now, state.suppliers);
  const since = daysSinceNf(a, state.now);
  return (
    <div className="pp">
      <Link to="/" className="back"><ArrowLeft size={16} /> Painel</Link>
      <header className="asset-hero">
        <AssetThumb asset={a} size={112} />
        <div className="asset-hero-main">
          <div className="row-gap">
            <StageLabel stage={a.stage} />
            <span className={`chip crit-${a.criticality}`}>Criticidade {criticalityLabel[a.criticality]}</span>
          </div>
          <h1>{a.name}</h1>
          <p className="muted mono">{a.id} · {a.om} · {a.tag ?? 'sem TAG'} · {a.area}</p>
          <p className="small">{a.problem}</p>
          <StatusText asset={a} />
          <DeskOwner asset={a} />
        </div>
      </header>

      <Journey stage={a.stage} dates={[a.createdAt, a.nfRequestedAt, a.remittance?.issuedAt ?? null, a.departedAt, a.arrivedSupplierAt, a.returnStartedAt, a.arrivedPlantAt, a.closedAt]} here={a.stage === 'encerrado' ? 'Fim' : `Com ${personaName[nextAction(a, state.now).persona]}`} />

      <NextActionCard asset={a} />

      <div className="detail-cols">
        <section className="pp-section">
          <h2><Building2 size={18} /> Oficina e combinado</h2>
          {s ? (
            <dl className="facts">
              <div><dt>Oficina</dt><dd>{s.name}</dd></div>
              <div><dt>Cidade</dt><dd>{s.city}/{s.uf}</dd></div>
              <div><dt>Contato</dt><dd>{s.contact}</dd></div>
              <div><dt>Orçamento</dt><dd className="mono">{money(a.quote)}</dd></div>
              <div><dt>Prazo combinado</dt><dd>{a.agreedDays ? `${a.agreedDays} dias de reparo` : '—'}</dd></div>
              <div><dt>Data prometida</dt><dd>{dateBr(a.dueAt)}</dd></div>
            </dl>
          ) : <p className="muted">Oficina ainda não definida.</p>}
          {s && (
            <div className="row-gap contact-actions">
              <a className="btn btn-secondary" href={mailto(a, s.email, state.now)}><Mail size={16} /> E-mail para a oficina</a>
              <a className="btn btn-secondary" href={`tel:${s.phone.replace(/\D/g, '')}`}><Phone size={16} /> {s.phone}</a>
            </div>
          )}
          {['ida', 'oficina'].includes(a.stage) && <div className="due-big"><DueBar asset={a} now={state.now} /></div>}
        </section>

        <section className="pp-section">
          <h2><Receipt size={18} /> Fiscal</h2>
          <dl className="facts">
            <div><dt>NF de remessa</dt><dd className="mono">{a.remittance?.number ?? 'Não emitida'}</dd></div>
            <div><dt>Emissão</dt><dd>{dateBr(a.remittance?.issuedAt)}</dd></div>
            <div><dt>CFOP</dt><dd className="mono">{a.remittance?.cfop ?? '—'}</dd></div>
            <div><dt>Valor do bem</dt><dd className="mono">{money(a.remittance?.value)}</dd></div>
            <div><dt>NF de retorno</dt><dd className="mono">{a.returnInvoice?.number ?? 'Não lançada'}</dd></div>
            <div><dt>ICMS suspenso (estim.)</dt><dd className="mono">{money(icmsAtRisk(a))}</dd></div>
          </dl>
          {legal !== null && since !== null && (
            <div className="legal">
              <div className="legal-head">
                <CalendarClock size={16} /> Prazo legal: {since} de {legalLimitDays(s)} dias
                <b className={legal <= 30 ? 'text-danger' : ''}>{legal > 0 ? `restam ${legal}` : 'vencido'}</b>
              </div>
              <span className="legal-bar"><span style={{ width: `${Math.min(100, (since / legalLimitDays(s)) * 100)}%` }} className={legal <= 30 ? 'late' : legal <= 90 ? 'soon' : ''} /></span>
            </div>
          )}
        </section>

        <section className="pp-section">
          <h2><Radio size={18} /> TAG e lacre</h2>
          {a.tag ? (
            <dl className="facts">
              <div><dt>TAG</dt><dd className="mono">{a.tag}</dd></div>
              <div><dt>Lacre</dt><dd className={a.seal === 'violado' ? 'text-danger' : ''}>{a.seal === 'violado' ? <ShieldAlert size={14} /> : <ShieldCheck size={14} />} {sealText[a.seal]}</dd></div>
              <div><dt>Bateria</dt><dd><BatteryMedium size={14} /> {a.battery === null ? '—' : `${a.battery}%`}</dd></div>
              <div><dt>Último sinal</dt><dd>{ago(a.lastSignalAt, state.now)}</dd></div>
              <div><dt>Última posição</dt><dd><MapPin size={14} /> {a.position?.label ?? 'Sem posição válida'}</dd></div>
              <div><dt>Posição de</dt><dd>{a.position ? dateBr(a.position.at) : '—'}</dd></div>
            </dl>
          ) : <p className="muted">Sem TAG vinculada{a.stage === 'encerrado' ? ': liberada no encerramento.' : '.'}</p>}
          <Link className="small link" to="/dispositivos">Ver telemetria e comandos da TAG</Link>
        </section>

        <section className="pp-section">
          <h2>Histórico</h2>
          <ol className="history">
            {a.history.map((h, i) => (
              <li key={i} className={`h-${h.tone ?? 'info'}`}>
                <span className="small muted">{dateBr(h.at)} · {h.who}</span>
                <span>{h.text}</span>
              </li>
            ))}
          </ol>
        </section>
      </div>
    </div>
  );
}

function mailto(a: PortfolioAsset, email: string, now: string) {
  const r = dueRemaining(a, now);
  const subject = `Andamento do reparo: ${a.name} (${a.id})`;
  const body = [
    'Olá,',
    '',
    `Gostaria de uma atualização sobre o reparo do ${a.name} (${a.id}, ${a.tag ?? 'sem TAG'}), enviado com a NF ${a.remittance?.number ?? '(a informar)'}.`,
    r === null ? '' : r < 0 ? `O prazo combinado venceu em ${dateBr(a.dueAt)} (${-r} dias de atraso).` : `O prazo combinado é ${dateBr(a.dueAt)}.`,
    'Qual a previsão de devolução?',
    '',
    'Obrigado,',
    'Geraldo (PCM)',
  ].join('\n');
  return `mailto:${email}?subject=${encodeURIComponent(subject)}&body=${encodeURIComponent(body)}`;
}

function NextActionCard({ asset }: { asset: PortfolioAsset }) {
  const { state, dispatch } = usePortfolio();
  const n = nextAction(asset, state.now);
  const [supplierId, setSupplierId] = useState(asset.supplierId ?? '');
  const [quote, setQuote] = useState(asset.quote ? String(asset.quote) : '');
  const [days, setDays] = useState(asset.agreedDays ? String(asset.agreedDays) : '30');
  const [note, setNote] = useState('');
  const s = supplierOf(asset, state.suppliers);
  if (n.key === 'none' && asset.stage === 'encerrado') return null;

  return (
    <section className={`card next-card who-${n.persona}`} aria-labelledby="next">
      <div className="next-head">
        <Avatar p={n.persona} />
        <div>
          <p className="eyebrow">Próxima ação · {personaLabel[n.persona]}</p>
          <h2 id="next">{n.label}</h2>
        </div>
      </div>

      {n.key === 'plan' && (
        <form
          className="gt-form form-grid"
          onSubmit={e => {
            e.preventDefault();
            if (supplierId && Number(quote) > 0 && Number(days) > 0) dispatch({ type: 'plan', id: asset.id, supplierId, quote: Number(quote), agreedDays: Number(days) });
          }}
        >
          <label>
            Oficina
            <select className="input" value={supplierId} onChange={e => setSupplierId(e.target.value)}>
              <option value="">Escolha</option>
              {state.suppliers.map(x => <option key={x.id} value={x.id}>{x.name} · {x.city}/{x.uf}</option>)}
            </select>
          </label>
          <label>Orçamento (R$)<input className="input mono" inputMode="numeric" value={quote} onChange={e => setQuote(e.target.value.replace(/\D/g, ''))} /></label>
          <label>Prazo de reparo (dias)<input className="input mono" inputMode="numeric" value={days} onChange={e => setDays(e.target.value.replace(/\D/g, ''))} /></label>
          <button className="btn btn-primary" type="submit">Salvar combinado</button>
        </form>
      )}

      {n.key === 'request_nf' && (
        <div className="row-gap">
          <button className="btn btn-primary" type="button" onClick={() => dispatch({ type: 'request_nf', id: asset.id })}>Pedir NF de remessa ao Fiscal</button>
          <span className="small muted">A Clarice recebe o pedido com oficina, valor e prazo já preenchidos.</span>
        </div>
      )}

      {n.key === 'issue_nf' && <NfForm asset={asset} kind="remessa" />}

      {n.key === 'pickup' && (
        <div className="row-gap">
          <button className="btn btn-primary" type="button" onClick={() => dispatch({ type: 'pickup', id: asset.id })}>Acionar logística para a coleta</button>
          <span className="small muted">O ativo passa para a mesa do Izidio (Logística), com a NF e o endereço da oficina.</span>
        </div>
      )}

      {n.key === 'dispatch' && (
        <div className="row-gap">
          <button className="btn btn-primary" type="button" onClick={() => dispatch({ type: 'advance', id: asset.id })}><PlayCircle size={16} /> Simular coleta (TAG sai da usina)</button>
          <span className="small muted">O prazo legal já corre desde a NF; a TAG registra a saída.</span>
        </div>
      )}

      {n.key === 'track' && (
        <div className="row-gap">
          <button className="btn btn-primary" type="button" onClick={() => dispatch({ type: 'advance', id: asset.id })}><PlayCircle size={16} /> Simular chegada na oficina</button>
          <span className="small muted">Quando a TAG chega à oficina, o ativo volta para a mesa do Geraldo acompanhar o reparo.</span>
        </div>
      )}

      {n.key === 'follow_up' && (
        <div className="follow">
          <div className="row-gap">
            {s && <a className="btn btn-primary" href={mailto(asset, s.email, state.now)}><Mail size={16} /> E-mail para {s.contact.split(' ')[0]}</a>}
            {s && <a className="btn btn-secondary" href={`tel:${s.phone.replace(/\D/g, '')}`}><Phone size={16} /> Ligar</a>}
          </div>
          <form className="row-gap" onSubmit={e => { e.preventDefault(); dispatch({ type: 'follow_up', id: asset.id, note: note ? `Cobrança: ${note}` : '' }); setNote(''); }}>
            <input className="input grow" value={note} onChange={e => setNote(e.target.value)} placeholder="O que a oficina respondeu? Ex.: nova data 30/11" aria-label="Resposta da oficina" />
            <button className="btn btn-secondary" type="submit">Registrar cobrança</button>
          </form>
          <button className="btn btn-ghost small" type="button" onClick={() => dispatch({ type: 'advance', id: asset.id })}><PlayCircle size={16} /> Simular fim do reparo (TAG sai da oficina)</button>
        </div>
      )}

      {n.key === 'confirm_arrival' && (
        <div className="row-gap">
          <button className="btn btn-primary" type="button" onClick={() => dispatch({ type: 'advance', id: asset.id })}><PlayCircle size={16} /> Simular chegada na usina (portaria)</button>
          <span className="small muted">Ao chegar, o ativo vai para a mesa da Clarice lançar a NF de retorno.</span>
        </div>
      )}

      {n.key === 'return_nf' && <NfForm asset={asset} kind="retorno" />}
    </section>
  );
}
