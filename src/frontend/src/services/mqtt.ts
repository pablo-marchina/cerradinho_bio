import mqtt, {type IClientOptions, type MqttClient, type Packet} from 'mqtt';
import type {
  Alert,
  AlertStatus,
  Asset,
  CommandInput,
  CommandRecord,
  CommandStatus,
  Event,
  Scenario,
  Snapshot,
} from './types';
import {mqttRuntime} from './runtime';

type Json = Record<string, unknown>;

const now = () => new Date().toISOString();
const deviceAssetId = (deviceId: string) => `DEVICE-${deviceId.toUpperCase()}`;
const asNumber = (value: unknown): number | null =>
  typeof value === 'number' && Number.isFinite(value) ? value : null;
const asBoolean = (value: unknown): boolean | null => typeof value === 'boolean' ? value : null;
const asText = (value: unknown): string | null => typeof value === 'string' && value ? value : null;
const validIso = (value: unknown): string | null => {
  const text = asText(value);
  return text && Number.isFinite(Date.parse(text)) ? text : null;
};
const parseJson = (payload: Uint8Array): Json | null => {
  try {
    const parsed = JSON.parse(new TextDecoder().decode(payload));
    return parsed && typeof parsed === 'object' && !Array.isArray(parsed) ? parsed as Json : null;
  } catch {
    return null;
  }
};

function blankAsset(deviceId: string): Asset {
  return {
    id: deviceAssetId(deviceId),
    name: 'Protótipo ESP32',
    tag: deviceId,
    supplier: 'Não informado pelo dispositivo',
    owner: 'Não informado pelo dispositivo',
    nf: '',
    returnNf: null,
    departedAt: '',
    dueAt: '',
    returnedAt: null,
    closedAt: now(),
    seal: 'unknown',
    battery: null,
    batteryFault: false,
    connection: 'unknown',
    connectionReason: 'unknown',
    lastContact: null,
    queue: null,
    queueReportedAt: null,
    position: null,
    positions: [],
    intervalDays: 7,
    permanenceLimit: 150,
    cycleId: 'Aguardando status',
    gnss: 'unknown',
    overflow: 0,
    confirmed: 0,
  };
}

const snapshot: Snapshot = {
  asOf: now(),
  assets: [blankAsset(mqttRuntime.deviceId)],
  alerts: [],
  events: [],
  commands: [],
  archived: [],
};

const asset = () => snapshot.assets[0];
const eventTitle = (type: string) => ({
  VIOLACAO_LACRE: 'Violação do lacre',
  VIOLACAO_LACRE_RESUMO: 'Resumo de violações do lacre',
  BATERIA_BAIXA: 'Bateria abaixo do limite',
  SENSOR_BATERIA_FALHA: 'Falha no sensor de bateria',
  CICLO_INICIADO: 'Ciclo iniciado',
  CICLO_ENCERRADO: 'Ciclo encerrado',
  AUTORIZACAO_REGISTRADA: 'Retirada autorizada',
  RETIRADA_AUTORIZADA: 'Retirada autorizada realizada',
  LACRE_FECHADO: 'Lacre fechado',
  LACRE_REAPLICADO: 'Lacre reaplicado',
  GNSS_SEM_FIX: 'GNSS sem posição válida',
  GNSS_RECUPERADO: 'GNSS recuperado',
  location_sample: 'Posição válida registrada',
  location_unavailable: 'Posição indisponível; última posição preservada',
}[type] || type.replaceAll('_', ' '));

function addAlert(kind: Alert['kind'], at: string) {
  const current = snapshot.alerts.find(item => item.assetId === asset().id && item.kind === kind && item.status !== 'resolved');
  if (!current) snapshot.alerts.unshift({id: `MQTT-AL-${kind}-${Date.now()}`, assetId: asset().id, kind, at, status: 'open'});
}

