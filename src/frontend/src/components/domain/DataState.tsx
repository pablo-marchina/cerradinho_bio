import { AlertCircle, LoaderCircle } from 'lucide-react';
import { Button } from '@/components/ui/primitives';
import {isMqttMode} from '@/services/runtime';
export function DataState({loading,error,retry}:{loading:boolean;error:Error|null;retry:()=>void}){return <div className="card empty" role={error?'alert':'status'}>{loading?<><LoaderCircle className="spin" size={24}/>{isMqttMode?'Conectando ao broker MQTT…':'Carregando dados simulados…'}</>:<><AlertCircle size={24}/><strong>Não foi possível obter os dados</strong><span>{error?.message}</span><Button onClick={retry}>Tentar novamente</Button></>}</div>;}
