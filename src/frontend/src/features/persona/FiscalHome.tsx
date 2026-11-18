import { useMemo, useState } from 'react';
import { Link } from 'react-router-dom';
import { FileText, Factory, CalendarClock, Landmark, Search, CircleCheck, CircleAlert, ChevronDown } from 'lucide-react';
import { usePortfolio } from '@/portfolio/store';
import { AssetThumb, StageLabel, Figure } from '@/portfolio/ui';
import { ago, cfopFor, dateBr, icmsAtRisk, isOpen, legalLimitDays, legalRemaining, money, moneyShort, supplierOf } from '@/portfolio/logic';
import { NfForm } from './NfForm';
import { DeadlineCalendar } from '@/portfolio/Calendar';
import type { PortfolioAsset } from '@/portfolio/types';

function useFiscal() {
  const { state } = usePortfolio();
  const { now, suppliers } = state;
  const open = state.assets.filter(isOpen);
  const toIssue = open.filter(a => a.stage === 'aguardando_nf').sort((a, b) => Date.parse(a.nfRequestedAt ?? now) - Date.parse(b.nfRequestedAt ?? now));
  const noReturn = open.filter(a => a.stage === 'na_usina').sort((a, b) => Date.parse(a.arrivedPlantAt ?? now) - Date.parse(b.arrivedPlantAt ?? now));
  const legal = open
    .map(a => ({ a, r: legalRemaining(a, now, suppliers) }))
    .filter((x): x is { a: PortfolioAsset; r: number } => x.r !== null)
    .sort((x, y) => x.r - y.r);
  return { state, now, suppliers, open, toIssue, noReturn, legal, icms: open.reduce((s, a) => s + icmsAtRisk(a), 0) };
}

/** Pendências da Clarice: duas filas separadas em abas, uma de cada vez. */
export function FiscalHome() {
  const { now, suppliers, toIssue, noReturn, legal } = useFiscal();
  const [tab, setTab] = useState<'emitir' | 'baixa'>(toIssue.length || !noReturn.length ? 'emitir' : 'baixa');
  const [expanded, setExpanded] = useState<string | null>(null);
  const list = tab === 'emitir' ? toIssue : noReturn;

  return (
    <div className="pp">
      <header className="pp-hero">
        <div>
          <p className="eyebrow">Pendências do Fiscal</p>
          <h1>{toIssue.length + noReturn.length} notas esperando por você</h1>
          <p className="lead muted">Os ativos que voltam para a usina entram aqui assim que a TAG passa pela portaria, sem depender de aviso do PCM.</p>
        </div>
      </header>

      <div className="tabbed">
      <div className="tabs" role="tablist" aria-label="Filas do Fiscal">
        <button type="button" role="tab" aria-selected={tab === 'emitir'} className={tab === 'emitir' ? 'on' : ''} onClick={() => { setTab('emitir'); setExpanded(null); }}>
          <span className="tab-icon"><FileText size={22} /></span>
          <span className="tab-text"><b>NF de remessa a emitir</b><span className="muted">Pedidos do PCM, mais antigos primeiro</span></span>
          <span className="tab-count mono t-warn">{toIssue.length}</span>
        </button>
        <button type="button" role="tab" aria-selected={tab === 'baixa'} className={tab === 'baixa' ? 'on' : ''} onClick={() => { setTab('baixa'); setExpanded(null); }}>
          <span className="tab-icon"><Factory size={22} /></span>
          <span className="tab-text"><b>Voltaram sem NF de retorno</b><span className="muted">Lance a NF para encerrar o ciclo</span></span>
          <span className="tab-count mono t-alert">{noReturn.length}</span>
        </button>
      </div>

      <section className="pp-panel" role="tabpanel" aria-label={tab === 'emitir' ? 'NF de remessa a emitir' : 'Voltaram sem NF de retorno'}>
        {!list.length && <p className="muted empty">Nada pendente nesta fila.</p>}
        <ul className="fiscal-list">
          {list.map(a => {
            const s = supplierOf(a, suppliers);
            return (
              <li key={a.id}>
                <button type="button" className="fiscal-row" onClick={() => setExpanded(expanded === a.id ? null : a.id)} aria-expanded={expanded === a.id}>
                  <AssetThumb asset={a} size={44} />
                  <span className="grow">
                    <b>{a.name}</b>
                    <span className="muted mono">{a.id} · {tab === 'emitir' ? a.om : a.remittance?.number}</span>
                  </span>
                  {tab === 'emitir' ? (
                    <span className="fiscal-meta"><span>{s?.name} · {s?.uf}</span><span className="muted">CFOP {cfopFor('remessa', s)} · pedido {ago(a.nfRequestedAt, now)}</span></span>
                  ) : (
                    <span className="fiscal-meta"><span className="t-alert">Chegou {ago(a.arrivedPlantAt, now)}</span><span className="muted">{dateBr(a.arrivedPlantAt)}</span></span>
                  )}
                  <span className="fiscal-cta">{tab === 'emitir' ? 'Vincular NF' : 'Lançar retorno'} <ChevronDown size={18} className={expanded === a.id ? 'rot' : ''} /></span>
                </button>
                {expanded === a.id && <NfForm asset={a} kind={tab === 'emitir' ? 'remessa' : 'retorno'} onDone={() => setExpanded(null)} />}
              </li>
            );
          })}
        </ul>
      </section>
      </div>

      {legal.some(x => x.r <= 30) && (
        <Link to="/fiscal/prazos" className="nudge">
          <CalendarClock size={20} /> {legal.filter(x => x.r <= 30).length} remessas chegam ao limite fiscal em até 30 dias. Ver prazos fiscais
        </Link>
      )}
    </div>
  );
}

