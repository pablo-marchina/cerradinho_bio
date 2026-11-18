import { useMemo, useState } from 'react';
import { Link } from 'react-router-dom';
import { FileText, Factory, CalendarClock, Landmark, Search, CircleCheck, CircleAlert, ChevronDown } from 'lucide-react';
import { usePortfolio } from '@/portfolio/store';
import { AssetThumb, StageLabel, ProcessRail, Figure } from '@/portfolio/ui';
import { ago, cfopFor, dateBr, icmsAtRisk, isOpen, legalLimitDays, legalRemaining, money, moneyShort, stageCounts, supplierOf } from '@/portfolio/logic';
import { NfForm } from './NfForm';
import { DeadlineCalendar } from '@/portfolio/Calendar';
import type { PortfolioAsset } from '@/portfolio/types';

export function FiscalHome() {
  const { state } = usePortfolio();
  const { now, suppliers } = state;
  const open = state.assets.filter(isOpen);
  const toIssue = open.filter(a => a.stage === 'aguardando_nf').sort((a, b) => Date.parse(a.nfRequestedAt ?? now) - Date.parse(b.nfRequestedAt ?? now));
  const noReturn = open.filter(a => a.stage === 'na_usina').sort((a, b) => Date.parse(a.arrivedPlantAt ?? now) - Date.parse(b.arrivedPlantAt ?? now));
  const legal = open
    .map(a => ({ a, r: legalRemaining(a, now, suppliers) }))
    .filter((x): x is { a: PortfolioAsset; r: number } => x.r !== null)
    .sort((x, y) => x.r - y.r);
  const icms = open.reduce((s, a) => s + icmsAtRisk(a), 0);
  const [open1, setOpen1] = useState<string | null>(null);

  return (
    <div className="pp">
      <header className="pp-hero">
        <div>
          <p className="eyebrow">Visão do Fiscal · Clarice</p>
          <h1>{toIssue.length} NFs para emitir e {noReturn.length} retornos sem baixa</h1>
          <p className="muted">Os ativos que já voltaram para a usina aparecem aqui assim que a TAG passa pela portaria, sem depender de aviso do PCM.</p>
        </div>
      </header>

      <section className="figs" aria-label="Pendências fiscais">
        <Figure label="NFs de remessa a emitir" value={toIssue.length} tone="warn" />
        <Figure label="Voltaram sem NF de retorno" value={noReturn.length} tone="alert" />
        <Figure label="Prazo legal em até 30 dias" value={legal.filter(x => x.r <= 30).length} tone="alert" />
        <Figure label="ICMS suspenso (estimado)" value={moneyShort(icms)}  />
      </section>

      <div className="split">
        <section className="pp-section" aria-labelledby="emitir">
          <div className="pp-head row-between"><h2 id="emitir">NFs de remessa a emitir</h2><span className="muted small">Mais antigas primeiro</span></div>
          {!toIssue.length && <p className="muted empty">Nenhum pedido de NF pendente.</p>}
          <ul className="fiscal-list">
            {toIssue.map(a => {
              const s = supplierOf(a, suppliers);
              return (
                <li key={a.id}>
                  <button type="button" className="fiscal-row" onClick={() => setOpen1(open1 === a.id ? null : a.id)} aria-expanded={open1 === a.id}>
                    <AssetThumb asset={a} size={36} />
                    <span className="grow">
                      <b>{a.name}</b>
                      <span className="small muted mono">{a.id} · {a.om} · pedido {ago(a.nfRequestedAt, now)}</span>
                      <span className="small">{s?.name} · {s?.city}/{s?.uf} · CFOP {cfopFor('remessa', s)} · prazo legal {legalLimitDays(s)} d</span>
                    </span>
                    <ChevronDown size={18} className={open1 === a.id ? 'rot' : ''} />
                  </button>
                  {open1 === a.id && <NfForm asset={a} kind="remessa" onDone={() => setOpen1(null)} />}
                </li>
              );
            })}
          </ul>
        </section>

        <section className="pp-section" aria-labelledby="baixa">
          <div className="pp-head row-between"><h2 id="baixa">Voltaram para a usina sem baixa</h2><span className="muted small">Lance a NF de retorno para encerrar</span></div>
          {!noReturn.length && <p className="muted empty">Nenhum retorno pendente.</p>}
          <ul className="fiscal-list">
            {noReturn.map(a => (
              <li key={a.id}>
                <button type="button" className="fiscal-row" onClick={() => setOpen1(open1 === a.id ? null : a.id)} aria-expanded={open1 === a.id}>
                  <AssetThumb asset={a} size={36} />
                  <span className="grow">
                    <b>{a.name}</b>
                    <span className="small muted mono">{a.id} · {a.remittance?.number}</span>
                    <span className="small text-danger">Chegou {ago(a.arrivedPlantAt, now)} ({dateBr(a.arrivedPlantAt)})</span>
                  </span>
                  <ChevronDown size={18} className={open1 === a.id ? 'rot' : ''} />
                </button>
                {open1 === a.id && <NfForm asset={a} kind="retorno" onDone={() => setOpen1(null)} />}
              </li>
            ))}
          </ul>
        </section>
      </div>

      <div className="split">
        <section className="pp-section" aria-labelledby="legal">
          <div className="pp-head row-between"><h2 id="legal">Prazo legal da suspensão do ICMS</h2><span className="muted small">Os 10 mais próximos de vencer</span></div>
          <ul className="legal-list">
            {legal.slice(0, 10).map(({ a, r }) => {
              const lim = legalLimitDays(supplierOf(a, suppliers));
              return (
                <li key={a.id}>
                  <Link to={`/ativos/${a.id}`}>
                    <span className="legal-name"><b>{a.name}</b><span className="small muted mono">{a.id} · {money(icmsAtRisk(a))}</span></span>
                    <span className="legal-bar"><span className={r <= 30 ? 'late' : r <= 90 ? 'soon' : ''} style={{ width: `${Math.min(100, ((lim - r) / lim) * 100)}%` }} /></span>
                    <span className={`mono small ${r <= 30 ? 'text-danger' : ''}`}>{r} d</span>
                  </Link>
                </li>
              );
            })}
          </ul>
        </section>
        <ReturnLookup />
      </div>

      <section className="pp-section" aria-labelledby="pipe-f">
        <div className="pp-head row-between"><h2 id="pipe-f">Panorama do processo</h2></div>
        <ProcessRail counts={stageCounts(open)} hide={['encerrado']} />
      </section>

      <section className="pp-section" aria-labelledby="calf">
        <div className="pp-head"><h2 id="calf">Calendário dos limites fiscais</h2><p className="muted">Quando vence a suspensão do ICMS de cada remessa aberta.</p></div>
        <DeadlineCalendar kinds={['fiscal']} />
      </section>

      <section className="pp-section" aria-labelledby="nfguia">
        <h2 id="nfguia">O que a NF de remessa para conserto leva</h2>
        <p className="small muted">Levantamento para validar com o Fiscal da Cerradinho. A emissão e a assinatura continuam no sistema fiscal; o GuaráTracking só guarda o vínculo com o ativo.</p>
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

function ReturnLookup() {
  const { state } = usePortfolio();
  const [q, setQ] = useState('');
  const hits = useMemo(() => {
    const t = q.trim().toLowerCase();
    if (t.length < 2) return [];
    return state.assets.filter(a => [a.name, a.id, a.tag ?? '', a.om, a.remittance?.number ?? ''].some(v => v.toLowerCase().includes(t))).slice(0, 5);
  }, [q, state.assets]);
  return (
    <section className="pp-section" aria-labelledby="voltou">
      <div className="pp-head row-between"><h2 id="voltou">Esse ativo já voltou?</h2></div>
      <label className="search-box">
        <Search size={18} aria-hidden="true" />
        <input className="input" value={q} onChange={e => setQ(e.target.value.toUpperCase())} placeholder="NF, TAG, OM ou código do ativo" aria-label="Buscar ativo por NF, TAG ou OM" />
      </label>
      <ul className="lookup">
        {hits.map(a => {
          const back = ['na_usina', 'encerrado'].includes(a.stage);
          return (
            <li key={a.id}>
              <Link to={`/ativos/${a.id}`}>
                {back ? <CircleCheck className="ok" size={22} /> : <CircleAlert className="warn" size={22} />}
                <span className="grow">
                  <b>{back ? 'Sim, está na usina' : 'Não, ainda está fora'}</b>
                  <span className="small muted">{a.name} · {a.id} · {a.remittance?.number ?? 'sem NF'}</span>
                  <span className="small">{back ? `Chegou em ${dateBr(a.arrivedPlantAt)}${a.stage === 'na_usina' ? ' · falta a NF de retorno' : ' · ciclo encerrado'}` : <StageLabel stage={a.stage} />}</span>
                </span>
              </Link>
            </li>
          );
        })}
        {q.trim().length >= 2 && !hits.length && <li className="muted">Nada encontrado.</li>}
      </ul>
    </section>
  );
}
