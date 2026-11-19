import mqtt from 'mqtt';

const broker=process.env.MQTT_URL||'mqtt://127.0.0.1:1883';
const deviceId=process.env.MQTT_DEVICE_ID||'prototipo-001';
const prefix=`guaratracking/${deviceId}/`;
const options={
  clientId:`guaratracking-simulador-${process.pid}`,
  username:process.env.MQTT_USERNAME||undefined,
  password:process.env.MQTT_PASSWORD||undefined,
  will:{topic:`${prefix}online`,payload:'offline',qos:1,retain:true},
};
const client=mqtt.connect(broker,options);
const state={seal:'integro',battery:68,cycleActive:true,cycleId:12,nf:'NF-000123',daysOutside:39,queue:0,returnNf:false};
const publish=(suffix,payload,retain=false)=>client.publish(`${prefix}${suffix}`,typeof payload==='string'?payload:JSON.stringify(payload),{qos:0,retain});
const event=(type,extra={})=>publish('evento',{device_id:deviceId,type,uptime_ms:Math.floor(process.uptime()*1000),clock:'uptime_only',demo:true,seal:state.seal==='violado'?'violated':'intact',battery_sim_pct:state.battery,ciclo_id:state.cycleId,ciclo_ativo:state.cycleActive,dias_fora:state.daysOutside,dia_ocorrencia:state.daysOutside,atraso:false,permanencia:false,critico:type==='VIOLACAO_LACRE',...extra});
const status=()=>publish('status',{device_id:deviceId,lacre:state.seal,bateria_pct:state.battery,ciclo_ativo:state.cycleActive,nf:state.nf,dias_fora:state.daysOutside,gnss_fix:true,fila:state.queue},true);
const location=()=>event('location_sample',{fix_valid:true,latitude:-21.120000,longitude:-48.520000,distance_m:null,moved:false,baseline_created:false});
const respond=(id,acao,ok,erro,extra={})=>publish('cmd/resposta',{id:id||'',acao:acao||'',ok,...(!ok&&erro?{erro}:{}),...extra});

function handleCommand(payload){
  let command;
  try{command=JSON.parse(payload.toString());}catch{respond('','',false,'json_invalido');return;}
  const id=typeof command.id==='string'?command.id:'';
  const acao=typeof command.acao==='string'?command.acao:'';
  let ok=true,erro='',extra={};
  switch(acao){
    case 'ciclo_iniciar':case 'vincular':
      if(state.cycleActive){ok=false;erro='ciclo_ativo';break;}
      state.cycleActive=true;state.cycleId+=1;state.daysOutside=0;state.nf='';state.returnNf=false;event('CICLO_INICIADO');break;
    case 'remessa':
      if(!state.cycleActive){ok=false;erro='sem_ciclo_ativo';break;}
      if(typeof command.valor!=='string'||!/^[A-Za-z0-9./-]{1,15}$/.test(command.valor)){ok=false;erro='nf_invalida';break;}
      state.nf=command.valor;event('REMESSA_REGISTRADA');break;
    case 'nf_retorno':
      if(!state.cycleActive){ok=false;erro='sem_ciclo_ativo';break;}
      if(state.returnNf){ok=false;erro='nf_ja_lancada';break;}
      state.returnNf=true;event('NF_RETORNO_LANCADA');break;
    case 'autorizar':
      extra={janela_dias:3,valida_ate_ms:Math.floor(process.uptime()*1000)+259200000};event('AUTORIZACAO_REGISTRADA');break;
    case 'ciclo_encerrar':
      if(!state.cycleActive){ok=false;erro='sem_ciclo_ativo';break;}
      if(state.seal==='violado'){ok=false;erro='encerramento_recusado';extra={motivo:'lacre_aberto'};break;}
      if(!state.returnNf){ok=false;erro='encerramento_recusado';extra={motivo:'nf_retorno_ausente'};break;}
      state.cycleActive=false;event('CICLO_ENCERRADO');break;
    case 'avancar':case 'voltar':case 'ligar':case 'desligar':break;
    default:ok=false;erro='acao_desconhecida';
  }
  respond(id,acao,ok,erro,extra);
  status();
}

client.on('connect',()=>{
  console.log(`Simulador conectado a ${broker} como ${deviceId}`);
  client.subscribe(`${prefix}cmd`,error=>{if(error)throw error;});
  publish('online','online',true);
  status();
  event('location_sample',{fix_valid:true,latitude:-21.120000,longitude:-48.520000,distance_m:null,moved:null,baseline_created:true});
  setTimeout(()=>{state.seal='violado';event('VIOLACAO_LACRE');status();},1000);
  setInterval(()=>{status();location();},5000);
});
client.on('message',(topic,payload)=>{if(topic===`${prefix}cmd`)handleCommand(payload);});
client.on('error',error=>{console.error(`Falha MQTT: ${error.message}`);process.exitCode=1;});
const stop=()=>{publish('online','offline',true);client.end(false,()=>process.exit());};
process.on('SIGINT',stop);
process.on('SIGTERM',stop);
