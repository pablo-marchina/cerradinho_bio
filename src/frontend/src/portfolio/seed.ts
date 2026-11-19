import type { Criticality, PortfolioAsset, PortfolioState, SealState, Stage, Supplier } from './types.ts';

// Carteira sintética para demonstrar as visões. Nada aqui é dado do parceiro:
// oficinas, contatos, ativos, OMs e NFs são fictícios e gerados de forma determinística.

const DAY = 86400000;
export const PLANT = { lat: -18.42, lng: -52.6, label: 'Usina (Chapadão do Céu, GO)', uf: 'GO' };

export const SUPPLIERS: Supplier[] = [
  { id: 'SUP-01', name: 'Oficina Alfa Motores', city: 'Rio Verde', uf: 'GO', contact: 'Marcos (fictício)', email: 'atendimento@alfa-motores.exemplo', phone: '(64) 0000-0101', lat: -17.79, lng: -50.92 },
  { id: 'SUP-02', name: 'Beta Bombas Industriais', city: 'Ribeirão Preto', uf: 'SP', contact: 'Paula (fictícia)', email: 'servicos@beta-bombas.exemplo', phone: '(16) 0000-0202', lat: -21.18, lng: -47.81 },
  { id: 'SUP-03', name: 'Gama Redutores', city: 'Uberlândia', uf: 'MG', contact: 'Rafael (fictício)', email: 'contato@gama-redutores.exemplo', phone: '(34) 0000-0303', lat: -18.92, lng: -48.28 },
  { id: 'SUP-04', name: 'Delta Válvulas', city: 'Goiânia', uf: 'GO', contact: 'Sônia (fictícia)', email: 'oficina@delta-valvulas.exemplo', phone: '(62) 0000-0404', lat: -16.69, lng: -49.26 },
  { id: 'SUP-05', name: 'Épsilon Compressores', city: 'Campinas', uf: 'SP', contact: 'Bruno (fictício)', email: 'pos-venda@epsilon.exemplo', phone: '(19) 0000-0505', lat: -22.91, lng: -47.06 },
  { id: 'SUP-06', name: 'Zeta Hidráulica', city: 'Uberaba', uf: 'MG', contact: 'Lúcia (fictícia)', email: 'reparos@zeta-hidraulica.exemplo', phone: '(34) 0000-0606', lat: -19.75, lng: -47.93 },
  { id: 'SUP-07', name: 'Eta Eletromecânica', city: 'Jataí', uf: 'GO', contact: 'Diego (fictício)', email: 'contato@eta-eletro.exemplo', phone: '(64) 0000-0707', lat: -17.88, lng: -51.72 },
  { id: 'SUP-08', name: 'Teta Usinagem', city: 'Barretos', uf: 'SP', contact: 'Fernanda (fictícia)', email: 'comercial@teta-usinagem.exemplo', phone: '(17) 0000-0808', lat: -20.56, lng: -48.57 },
];

const NAMES = [
  'Motor trifásico 75 cv', 'Bomba centrífuga', 'Redutor de velocidade', 'Válvula de controle', 'Compressor de ar', 'Cilindro hidráulico',
  'Motor da moenda', 'Bomba de vinhaça', 'Ventilador da caldeira', 'Inversor de frequência', 'Turbina de acionamento', 'Bomba dosadora',
  'Mancal da esteira', 'Picador de cana', 'Desfibrador', 'Motor do exaustor', 'Bomba de caldo', 'Redutor da esteira', 'Painel de comando',
  'Eixo da moenda', 'Trocador de calor', 'Motor elétrico 30 cv', 'Bomba de condensado', 'Válvula de segurança', 'Rolo de moenda',
];
const AREAS = ['Extração', 'Caldeira', 'Destilaria', 'Utilidades', 'Fábrica de açúcar', 'Logística interna'];

function rng(seed: number) {
  let s = seed >>> 0;
  return () => {
    s = (s * 1664525 + 1013904223) >>> 0;
    return s / 4294967296;
  };
}

const plan: [Stage, number][] = [
  ['planejamento', 4], ['aguardando_nf', 5], ['aguardando_coleta', 4], ['ida', 7], ['oficina', 36], ['retorno', 7], ['na_usina', 7], ['encerrado', 28],
];

