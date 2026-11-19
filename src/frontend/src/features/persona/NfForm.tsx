import { useState } from 'react';
import { cfopFor, normalizeNf, supplierOf, validNf } from '@/portfolio/logic';
import { makeInvoice, usePortfolio } from '@/portfolio/store';
import type { PortfolioAsset } from '@/portfolio/types';

/**
 * Registro da NF emitida no sistema fiscal (SAP/Gama Soft). O GuaráTracking não emite NF-e:
 * a Clarice emite e assina no sistema fiscal e aqui só vincula o número ao ativo.
 */
export function NfForm({ asset, kind, onDone }: { asset: PortfolioAsset; kind: 'remessa' | 'retorno'; onDone?: () => void }) {
  const { state, dispatch } = usePortfolio();
  const supplier = supplierOf(asset, state.suppliers);
  const [number, setNumber] = useState('');
  const [value, setValue] = useState(String(asset.remittance?.value ?? ''));
  const [key, setKey] = useState('');
  const [danfe, setDanfe] = useState(false);
  const [error, setError] = useState('');
  const cfop = cfopFor(kind, supplier);

  const submit = (e: React.FormEvent) => {
    e.preventDefault();
    if (!validNf(number)) return setError('Informe o número da NF: letras, números, hífen, ponto ou barra, até 15 caracteres.');
    const v = Number(value);
    if (!Number.isFinite(v) || v <= 0) return setError('Informe o valor do bem que consta na NF.');
    if (key && !/^\d{44}$/.test(key)) return setError('A chave de acesso tem 44 dígitos.');
    if (kind === 'remessa' && !danfe) return setError('Confirme que a DANFE foi impressa para acompanhar o ativo.');
    const invoice = makeInvoice(kind, asset, state, number, v, key ? { accessKey: key } : {});
    dispatch(kind === 'remessa' ? { type: 'issue_nf', id: asset.id, invoice } : { type: 'return_nf', id: asset.id, invoice });
    onDone?.();
  };

  return (
    <form className="gt-form nf-form" onSubmit={submit} noValidate>
      <div className="form-grid">
        <label>
          Número da NF de {kind}
          <input
            className="input mono"
            value={number}
            onChange={e => setNumber(normalizeNf(e.target.value))}
            placeholder="NF-000123"
            autoCapitalize="characters"
            aria-describedby="nf-hint"
          />
          <span id="nf-hint" className="small muted">Letras viram maiúsculas automaticamente.</span>
        </label>
        <label>
          Valor do bem (R$)
          <input className="input mono" inputMode="decimal" value={value} onChange={e => setValue(e.target.value.replace(/[^\d.]/g, ''))} />
        </label>
        <label>
          CFOP
          <input className="input mono" value={cfop} readOnly aria-describedby="cfop-hint" />
          <span id="cfop-hint" className="small muted">
            {cfop.startsWith('6') ? 'Oficina fora de GO: operação interestadual.' : 'Oficina em GO: operação interna.'} Prazo legal {supplier?.uf === 'GO' ? '270' : '360'} dias.
          </span>
        </label>
        <label>
          Chave de acesso (opcional)
          <input className="input mono" value={key} onChange={e => setKey(e.target.value.replace(/\D/g, '').slice(0, 44))} placeholder="44 dígitos" />
        </label>
      </div>
      {kind === 'remessa' && (
        <label className="check">
          <input type="checkbox" checked={danfe} onChange={e => setDanfe(e.target.checked)} /> DANFE impressa e entregue ao PCM para seguir com o ativo
        </label>
      )}
      {error && <p role="alert" className="form-error">{error}</p>}
      <div className="row-gap">
        <button className="btn btn-primary" type="submit">
          {kind === 'remessa' ? 'Vincular NF de remessa ao ativo' : 'Lançar NF de retorno e encerrar'}
        </button>
        <span className="small muted">A NF-e é emitida e assinada no sistema fiscal; aqui só fica o vínculo com o ativo.</span>
      </div>
    </form>
  );
}