/** Prazos fiscais: limite da suspensão do ICMS de cada remessa aberta. */
export function FiscalPrazos() {
  const { suppliers, legal, icms } = useFiscal();
  return (
    <div className="pp">
      <header className="pp-hero">
        <div>
          <p className="eyebrow">Prazos fiscais</p>
          <h1>Limite da suspensão do ICMS</h1>
          <p className="lead muted">270 dias para oficinas em GO e 360 para as de fora, contados da emissão da NF de remessa.</p>
        </div>
      </header>
      <section className="figs" aria-label="Resumo dos prazos">
        <Figure icon={CalendarClock} label="Vencem em até 30 dias" value={legal.filter(x => x.r <= 30).length} tone="alert" />
        <Figure icon={CalendarClock} label="Vencem em até 90 dias" value={legal.filter(x => x.r <= 90).length} tone="warn" />
        <Figure icon={Landmark} label="ICMS suspenso (estimado)" value={moneyShort(icms)} />
      </section>
      <section className="pp-section" aria-labelledby="legal">
        <div className="pp-head"><h2 id="legal">Os 10 mais próximos de vencer</h2></div>
        <ul className="legal-list">
          {legal.slice(0, 10).map(({ a, r }) => {
            const lim = legalLimitDays(supplierOf(a, suppliers));
            return (
              <li key={a.id}>
                <Link to={`/ativos/${a.id}`}>
                  <span className="legal-name"><b>{a.name}</b><span className="muted mono">{a.id} · {money(icmsAtRisk(a))}</span></span>
                  <span className="legal-bar"><span className={r <= 30 ? 'late' : r <= 90 ? 'soon' : ''} style={{ width: `${Math.min(100, ((lim - r) / lim) * 100)}%` }} /></span>
                  <span className={`mono ${r <= 30 ? 't-alert' : ''}`}>{r} d</span>
                </Link>
              </li>
            );
          })}
        </ul>
      </section>
      <section className="pp-section" aria-labelledby="calf">
        <div className="pp-head"><h2 id="calf">Calendário dos limites</h2></div>
        <DeadlineCalendar kinds={['fiscal']} />
      </section>
    </div>
  );
}

/** Consulta: "esse ativo já voltou?" e o que a NF de remessa leva. */
export function FiscalConsulta() {
  const { state } = usePortfolio();
  const [q, setQ] = useState('');
  const hits = useMemo(() => {
    const t = q.trim().toLowerCase();
    if (t.length < 2) return [];
    return state.assets.filter(a => [a.name, a.id, a.tag ?? '', a.om, a.remittance?.number ?? ''].some(v => v.toLowerCase().includes(t))).slice(0, 6);
  }, [q, state.assets]);
  return (
    <div className="pp">
      <header className="pp-hero">
        <div>
          <p className="eyebrow">Consultar ativo</p>
          <h1>Esse ativo já voltou?</h1>
          <p className="lead muted">Busque pela NF, pela TAG, pela OM ou pelo código do ativo.</p>
        </div>
      </header>
      <section className="pp-panel">
        <label className="search-box big">
          <Search size={22} aria-hidden="true" />
          <input className="input big" autoFocus value={q} onChange={e => setQ(e.target.value.toUpperCase())} placeholder="Ex.: NF-010884, GT-0042 ou OM-4500185" aria-label="Buscar ativo por NF, TAG, OM ou código" />
        </label>
        <ul className="lookup">
          {hits.map(a => {
            const back = ['na_usina', 'encerrado'].includes(a.stage);
            return (
              <li key={a.id}>
                <Link to={`/ativos/${a.id}`}>
                  {back ? <CircleCheck className="ok" size={28} /> : <CircleAlert className="warn" size={28} />}
                  <span className="grow">
                    <b>{back ? 'Sim, está na usina' : 'Não, ainda está fora'}</b>
                    <span className="muted">{a.name} · {a.id} · {a.remittance?.number ?? 'sem NF'}</span>
                  </span>
                  <span>{back ? (a.stage === 'na_usina' ? <span className="t-warn">Falta a NF de retorno</span> : <span className="t-ok">Ciclo encerrado</span>) : <StageLabel stage={a.stage} />}</span>
                </Link>
              </li>
            );
          })}
          {q.trim().length >= 2 && !hits.length && <li className="muted">Nada encontrado.</li>}
        </ul>
      </section>
      <section className="pp-section" aria-labelledby="nfguia">
        <div className="pp-head">
          <h2 id="nfguia">O que a NF de remessa para conserto leva</h2>
          <p className="muted">Levantamento para validar com o Fiscal da Cerradinho. A emissão e a assinatura continuam no sistema fiscal; o GuaráTracking só guarda o vínculo com o ativo.</p>
        </div>
        <ul className="nf-guide">
          <li><b>Natureza da operação:</b> remessa para conserto ou reparo.</li>
          <li><b>CFOP:</b> 5.915 dentro do estado e 6.915 para outro estado. No retorno, 5.916 ou 6.916.</li>
          <li><b>Destinatário:</b> razão social, CNPJ, IE e endereço da oficina. A TAG usa esse endereço como referência.</li>
          <li><b>Item:</b> descrição, NCM, quantidade e valor do bem.</li>
          <li><b>ICMS:</b> suspenso, com o fundamento legal e o prazo de retorno nas informações complementares.</li>
          <li><b>Assinatura e autorização:</b> certificado digital da empresa e protocolo de autorização da SEFAZ.</li>
          <li><b>DANFE:</b> impressa, segue junto com o ativo até a oficina.</li>
        </ul>
      </section>
    </div>
  );
}
