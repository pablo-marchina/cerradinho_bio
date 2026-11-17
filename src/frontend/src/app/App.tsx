import { Routes,Route,Link } from 'react-router-dom';
import { AppShell } from '@/components/layout/AppShell';
import { Dashboard } from '@/features/Dashboard';
import { Equipment,EquipmentDetail } from '@/features/Equipment';
import { Alerts } from '@/features/Alerts';
import { Positions,Cycles,Devices,Reports } from '@/features/Other';
export function App(){return <Routes><Route element={<AppShell/>}><Route index element={<Dashboard/>}/><Route path="equipamentos" element={<Equipment/>}/><Route path="equipamentos/:id" element={<EquipmentDetail/>}/><Route path="alertas" element={<Alerts/>}/><Route path="mapa" element={<Positions/>}/><Route path="ciclos" element={<Cycles/>}/><Route path="dispositivos" element={<Devices/>}/><Route path="relatorios" element={<Reports/>}/><Route path="*" element={<><h1>Página não encontrada</h1><Link to="/">Voltar à visão geral</Link></>}/></Route></Routes>;}