export function createPortfolio(nowIso = new Date().toISOString()): PortfolioState {
  const r = rng(131);
  const now = Date.parse(nowIso);
  const at = (daysAgo: number) => new Date(now - daysAgo * DAY - Math.floor(r() * 8) * 3600000).toISOString();
  const pick = <T,>(xs: T[]) => xs[Math.floor(r() * xs.length)];
  const assets: PortfolioAsset[] = [];
  let n = 0;
  let violated = 0;
  let silent = 0;
  let nearLegal = 0;

  for (const [stage, count] of plan) {
    for (let i = 0; i < count; i++) {
      n++;
      const code = String(1000 + n * 7).padStart(4, '0');
      const supplier = SUPPLIERS[(n * 5 + i) % SUPPLIERS.length];
      const criticality: Criticality = r() < 0.25 ? 'alta' : r() < 0.6 ? 'media' : 'baixa';
      let agreedDays = [15, 20, 30, 45, 60][Math.floor(r() * 5)];
      // Datas em "dias atrás", calculadas de trás para frente a partir da etapa atual.
      const ri = (a: number, b: number) => a + Math.floor(r() * (b - a + 1));
      const repairDays = Math.max(5, Math.round(agreedDays * (0.6 + r() * 0.65)));
      let closedAgo = 0, plantAgo = 0, returnAgo = 0, arrivedSupAgo = 0, departedAgo = 0, nfAgo = 0;
      if (stage === 'encerrado') { closedAgo = ri(3, 170); plantAgo = closedAgo + ri(1, 4); }
      if (stage === 'na_usina') plantAgo = ri(1, 9);
      if (stage === 'encerrado' || stage === 'na_usina') returnAgo = plantAgo + ri(1, 2);
      if (stage === 'retorno') returnAgo = ri(0, 2);
      if (['retorno', 'na_usina', 'encerrado'].includes(stage)) arrivedSupAgo = returnAgo + repairDays;
      if (stage === 'oficina') {
        if (nearLegal < 2 && supplier.uf === 'GO') { arrivedSupAgo = 246 + nearLegal * 12; nearLegal++; agreedDays = arrivedSupAgo + 20; }
        else if (r() < 0.2) { const late = ri(2, 28); agreedDays = Math.max(10, ri(10, 50)); arrivedSupAgo = agreedDays + late; }
        else { arrivedSupAgo = ri(2, 55); agreedDays = Math.ceil((arrivedSupAgo + ri(2, 40)) / 5) * 5; }
      }
      if (['oficina', 'retorno', 'na_usina', 'encerrado'].includes(stage)) departedAgo = arrivedSupAgo + ri(1, 2);
      if (stage === 'ida') departedAgo = ri(0, 2);
      if (departedAgo || stage === 'ida') nfAgo = departedAgo + ri(1, 2);
      if (stage === 'aguardando_coleta') nfAgo = ri(1, 4);
      const hasNf = !['planejamento', 'aguardando_nf'].includes(stage);
      const issuedAt = hasNf ? at(nfAgo) : null;
      const outsideOrLater = ['ida', 'oficina', 'retorno', 'na_usina', 'encerrado'].includes(stage);
      const atSupplierOrLater = ['oficina', 'retorno', 'na_usina', 'encerrado'].includes(stage);
      const value = Math.round((8 + r() * 140) * 1000);

      let seal: SealState = stage === 'planejamento' ? 'sem_lacre' : 'integro';
      if (stage === 'oficina' && violated < 2 && r() < 0.3) {
        seal = 'violado';
        violated++;
      } else if (stage === 'oficina' && r() < 0.15) seal = 'autorizado';

      const signalAgo = stage === 'oficina' && silent < 4 && r() < 0.18 ? (silent++, 10 + Math.floor(r() * 20)) : r() * 3;
      const tag = stage === 'planejamento' && i % 2 === 0 ? null : stage === 'encerrado' ? `GT-${String(n).padStart(4, '0')}` : `GT-${String(n).padStart(4, '0')}`;
      const legsFrom = stage === 'ida' ? r() : stage === 'retorno' ? 1 - r() : null;
      const position =
        stage === 'oficina'
          ? { lat: supplier.lat + (r() - 0.5) * 0.05, lng: supplier.lng + (r() - 0.5) * 0.05, label: `${supplier.city}/${supplier.uf}`, at: at(signalAgo) }
          : legsFrom !== null
            ? { lat: PLANT.lat + (supplier.lat - PLANT.lat) * legsFrom, lng: PLANT.lng + (supplier.lng - PLANT.lng) * legsFrom, label: 'Em rota (estimado)', at: at(r()) }
            : stage === 'na_usina'
              ? { lat: PLANT.lat, lng: PLANT.lng, label: PLANT.label, at: at(r()) }
              : null;
      const a: PortfolioAsset = {
        id: `AT-${code}`,
        name: pick(NAMES),
        om: `OM-${4500000 + n * 37}`,
        area: pick(AREAS),
        owner: 'Geraldo (PCM)',
        criticality,
        photo: null,
        tag: stage === 'encerrado' ? null : tag,
        sealApplied: stage !== 'planejamento',
        stage,
        supplierId: stage === 'planejamento' && i < 2 ? null : supplier.id,
        quote: stage === 'planejamento' && i < 2 ? null : Math.round(value * (0.08 + r() * 0.2)),
        agreedDays: stage === 'planejamento' && i < 2 ? null : agreedDays,
        problem: pick(['Rolamento travado', 'Enrolamento queimado', 'Vazamento no selo mecânico', 'Desgaste de engrenagens', 'Vibração excessiva', 'Ruptura de vedação']),
        createdAt: at(Math.max(nfAgo, 0) + 3 + Math.floor(r() * 3)),
        nfRequestedAt: stage === 'planejamento' ? null : at(Math.max(nfAgo, 0) + 1 + (stage === 'aguardando_nf' ? Math.floor(r() * 5) : 0)),
        remittance: issuedAt ? { number: `NF-${String(10000 + n * 13).padStart(6, '0')}`, series: '1', issuedAt, cfop: supplier.uf === 'GO' ? '5.915' : '6.915', value } : null,
        pickupRequestedAt: hasNf && (stage !== 'aguardando_coleta' || i % 2 === 0) ? at(Math.max(departedAgo, 0)) : null,
        departedAt: outsideOrLater ? at(departedAgo) : null,
        arrivedSupplierAt: atSupplierOrLater ? at(arrivedSupAgo) : null,
        dueAt: atSupplierOrLater ? new Date(now - arrivedSupAgo * DAY + agreedDays * DAY).toISOString() : stage === 'ida' ? new Date(now + (agreedDays + 2) * DAY).toISOString() : null,
        returnStartedAt: ['retorno', 'na_usina', 'encerrado'].includes(stage) ? at(returnAgo) : null,
        arrivedPlantAt: ['na_usina', 'encerrado'].includes(stage) ? at(plantAgo) : null,
        returnInvoice: stage === 'encerrado' ? { number: `NF-${String(50000 + n * 11).padStart(6, '0')}`, series: '1', issuedAt: at(closedAgo), cfop: supplier.uf === 'GO' ? '5.916' : '6.916', value } : null,
        closedAt: stage === 'encerrado' ? at(closedAgo) : null,
        seal: stage === 'encerrado' ? 'sem_lacre' : seal,
        battery: stage === 'encerrado' || !tag ? null : Math.round(15 + r() * 85),
        lastSignalAt: tag && stage !== 'encerrado' ? at(signalAgo) : null,
        position: stage === 'encerrado' ? null : position,
        lastFollowUpAt: stage === 'oficina' && r() < 0.45 ? at(Math.floor(r() * 6)) : null,
        history: [],
      };
      a.history = buildHistory(a, supplier);
      assets.push(a);
    }
  }

  const usedTags = new Set(assets.map(a => a.tag).filter(Boolean));
  const freeTags = Array.from({ length: 12 }, (_, k) => `GT-${String(200 + k).padStart(4, '0')}`).filter(t => !usedTags.has(t));
  return { now: nowIso, assets, suppliers: SUPPLIERS, notices: initialNotices(assets, nowIso), freeTags };
}