function updateCommonState(message: Json, receivedAt: string) {
  const current = asset();
  const deviceId = asText(message.device_id);
  if (deviceId) current.tag = deviceId;

  const seal = asText(message.lacre) || asText(message.seal);
  if (seal === 'violado' || seal === 'violated') current.seal = 'violated';
  if (seal === 'integro' || seal === 'intact') current.seal = 'intact';
  if (seal === 'autorizado' || seal === 'authorized') current.seal = 'authorized';

  const battery = asNumber(message.bateria_pct) ?? asNumber(message.battery_pct) ?? asNumber(message.battery_sim_pct);
  if (battery !== null) {
    current.battery = Math.max(0, Math.min(100, battery));
    current.batteryFault = false;
  }

  const cycleActive = asBoolean(message.ciclo_ativo);
  const cycleId = asNumber(message.ciclo_id) ?? asText(message.ciclo_id);
  if (cycleActive !== null) {
    current.closedAt = cycleActive ? null : receivedAt;
    current.cycleId = cycleActive ? String(cycleId ?? 'Ativo') : 'Sem ciclo';
  } else if (cycleId !== null) current.cycleId = String(cycleId);

  const invoice = asText(message.nf);
  if (invoice) current.nf = invoice;
  const daysOutside = asNumber(message.dias_fora);
  if (daysOutside !== null) current.departedAt = new Date(Date.parse(receivedAt) - daysOutside * 86400000).toISOString();

  const fix = asBoolean(message.gnss_fix) ?? asBoolean(message.fix_valid);
  if (fix !== null) current.gnss = fix ? 'fix' : 'no_fix';

  const queueValue = asNumber(message.fila);
  const queueObject = message.fila && typeof message.fila === 'object' ? message.fila as Json : null;
  const pending = queueValue ?? asNumber(queueObject?.pendentes);
  if (pending !== null) {
    current.queue = pending;
    current.queueReportedAt = receivedAt;
  }
  const lost = asNumber(queueObject?.perdidos);
  if (lost !== null) current.overflow = lost;
}

function applyPosition(message: Json, receivedAt: string) {
  const current = asset();
  const positionObject = message.posicao && typeof message.posicao === 'object' ? message.posicao as Json : null;
  const lat = asNumber(message.latitude) ?? asNumber(message.last_valid_latitude) ?? asNumber(positionObject?.latitude);
  const lng = asNumber(message.longitude) ?? asNumber(message.last_valid_longitude) ?? asNumber(positionObject?.longitude);
  if (lat === null || lng === null) return;
  const capturedAt = validIso(positionObject?.captured_at) || validIso(message.captured_at);
  const position = {label: 'Última posição válida do ESP32', lat, lng, at: capturedAt};
  current.position = position;
  const duplicate = current.positions?.some(item => item.lat === lat && item.lng === lng && item.at === capturedAt);
  if (!duplicate) current.positions = [...(current.positions || []), position].slice(-50);
}

function receiveStatus(message: Json, packet: Packet, receivedAt: string) {
  updateCommonState(message, receivedAt);
  applyPosition(message, receivedAt);
  const reportedAt = validIso(message.reported_at);
  if (!(packet as {retain?: boolean}).retain) asset().lastContact = receivedAt;
  else if (!asset().lastContact && reportedAt) asset().lastContact = reportedAt;
}

function receiveOnline(payload: Uint8Array, packet: Packet, receivedAt: string) {
  const text = new TextDecoder().decode(payload).trim();
  const message = parseJson(payload);
  const state = message ? asText(message.state) : text;
  asset().connection = state === 'online' ? 'connected' : state === 'offline' ? 'disconnected' : 'unknown';
  asset().connectionReason = state === 'online' ? 'connected' : state === 'offline' ? 'lwt' : 'unknown';
  asset().connectionAt = receivedAt;
  const reportedAt = message ? validIso(message.reported_at) : null;
  if (!(packet as {retain?: boolean}).retain && state === 'online') asset().lastContact = receivedAt;
  else if (!asset().lastContact && reportedAt) asset().lastContact = reportedAt;
}

