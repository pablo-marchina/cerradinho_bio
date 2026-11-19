import {useEffect,useRef,useState} from 'react';
import {Menu,Search,FlaskConical,Radio} from 'lucide-react';
import {Outlet,useNavigate,useLocation} from 'react-router-dom';
import {useSnapshot} from '@/services/useSnapshot';
import {formatDate} from '@/services/domain';
import {isMqttMode,isPhysicalEsp32,mqttRuntime} from '@/services/runtime';
import {Sidebar} from './Sidebar';
import {PersonaSwitcher} from '@/portfolio/ui';
import {PortfolioLed} from '@/portfolio/Guara';
import {SourceToggle} from './SourceToggle';

export function AppShell(){
  const q=useSnapshot(),[open,setOpen]=useState(false),[search,setSearch]=useState('');
  const nav=useNavigate(),location=useLocation(),trigger=useRef<HTMLButtonElement>(null);
  useEffect(()=>{setOpen(false);},[location.pathname]);
  useEffect(()=>{if(!open)return;const cb=(event:KeyboardEvent)=>{if(event.key==='Escape'){setOpen(false);trigger.current?.focus();}};window.addEventListener('keydown',cb);return()=>window.removeEventListener('keydown',cb);},[open]);
  const ModeIcon=isMqttMode?Radio:FlaskConical;
  return <div className="app-shell"><a className="skip" href="#main-content">Ir para conteúdo</a>{open&&<button className="drawer-overlay" aria-label="Fechar menu" onClick={()=>{setOpen(false);trigger.current?.focus();}}/>}<Sidebar open={open} onClose={()=>setOpen(false)}/><div className="workspace"><header className="topbar"><button ref={trigger} className="icon-btn menu-toggle" aria-label="Abrir menu" aria-expanded={open} aria-controls="sidebar" onClick={()=>setOpen(value=>!value)}><Menu size={22}/></button><form role="search" className="top-search" onSubmit={event=>{event.preventDefault();nav(`/equipamentos?q=${encodeURIComponent(search)}`);}}><Search size={18} aria-hidden="true"/><input value={search} onChange={event=>setSearch(event.target.value)} placeholder="Buscar ativo, TAG ou NF" aria-label="Buscar equipamento"/></form><PortfolioLed/><PersonaSwitcher/><div className="top-meta"><SourceToggle/><div className="avatar" aria-label="Perfil demonstrativo">GT</div></div></header><main id="main-content" tabIndex={-1} className="content">{q.data&&!(location.pathname==='/'||/^\/(fiscal|logistica|ativos|prazos|onde-estao)/.test(location.pathname))&&<p className="small muted">{isMqttMode?'Horário da última atualização do painel':'Relógio do cenário'}: {formatDate(q.data.asOf)} · Brasília (UTC−3){!isMqttMode&&' · Alterações locais, perdidas ao recarregar.'}</p>}{q.isRefetchError&&<div role="alert" className="critical-strip">Falha ao atualizar; dados anteriormente carregados podem estar desatualizados. <button className="btn btn-secondary" onClick={()=>void q.refetch()}>Tentar novamente</button></div>}<Outlet/></main></div><div className="bottom-bar"><PersonaSwitcher/></div></div>;
}
