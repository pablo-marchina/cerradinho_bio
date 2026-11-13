'use strict';

const net = require('node:net');

const host = process.env.EMQX_HOST || '127.0.0.1';
const tcpPort = Number(process.env.EMQX_TCP_PORT || 1883);
const wsUrl = process.env.EMQX_WS_URL || `ws://${host}:8083/mqtt`;
const timeoutMs = Number(process.env.EMQX_TEST_TIMEOUT_MS || 12000);

if (typeof WebSocket !== 'function') {
  console.error('FALHA: este teste exige Node.js 22 ou superior (WebSocket global).');
  process.exit(1);
}

function encodeRemainingLength(value) {
  const bytes = [];
  do {
    let digit = value % 128;
    value = Math.floor(value / 128);
    if (value > 0) digit |= 0x80;
    bytes.push(digit);
  } while (value > 0);
  return Buffer.from(bytes);
}

function mqttString(value) {
  const body = Buffer.from(value);
  return Buffer.concat([
    Buffer.from([body.length >> 8, body.length & 0xff]),
    body,
  ]);
}

function mqttPacket(header, payload) {
  return Buffer.concat([
    Buffer.from([header]),
    encodeRemainingLength(payload.length),
    payload,
  ]);
}

function connectPacket(clientId) {
  const variableHeader = Buffer.from([0, 4, 77, 81, 84, 84, 4, 2, 0, 15]);
  return mqttPacket(0x10, Buffer.concat([variableHeader, mqttString(clientId)]));
}

function subscribePacket(topic, packetId) {
  return mqttPacket(0x82, Buffer.concat([
    Buffer.from([packetId >> 8, packetId & 0xff]),
    mqttString(topic),
    Buffer.from([0]),
  ]));
}

function publishPacket(topic, payload) {
  return mqttPacket(0x30, Buffer.concat([mqttString(topic), Buffer.from(payload)]));
}

function packetParser(onPacket) {
  let pending = Buffer.alloc(0);
  return chunk => {
    pending = Buffer.concat([pending, Buffer.from(chunk)]);
    while (pending.length >= 2) {
      let multiplier = 1;
      let remainingLength = 0;
      let cursor = 1;
      let digit;
      do {
        if (cursor >= pending.length) return;
        digit = pending[cursor++];
        remainingLength += (digit & 0x7f) * multiplier;
        multiplier *= 128;
      } while (digit & 0x80);

      const packetLength = cursor + remainingLength;
      if (pending.length < packetLength) return;
      const packet = pending.subarray(0, packetLength);
      pending = pending.subarray(packetLength);
      onPacket(packet);
    }
  };
}

function parsePublish(packet) {
  let cursor = 1;
  while (packet[cursor++] & 0x80) {
    // Avanca pelos bytes do tamanho restante.
  }
  const topicLength = packet.readUInt16BE(cursor);
  cursor += 2;
  return {
    topic: packet.subarray(cursor, cursor + topicLength).toString(),
    payload: packet.subarray(cursor + topicLength).toString(),
  };
}

const suffix = `${process.pid}-${Date.now()}`;
const topicRoot = `guaratracking/teste-dashboard-${suffix}`;
const statusTopic = `${topicRoot}/status`;
const commandTopic = `${topicRoot}/cmd`;
const statusPayload = JSON.stringify({
  device_id: `teste-dashboard-${suffix}`,
  lacre: 'integro',
  bateria_pct: 87,
});
const commandPayload = JSON.stringify({
  command_id: `cmd-${suffix}`,
  command: 'teste_conectividade',
});

let tcp;
let websocket;
let tcpReady = false;
let websocketReady = false;
let statusReceived = false;
let finished = false;

const timeout = setTimeout(() => {
  fail(`timeout de ${timeoutMs} ms`);
}, timeoutMs);

function cleanup() {
  clearTimeout(timeout);
  if (tcp) tcp.destroy();
  if (websocket && websocket.readyState < 2) websocket.close();
}

function fail(message) {
  if (finished) return;
  finished = true;
  cleanup();
  console.error(`FALHA: ${message}`);
  setTimeout(() => process.exit(1), 50);
}

function pass() {
  if (finished) return;
  finished = true;
  cleanup();
  console.log('PASS: MQTT TCP -> WebSocket entregou status corretamente');
  console.log('PASS: WebSocket -> MQTT TCP entregou comando corretamente');
  console.log(`PASS: broker local validado em ${host}:${tcpPort} e ${wsUrl}`);
  setTimeout(() => process.exit(0), 50);
}

function publishStatusWhenReady() {
  if (!tcpReady || !websocketReady) return;
  tcp.write(publishPacket(statusTopic, statusPayload));
}

const handleTcpPacket = packet => {
  const type = packet[0] & 0xf0;
  if (type === 0x20) {
    if (packet[3] !== 0) return fail(`broker recusou MQTT TCP (codigo ${packet[3]})`);
    tcp.write(subscribePacket(commandTopic, 1));
    return;
  }
  if (type === 0x90) {
    tcpReady = true;
    publishStatusWhenReady();
    return;
  }
  if (type === 0x30) {
    const message = parsePublish(packet);
    if (message.topic !== commandTopic || message.payload !== commandPayload) {
      return fail('comando recebido pelo MQTT TCP diverge do publicado no WebSocket');
    }
    if (!statusReceived) return fail('comando chegou antes da validacao do status');
    pass();
  }
};

const handleWebSocketPacket = packet => {
  const type = packet[0] & 0xf0;
  if (type === 0x20) {
    if (packet[3] !== 0) return fail(`broker recusou MQTT WebSocket (codigo ${packet[3]})`);
    websocket.send(subscribePacket(statusTopic, 1));
    return;
  }
  if (type === 0x90) {
    websocketReady = true;
    publishStatusWhenReady();
    return;
  }
  if (type === 0x30) {
    const message = parsePublish(packet);
    if (message.topic !== statusTopic || message.payload !== statusPayload) {
      return fail('status recebido pelo WebSocket diverge do publicado no MQTT TCP');
    }
    statusReceived = true;
    websocket.send(publishPacket(commandTopic, commandPayload));
  }
};

const parseTcp = packetParser(handleTcpPacket);
const parseWebSocket = packetParser(handleWebSocketPacket);

websocket = new WebSocket(wsUrl, 'mqtt');
websocket.binaryType = 'arraybuffer';
websocket.onopen = () => websocket.send(connectPacket(`teste-ws-${suffix}`));
websocket.onmessage = event => parseWebSocket(Buffer.from(event.data));
websocket.onerror = () => fail(`nao foi possivel abrir ${wsUrl}`);

tcp = net.createConnection(tcpPort, host, () => {
  tcp.write(connectPacket(`teste-tcp-${suffix}`));
});
tcp.on('data', parseTcp);
tcp.on('error', error => fail(`MQTT TCP indisponivel: ${error.message}`));
