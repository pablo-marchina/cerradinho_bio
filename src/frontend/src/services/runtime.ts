// Fonte dos dados: a chave do topo do painel grava a escolha no navegador e recarrega a página.
// Sem escolha gravada, vale o VITE_DATA_SOURCE do .env.local (demo por padrão).
const KEY = 'guaratracking.fonte';
const saved = (() => {
  try {
    return localStorage.getItem(KEY);
  } catch {
    return null;
  }
})();

export const envDataSource = import.meta.env.VITE_DATA_SOURCE === 'mqtt' ? 'mqtt' : 'demo';
export const dataSource: 'mqtt' | 'demo' = saved === 'mqtt' || saved === 'demo' ? saved : envDataSource;
export const isMqttMode = dataSource === 'mqtt';
export const mqttSource = import.meta.env.VITE_MQTT_SOURCE === 'esp32' ? 'esp32' : 'simulator';
export const isPhysicalEsp32 = isMqttMode && mqttSource === 'esp32';
/** Só oferece "dispositivo real" quando há um broker configurado no .env.local. */
export const hasMqttConfig = Boolean(import.meta.env.VITE_MQTT_URL);

export function setDataSource(next: 'mqtt' | 'demo') {
  try {
    localStorage.setItem(KEY, next);
  } catch {
    /* sem armazenamento: a troca vale só até recarregar */
  }
  window.location.reload();
}

export const mqttRuntime = {
  url: import.meta.env.VITE_MQTT_URL || 'ws://127.0.0.1:8083/mqtt',
  deviceId: import.meta.env.VITE_MQTT_DEVICE_ID || 'prototipo-001',
  username: import.meta.env.VITE_MQTT_USERNAME || undefined,
  password: import.meta.env.VITE_MQTT_PASSWORD || undefined,
  source: mqttSource,
};
