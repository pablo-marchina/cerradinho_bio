import {LayoutDashboard,Package,Bell,MapPin,FileText,RadioTower,ChartNoAxesCombined,X} from 'lucide-react';
import {NavLink} from 'react-router-dom';
import {isMqttMode,isPhysicalEsp32} from '@/services/runtime';

const groups=[{title:'ACOMPANHAMENTO',items:[{to:'/',label:'Visão geral',icon:LayoutDashboard},{to:'/equipamentos',label:'Equipamentos',icon:Package},{to:'/alertas',label:'Alertas',icon:Bell},{to:'/mapa',label:'Mapa',icon:MapPin}]},{title:'OPERAÇÃO',items:[{to:'/ciclos',label:'Ciclos e retornos',icon:FileText},{to:'/dispositivos',label:'Dispositivos',icon:RadioTower},{to:'/relatorios',label:'Relatórios',icon:ChartNoAxesCombined}]}];

export function Sidebar({open,onClose}:{open:boolean;onClose:()=>void}){
  const environment=isMqttMode?(isPhysicalEsp32?'MQTT · ESP32 físico':'MQTT · simulador'):'Ambiente de demonstração';
  return <aside id="sidebar" className={`sidebar ${open?'sidebar-open':''}`} aria-label="Navegação do painel"><div className="brand"><NavLink to="/" onClick={onClose}><img src="/logo-guara.png" alt="Guarátracking"/></NavLink><button className="icon-btn mobile-close" onClick={onClose} aria-label="Fechar menu"><X size={20}/></button><p>Manutenção externa</p></div><nav aria-label="Menu principal">{groups.map(group=><div className="nav-group" key={group.title}><p>{group.title}</p>{group.items.filter(item=>!isMqttMode||item.to!=='/relatorios').map(({to,label,icon:Icon})=><NavLink key={to} to={to} end={to==='/'} onClick={onClose} className={({isActive})=>`nav-link ${isActive?'active':''}`}><Icon size={19} aria-hidden="true"/><span>{label}</span></NavLink>)}</div>)}</nav><div className="sidebar-footer"><span className="small-dot"/> {environment}<br/><span className="muted">Frontend · v1.0</span></div></aside>;
}
