// Carteira de ativos em manutenção externa (visões por persona).
// Todos os dados de demonstração são fictícios: ativos, oficinas, contatos e NFs.

export type Stage =
  | 'planejamento' // PCM recebeu a demanda e está cotando oficina, preço e prazo
  | 'aguardando_nf' // PCM pediu a NF de remessa ao Fiscal
  | 'aguardando_coleta' // NF emitida; PCM acionou (ou precisa acionar) a logística
  | 'ida' // em trânsito para a oficina
  | 'oficina' // em reparo no fornecedor
  | 'retorno' // em trânsito de volta para a usina
  | 'na_usina' // chegou na usina; aguardando NF de retorno (baixa fiscal)
  | 'encerrado'; // NF de retorno lançada e ciclo fechado

export type Criticality = 'alta' | 'media' | 'baixa';
export type Persona = 'pcm' | 'fiscal' | 'logistica';
export type SealState = 'integro' | 'violado' | 'autorizado' | 'sem_lacre';

export type Supplier = {
  id: string;
  name: string;
  city: string;
  uf: string;
  contact: string;
  email: string;
  phone: string;
  lat: number;
  lng: number;
};

export type Invoice = {
  number: string;
  series: string;
  issuedAt: string;
  cfop: string;
  value: number;
  accessKey?: string;
  protocol?: string;
};

export type HistoryEntry = { at: string; who: string; text: string; tone?: 'info' | 'success' | 'warning' | 'critical' };

export type PortfolioAsset = {
  id: string; // código do ativo (patrimônio ou identificação interna)
  name: string;
  om: string; // ordem de manutenção no SAP
  area: string; // área requisitante
  owner: string; // analista de PCM responsável
  criticality: Criticality;
  photo: string | null; // data URL da foto tirada no cadastro
  tag: string | null;
  sealApplied: boolean;
  stage: Stage;
  supplierId: string | null;
  quote: number | null; // valor estimado do reparo, em reais
  agreedDays: number | null; // prazo de reparo combinado com a oficina
  problem: string;
  createdAt: string;
  nfRequestedAt: string | null;
  remittance: Invoice | null; // NF de remessa para conserto
  pickupRequestedAt: string | null;
  departedAt: string | null;
  arrivedSupplierAt: string | null;
  dueAt: string | null; // data prometida de retorno
  returnStartedAt: string | null;
  arrivedPlantAt: string | null;
  returnInvoice: Invoice | null;
  closedAt: string | null;
  seal: SealState;
  battery: number | null;
  lastSignalAt: string | null;
  position: { lat: number; lng: number; label: string; at: string } | null;
  lastFollowUpAt: string | null; // última cobrança registrada ao fornecedor
  history: HistoryEntry[];
};

export type Notice = { id: string; to: Persona; at: string; assetId: string; text: string; read: boolean };

export type PortfolioState = { now: string; assets: PortfolioAsset[]; suppliers: Supplier[]; notices: Notice[]; freeTags: string[] };
