import {Routes,Route,Link} from 'react-router-dom';
import {AppShell} from '@/components/layout/AppShell';
import {Card} from '@/components/ui/primitives';
import {Dashboard} from '@/features/Dashboard';
import {Equipment,EquipmentDetail} from '@/features/Equipment';
import {Alerts} from '@/features/Alerts';
import {Positions,Cycles,Devices,Reports} from '@/features/Other';
import {isMqttMode} from '@/services/runtime';
import {PcmHome,PcmMapa,PcmPrazos} from '@/features/persona/PcmHome';
import {FiscalHome,FiscalPrazos,FiscalConsulta} from '@/features/persona/FiscalHome';
import {LogisticsHome,LogisticsOficinas,LogisticsTransporte} from '@/features/persona/LogisticsHome';
import {AssetDetail} from '@/features/persona/AssetDetail';
import {NewAsset} from '@/features/persona/NewAsset';
import {AssetsPage} from '@/features/persona/AssetsPage';

function MqttReports(){return <><h1>Relatórios indisponíveis no modo MQTT</h1><Card className="section-card"><p>O dispositivo não fornece fornecedor, responsável, prazo fiscal ou histórico persistente suficientes para calcular estes relatórios com integridade.</p><p className="muted">Use o modo demonstração para avaliar o layout dos relatórios. A versão integrada depende de backend e persistência.</p></Card></>}

export function App(){return <Routes><Route element={<AppShell/>}><Route index element={<PcmHome/>}/><Route path="prazos" element={<PcmPrazos/>}/><Route path="onde-estao" element={<PcmMapa/>}/><Route path="fiscal" element={<FiscalHome/>}/><Route path="fiscal/prazos" element={<FiscalPrazos/>}/><Route path="fiscal/consulta" element={<FiscalConsulta/>}/><Route path="logistica" element={<LogisticsHome/>}/><Route path="logistica/oficinas" element={<LogisticsOficinas/>}/><Route path="logistica/transporte" element={<LogisticsTransporte/>}/><Route path="ativos" element={<AssetsPage/>}/><Route path="ativos/novo" element={<NewAsset/>}/><Route path="ativos/:id" element={<AssetDetail/>}/><Route path="telemetria" element={<Dashboard/>}/><Route path="equipamentos" element={<Equipment/>}/><Route path="equipamentos/:id" element={<EquipmentDetail/>}/><Route path="alertas" element={<Alerts/>}/><Route path="mapa" element={<Positions/>}/><Route path="ciclos" element={<Cycles/>}/><Route path="dispositivos" element={<Devices/>}/><Route path="relatorios" element={isMqttMode?<MqttReports/>:<Reports/>}/><Route path="*" element={<><h1>Página não encontrada</h1><Link to="/">Voltar à visão geral</Link></>}/></Route></Routes>}
