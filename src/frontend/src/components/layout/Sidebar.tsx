import type { LucideIcon } from 'lucide-react';
import {
  Route, Workflow, PlusCircle, CalendarDays, Map as MapIcon, Inbox, CalendarClock, Search, Hourglass, Building2, Truck,
  LayoutDashboard, Package, Bell, MapPin, FileText, RadioTower, ChartNoAxesCombined, X,
} from 'lucide-react';
import { NavLink, useLocation } from 'react-router-dom';
import { isMqttMode, isPhysicalEsp32 } from '@/services/runtime';
import { PersonaSwitcher, useCurrentPersona } from '@/portfolio/ui';
import { personaLabel, personaName } from '@/portfolio/logic';
import { SourceToggle } from './SourceToggle';
import type { Persona } from '@/portfolio/types';

type Item = { to: string; label: string; icon: LucideIcon; end?: boolean; short?: string };

/** Cada pessoa tem o próprio menu: o PCM não vê as telas da Logística e vice-versa. */
const byPersona: Record<Persona, Item[]> = {
  pcm: [
    { to: '/', label: 'Painel', icon: Workflow, end: true },
    { to: '/ativos/novo', label: 'Novo envio', icon: PlusCircle, short: 'Novo' },
    { to: '/ativos', label: 'Trajetórias', icon: Route, end: true },
    { to: '/prazos', label: 'Calendário de prazos', icon: CalendarDays, short: 'Prazos' },
    { to: '/onde-estao', label: 'Onde estão', icon: MapIcon, short: 'Mapa' },
  ],
  fiscal: [
    { to: '/fiscal', label: 'Pendências', icon: Inbox, end: true },
    { to: '/fiscal/prazos', label: 'Prazos fiscais', icon: CalendarClock, short: 'Prazos' },
    { to: '/fiscal/consulta', label: 'Consultar ativo', icon: Search, short: 'Consultar' },
  ],
  logistica: [
    { to: '/logistica', label: 'Gargalos', icon: Hourglass, end: true },
    { to: '/logistica/transporte', label: 'Transporte', icon: Truck },
    { to: '/logistica/oficinas', label: 'Oficinas', icon: Building2 },
  ],
};

const tag: Item[] = [
  { to: '/telemetria', label: 'Visão da TAG', icon: LayoutDashboard },
  { to: '/equipamentos', label: 'Equipamentos', icon: Package },
  { to: '/alertas', label: 'Alertas', icon: Bell },
  { to: '/mapa', label: 'Mapa da TAG', icon: MapPin },
  { to: '/ciclos', label: 'Ciclos e retornos', icon: FileText },
  { to: '/dispositivos', label: 'Dispositivos', icon: RadioTower },
  { to: '/relatorios', label: 'Relatórios', icon: ChartNoAxesCombined },
];

export function Sidebar({ open, onClose }: { open: boolean; onClose: () => void }) {
  const persona = useCurrentPersona();
  const path = useLocation().pathname;
  const onTag = tag.some(i => path.startsWith(i.to));
  const environment = isMqttMode ? (isPhysicalEsp32 ? 'MQTT · ESP32 físico' : 'MQTT · simulador') : 'Ambiente de demonstração';
  const link = ({ to, label, icon: Icon, end }: Item) => (
    <NavLink key={to} to={to} end={end} onClick={onClose} className={({ isActive }) => `nav-link ${isActive ? 'active' : ''}`}>
      <Icon size={19} aria-hidden="true" />
      <span>{label}</span>
    </NavLink>
  );
  return (
    <aside id="sidebar" className={`sidebar ${open ? 'sidebar-open' : ''}`} aria-label="Navegação do painel">
      <div className="brand">
        <NavLink to="/" onClick={onClose}><img src="/logo-guaratracking.svg" alt="Guarátracking" width="190" /></NavLink>
        <button className="icon-btn mobile-close" onClick={onClose} aria-label="Fechar menu"><X size={20} /></button>
        <p>Manutenção externa</p>
      </div>
      <nav aria-label="Menu principal">
        <div className="nav-group">
          <p>{personaName[persona].toUpperCase()} · {personaLabel[persona].toUpperCase()}</p>
          {byPersona[persona].map(link)}
        </div>
        <details className="nav-group nav-tag" open={onTag}>
          <summary>TAG E DISPOSITIVOS</summary>
          {tag.filter(item => !isMqttMode || item.to !== '/relatorios').map(link)}
        </details>
      </nav>
      <div className="sidebar-persona"><p>VER COMO</p><PersonaSwitcher /></div>
      <div className="sidebar-source"><p>FONTE DOS DADOS</p><SourceToggle /></div>
      <div className="sidebar-footer"><span className="small-dot" /> {environment}<br /><span className="muted">Frontend · v1.0</span></div>
    </aside>
  );
}

/** No celular, os botões principais da pessoa ficam embaixo, ao alcance do polegar. */
export function BottomNav() {
  const persona = useCurrentPersona();
  return (
    <nav className="bottom-bar" aria-label="Atalhos">
      {byPersona[persona].map(({ to, label, short, icon: Icon, end }) => (
        <NavLink key={to} to={to} end={end} className={({ isActive }) => `bottom-item ${isActive ? 'on' : ''} ${to === '/ativos/novo' ? 'primary' : ''}`}>
          <Icon size={22} aria-hidden="true" />
          <span>{short ?? label}</span>
        </NavLink>
      ))}
    </nav>
  );
}
