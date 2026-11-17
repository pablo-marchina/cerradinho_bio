export const dataSource = import.meta.env.VITE_DATA_SOURCE === 'mqtt' ? 'mqtt' : 'demo';
export const isMqttMode = dataSource === 'mqtt';
export const mqttSource = import.meta.env.VITE_MQTT_SOURCE === 'esp32' ? 'esp32' : 'simulator';
export const isPhysicalEsp32 = isMqttMode && mqttSource === 'esp32';

export const mqttRuntime = {
  url: import.meta.env.VITE_MQTT_URL || 'ws://127.0.0.1:8083/mqtt',
  deviceId: import.meta.env.VITE_MQTT_DEVICE_ID || 'prototipo-001',
  username: import.meta.env.VITE_MQTT_USERNAME || undefined,
  password: import.meta.env.VITE_MQTT_PASSWORD || undefined,
  source: mqttSource,
};