function receiveEvent(message: Json, receivedAt: string) {
  updateCommonState(message, receivedAt);
  applyPosition(message, receivedAt);
  const type = asText(message.type) || asText(message.tipo) || 'EVENTO_MQTT';
  const occurredAt = validIso(message.occurred_at) || receivedAt;
  const critical = asBoolean(message.critical) ?? asBoolean(message.critico) ?? false;
  const record: Event = {
    id: asText(message.event_id) || `MQTT-EV-${Date.now()}-${snapshot.events.length}`,
    assetId: asset().id,
    cycleId: asset().cycleId,
    tag: asset().tag,
    title: eventTitle(type),
    kind: critical ? 'critical' : type.includes('RECUS') || type.includes('FALHA') || type.includes('BAIXA') || type.includes('SEM_FIX') ? 'warning' : type.includes('RECUPERADO') || type.includes('INICIADO') ? 'success' : 'info',
    occurredAt,
    receivedAt,
  };
  if (!snapshot.events.some(item => item.id === record.id)) snapshot.events.unshift(record);
  if (type === 'VIOLACAO_LACRE' || type === 'VIOLACAO_LACRE_RESUMO') {
    asset().seal = 'violated';
    addAlert('seal', occurredAt);
  }
  if (type === 'BATERIA_BAIXA') addAlert('battery', occurredAt);
  if (type === 'SENSOR_BATERIA_FALHA') {
    asset().battery = null;
    asset().batteryFault = true;
  }
  asset().lastContact = receivedAt;
}

function commandStep(command: CommandRecord, status: CommandStatus, at: string) {
  if (command.steps.at(-1)?.status !== status) command.steps.push({status, at});
  command.status = status;
}

function receiveResponse(message: Json, receivedAt: string) {
  const id = asText(message.id) || asText(message.command_id);
  if (!id) return;
  const command = snapshot.commands?.find(item => item.id === id);
  if (!command) return;
  const accepted = asBoolean(message.ok) ?? (asText(message.status) === 'aceito');
  commandStep(command, accepted ? 'executed' : 'refused', receivedAt);
  const error = asText(message.erro) || asText(message.motivo);
  const source = mqttRuntime.source === 'esp32' ? 'ESP32' : 'simulador MQTT';
  command.reason = accepted ? `Comando executado e confirmado pelo ${source}.` : `Comando recusado pelo ${source}${error ? `: ${error}` : '.'}`;
  const resultCycle = asNumber(message.ciclo_id);
  if (resultCycle !== null) command.resultCycleId = String(resultCycle);
}

const actionByType: Partial<Record<CommandInput['type'], string>> = {
  open: 'ciclo_iniciar',
  remittance: 'remessa',
  authorize: 'autorizar',
  return_nf: 'nf_retorno',
  close: 'ciclo_encerrar',
};

class MqttGateway {
  private client: MqttClient | null = null;
  private connecting: Promise<void> | null = null;
  private subscribed = false;
  private timeoutHandles = new Map<string, ReturnType<typeof setTimeout>>();

  connect() {
    if (this.client?.connected && this.subscribed) return Promise.resolve();
    if (this.connecting) return this.connecting;
    if (!this.client) this.createClient();
    this.connecting = new Promise<void>((resolve, reject) => {
      const client = this.client!;
      const prefix = `guaratracking/${mqttRuntime.deviceId}/`;
      const timer = setTimeout(() => reject(new Error('Tempo esgotado ao conectar e assinar o broker MQTT.')), 6000);
      const subscribe = () => {
        client.subscribe([`${prefix}status`, `${prefix}evento`, `${prefix}online`, `${prefix}cmd/resposta`], {qos: 0}, error => {
          clearTimeout(timer);
          if (error) reject(error); else { this.subscribed = true; resolve(); }
        });
      };
      if (client.connected) subscribe(); else client.once('connect', subscribe);
    }).finally(() => { this.connecting = null; });
    return this.connecting;
  }

