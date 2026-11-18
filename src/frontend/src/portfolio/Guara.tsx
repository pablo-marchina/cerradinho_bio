import { Link } from 'react-router-dom';
import { usePortfolio } from './store';
import { isCritical, isOpen, isOverdue, isUrgent, ownerOf } from './logic';
import type { Persona } from './types';

// Guará-LED da marca (Guia de Estilos v2, seção 02): o mesmo desenho do logo, com os estados do LED físico.
const P = {
  legs: 'M16.60 54.09L16.60 74.40M28.55 54.09L28.55 61.25L33.32 66.03L33.32 74.40',
  body: 'M7.04 50.50L7.04 19.44C7.04 18.42 7.14 17.41 7.34 16.41C7.54 15.41 7.83 14.44 8.22 13.49C8.61 12.55 9.09 11.66 9.66 10.81C10.22 9.96 10.87 9.18 11.59 8.45C12.31 7.73 13.09 7.09 13.94 6.52C14.79 5.96 15.69 5.48 16.63 5.09C17.57 4.70 18.54 4.40 19.54 4.20C20.54 4.00 21.55 3.90 22.57 3.90C23.59 3.90 24.60 4.00 25.60 4.20C26.60 4.40 27.57 4.70 28.52 5.09C29.46 5.48 30.35 5.96 31.20 6.52C32.05 7.09 32.83 7.73 33.55 8.45C34.28 9.18 34.92 9.96 35.49 10.81C36.05 11.66 36.53 12.55 36.92 13.49C37.31 14.44 37.61 15.41 37.81 16.41C38.00 17.41 38.10 18.42 38.10 19.44L38.10 50.50L7.04 50.50Z',
  base: 'M5.84 48.11L39.30 48.11C39.62 48.11 39.92 48.17 40.21 48.29C40.51 48.41 40.76 48.59 40.99 48.81C41.21 49.04 41.38 49.29 41.51 49.59C41.63 49.88 41.69 50.18 41.69 50.50L41.69 52.29C41.69 52.61 41.63 52.91 41.51 53.21C41.38 53.50 41.21 53.76 40.99 53.98C40.76 54.21 40.51 54.38 40.21 54.50C39.92 54.62 39.62 54.68 39.30 54.68L5.84 54.68C5.53 54.68 5.22 54.62 4.93 54.50C4.64 54.38 4.38 54.21 4.16 53.98C3.93 53.76 3.76 53.50 3.64 53.21C3.52 52.91 3.46 52.61 3.46 52.29L3.46 50.50C3.46 50.18 3.52 49.88 3.64 49.59C3.76 49.29 3.93 49.04 4.16 48.81C4.38 48.59 4.64 48.41 4.93 48.29C5.22 48.17 5.53 48.11 5.84 48.11Z',
  beak: 'M34.52 15.85C47.66 16.45 55.43 30.19 54.83 49.31C51.84 33.77 45.87 26.01 35.71 24.81L34.52 15.85Z',
  eyeL: 'M20.90 15.85C20.90 16.19 20.84 16.51 20.71 16.81C20.58 17.12 20.40 17.39 20.16 17.63C19.93 17.86 19.66 18.04 19.35 18.17C19.04 18.30 18.72 18.36 18.39 18.36C18.06 18.36 17.74 18.30 17.43 18.17C17.12 18.04 16.85 17.86 16.62 17.63C16.38 17.39 16.20 17.12 16.07 16.81C15.94 16.51 15.88 16.19 15.88 15.85C15.88 15.52 15.94 15.20 16.07 14.89C16.20 14.59 16.38 14.31 16.62 14.08C16.85 13.84 17.12 13.66 17.43 13.53C17.74 13.41 18.06 13.34 18.39 13.34C18.72 13.34 19.04 13.41 19.35 13.53C19.66 13.66 19.93 13.84 20.16 14.08C20.40 14.31 20.58 14.59 20.71 14.89C20.84 15.20 20.90 15.52 20.90 15.85Z',
  eyeR: 'M30.46 15.85C30.46 16.19 30.39 16.51 30.27 16.81C30.14 17.12 29.96 17.39 29.72 17.63C29.49 17.86 29.22 18.04 28.91 18.17C28.60 18.30 28.28 18.36 27.95 18.36C27.62 18.36 27.30 18.30 26.99 18.17C26.68 18.04 26.41 17.86 26.17 17.63C25.94 17.39 25.76 17.12 25.63 16.81C25.50 16.51 25.44 16.19 25.44 15.85C25.44 15.52 25.50 15.20 25.63 14.89C25.76 14.59 25.94 14.31 26.17 14.08C26.41 13.84 26.68 13.66 26.99 13.53C27.30 13.41 27.62 13.34 27.95 13.34C28.28 13.34 28.60 13.41 28.91 13.53C29.22 13.66 29.49 13.84 29.72 14.08C29.96 14.31 30.14 14.59 30.27 14.89C30.39 15.20 30.46 15.52 30.46 15.85Z',
};

export type LedState = 'aceso' | 'transmitindo' | 'alerta' | 'desligado';

export function GuaraBird({ state = 'aceso', size = 40 }: { state?: LedState; size?: number }) {
  return (
    <svg className={`guara-led ${state}`} viewBox="0 -14 60 92" width={size * 0.65} height={size} aria-hidden="true">
      <g className="rays"><path d="M22.6 -3V-11" /><path d="M11 0L6 -6" /><path d="M34 0L39 -6" /></g>
      <path className="body" d={P.body} />
      <path className="body" d={P.base} />
      <path className="ink-s" d={P.legs} fill="none" strokeWidth="2.99" strokeLinecap="round" strokeLinejoin="round" />
      <path className="ink-f" d={P.beak} strokeWidth="2.39" strokeLinecap="round" strokeLinejoin="round" />
      <path className="ink-f eye" d={P.eyeL} />
      <path className="ink-f eye" d={P.eyeR} />
    </svg>
  );
}

/** Estado da carteira no topo: alerta se há crítico, transmitindo se algo vence ou venceu, aceso se tudo está em dia. */
export function PortfolioLed() {
  const { state } = usePortfolio();
  const open = state.assets.filter(isOpen);
  const crit = open.filter(a => isCritical(a, state.now, state.suppliers)).length;
  const soon = open.filter(a => isOverdue(a, state.now) || isUrgent(a, state.now, state.suppliers)).length;
  const s: LedState = crit ? 'alerta' : soon ? 'transmitindo' : 'aceso';
  const text = crit ? `Alerta · ${crit} críticos` : soon ? `Atenção · ${soon} perto do prazo` : 'Tudo em dia';
  return (
    <Link to="/" className={`led-status s-${s}`} title={text} aria-label={`Estado da carteira: ${text}`}>
      <GuaraBird state={s} size={34} />
      <span>{text}</span>
    </Link>
  );
}

/** Mesa limpa: nada esperando pela pessoa. Recompensa discreta, no tom do guia. */
export function CleanDesk({ who, text }: { who: Persona; text?: string }) {
  const { state } = usePortfolio();
  const others = state.assets.filter(a => ownerOf(a) && ownerOf(a) !== who).length;
  return (
    <div className="clean-desk" role="status">
      <GuaraBird state="aceso" size={72} />
      <div>
        <h3>Mesa limpa</h3>
        <p className="muted">{text ?? 'Nenhum ativo esperando por você.'} {others ? `Os outros ${others} estão com as demais áreas.` : ''}</p>
      </div>
    </div>
  );
}