function buildHistory(a: PortfolioAsset, s: Supplier) {
  const h: PortfolioAsset['history'] = [{ at: a.createdAt, who: 'Geraldo (PCM)', text: `Ativo cadastrado a partir da ${a.om}: ${a.problem.toLowerCase()}.` }];
  if (a.nfRequestedAt) h.push({ at: a.nfRequestedAt, who: 'Geraldo (PCM)', text: `Oficina ${s.name} definida e NF de remessa solicitada ao Fiscal.` });
  if (a.remittance) h.push({ at: a.remittance.issuedAt, who: 'Clarice (Fiscal)', text: `NF de remessa emitida (CFOP ${a.remittance.cfop}).`, tone: 'success' });
  if (a.departedAt) h.push({ at: a.departedAt, who: 'Logística', text: 'Coletado pela transportadora com TAG e lacre.' });
  if (a.arrivedSupplierAt) h.push({ at: a.arrivedSupplierAt, who: 'Sistema', text: `Chegada registrada em ${s.city}/${s.uf}.` });
  if (a.seal === 'violado') h.push({ at: a.lastSignalAt ?? a.arrivedSupplierAt ?? a.createdAt, who: 'TAG', text: 'Abertura do lacre sem autorização.', tone: 'critical' });
  if (a.lastFollowUpAt) h.push({ at: a.lastFollowUpAt, who: 'Geraldo (PCM)', text: 'Oficina cobrada sobre o prazo.' });
  if (a.returnStartedAt) h.push({ at: a.returnStartedAt, who: 'Sistema', text: 'Saiu da oficina a caminho da usina.' });
  if (a.arrivedPlantAt) h.push({ at: a.arrivedPlantAt, who: 'Sistema', text: 'Chegou na usina. Aguardando NF de retorno.', tone: 'warning' });
  if (a.returnInvoice) h.push({ at: a.returnInvoice.issuedAt, who: 'Clarice (Fiscal)', text: 'NF de retorno lançada. Ciclo encerrado e TAG liberada.', tone: 'success' });
  return h.sort((x, y) => Date.parse(y.at) - Date.parse(x.at));
}

function initialNotices(assets: PortfolioAsset[], now: string) {
  return assets
    .filter(a => a.stage === 'aguardando_nf' || a.stage === 'na_usina')
    .map((a, i) => ({
      id: `NT-${i + 1}`,
      to: 'fiscal' as const,
      at: a.stage === 'aguardando_nf' ? a.nfRequestedAt ?? now : a.arrivedPlantAt ?? now,
      assetId: a.id,
      text: a.stage === 'aguardando_nf' ? `PCM pediu a NF de remessa de ${a.name} (${a.id}).` : `${a.name} (${a.id}) chegou na usina sem NF de retorno.`,
      read: false,
    }));
}
