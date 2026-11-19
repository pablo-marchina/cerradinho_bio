import { FlaskConical, Radio } from 'lucide-react';
import { dataSource, hasMqttConfig, isPhysicalEsp32, setDataSource } from '@/services/runtime';

/**
 * Chave entre dados de demonstração (fictícios, para apresentar e deixar o parceiro mexer)
 * e dado real do dispositivo pelo MQTT. A carteira por persona continua de demonstração
 * até existir backend; a chave muda a telemetria e os comandos da TAG.
 */
export function SourceToggle() {
  return (
    <div className="source-toggle" role="group" aria-label="Fonte dos dados">
      <button type="button" className={dataSource === 'demo' ? 'on' : ''} aria-pressed={dataSource === 'demo'} onClick={() => dataSource !== 'demo' && setDataSource('demo')} title="Dados fictícios para apresentação">
        <FlaskConical size={16} aria-hidden="true" /> Demonstração
      </button>
      <button
        type="button"
        className={dataSource === 'mqtt' ? 'on' : ''}
        aria-pressed={dataSource === 'mqtt'}
        disabled={!hasMqttConfig}
        onClick={() => dataSource !== 'mqtt' && setDataSource('mqtt')}
        title={hasMqttConfig ? 'Telemetria e comandos do dispositivo pelo MQTT' : 'Configure o broker no .env.local para usar o dispositivo real'}
      >
        <Radio size={16} aria-hidden="true" /> {isPhysicalEsp32 || dataSource !== 'mqtt' ? 'Dispositivo real' : 'Simulador MQTT'}
      </button>
    </div>
  );
}