  private createClient() {
      const options: IClientOptions = {
        clientId: `guaratracking-dashboard-${crypto.randomUUID().slice(0, 8)}`,
        clean: true,
        reconnectPeriod: 2000,
        connectTimeout: 5000,
        username: mqttRuntime.username,
        password: mqttRuntime.password,
      };
      const client = mqtt.connect(mqttRuntime.url, options);
      this.client = client;
      const prefix = `guaratracking/${mqttRuntime.deviceId}/`;
      client.on('message', (topic, payload, packet) => {
        const receivedAt = now();
        snapshot.asOf = receivedAt;
        const suffix = topic.slice(prefix.length);
        if (suffix === 'online') receiveOnline(payload, packet, receivedAt);
        else {
          const message = parseJson(payload);
          if (!message) return;
          if (suffix === 'status') receiveStatus(message, packet, receivedAt);
          if (suffix === 'evento') receiveEvent(message, receivedAt);
          if (suffix === 'cmd/resposta') receiveResponse(message, receivedAt);
        }
      });
      client.on('offline', () => {
        asset().connection = 'unknown';
        asset().connectionReason = 'unknown';
      });
      client.on('end', () => { this.subscribed = false; });
  }

  async getSnapshot() {
    await this.connect();
    snapshot.asOf = now();
    return structuredClone(snapshot);
  }

  async command(input: CommandInput) {
    await this.connect();
    const action = actionByType[input.type];
    if (!action) throw new Error('Esta ação administrativa não existe no protocolo MQTT do ESP32.');
    if (!this.client?.connected) throw new Error('Dashboard desconectado do broker MQTT.');
    const id = `web-${Date.now().toString(36)}`;
    const issuedAt = now();
    const record: CommandRecord = {
      ...input,
      id,
      tag: asset().tag,
      cycleId: String(asset().cycleId || 'Desconhecido'),
      issuedAt,
      expiresAt: new Date(Date.now() + 10000).toISOString(),
      status: 'created',
      reason: 'Preparando publicação no broker.',
      steps: [{status: 'created', at: issuedAt}],
    };
    snapshot.commands ??= [];
    snapshot.commands.push(record);
    const payload: Json = {id, acao: action};
    if (input.type === 'remittance') payload.valor = input.payload.nf;
    const topic = `guaratracking/${mqttRuntime.deviceId}/cmd`;
    await new Promise<void>((resolve, reject) => this.client!.publish(topic, JSON.stringify(payload), {qos: 0, retain: false}, error => error ? reject(error) : resolve()));
    if (record.status === 'created') {
      commandStep(record, 'published', now());
      record.reason = 'Publicado; aguardando cmd/resposta do ESP32.';
    }
    this.timeoutHandles.set(id, setTimeout(() => {
      if (record.status === 'published' || record.status === 'received') {
        commandStep(record, 'timeout', now());
        record.reason = 'Nenhuma cmd/resposta recebida em 10 segundos. Confira status e eventos antes de repetir.';
      }
      this.timeoutHandles.delete(id);
    }, 10000));
    return structuredClone(record);
  }

  updateAlert(id: string, status: AlertStatus, reason: string) {
    const alert = snapshot.alerts.find(item => item.id === id);
    if (!alert) throw new Error('Alerta não encontrado.');
    alert.status = status;
    alert.actions ??= [];
    alert.actions.push({status, actor: 'Operador do dashboard', reason, at: now()});
    return structuredClone(alert);
  }

  reset() {
    snapshot.assets = [blankAsset(mqttRuntime.deviceId)];
    snapshot.alerts = [];
    snapshot.events = [];
    snapshot.commands = [];
    snapshot.archived = [];
    snapshot.asOf = now();
    return {ok: true};
  }
}

const gateway = new MqttGateway();

export const mqttApi = {
  snapshot: () => gateway.getSnapshot(),
  updateAlert: async (id: string, status: AlertStatus, reason: string) => gateway.updateAlert(id, status, reason),
  registerAsset: async (_input: Record<string, string>) => { throw new Error('Cadastro de ativos depende do backend e não é enviado ao ESP32.'); },
  command: (input: CommandInput) => gateway.command(input),
  scenario: async (_id: string, _scenario: Scenario) => { throw new Error('Cenários sintéticos estão desativados no modo MQTT.'); },
  reset: async () => gateway.reset(),
};
