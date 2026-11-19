import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createPortfolio } from '../src/portfolio/seed.ts';
import {
  cfopFor, dueRemaining, flagsOf, isOpen, isOverdue, legalLimitDays, legalRemaining, maskNf, nextAction, normalizeNf, sortByAttention, stageCounts, validNf,
} from '../src/portfolio/logic.ts';

const NOW = '2026-11-18T15:00:00Z';
const s = createPortfolio(NOW);
const sup = id => s.suppliers.find(x => x.id === id);

test('Carteira de demonstração tem 70 ativos abertos e é determinística', () => {
  assert.equal(s.assets.filter(isOpen).length, 70);
  assert.deepEqual(createPortfolio(NOW).assets.map(a => a.id), s.assets.map(a => a.id));
  assert.equal(new Set(s.assets.map(a => a.id)).size, s.assets.length);
});

test('Toda etapa do processo tem ativos e as contagens somam o total', () => {
  const c = stageCounts(s.assets);
  for (const v of Object.values(c)) assert.ok(v > 0);
  assert.equal(Object.values(c).reduce((x, y) => x + y, 0), s.assets.length);
});

test('NF vira maiúscula, perde caracteres inválidos e respeita 15 caracteres', () => {
  assert.equal(normalizeNf('nf-abc 12/3'), 'NF-ABC12/3');
  assert.equal(normalizeNf('a'.repeat(20)).length, 15);
  assert.ok(validNf('NF-000123'));
  assert.ok(!validNf('nf-000123'));
  assert.equal(maskNf('NF-000123'), 'NF ••0123');
});

test('Prazo legal: 270 dias em GO e 360 fora; CFOP interno e interestadual', () => {
  assert.equal(legalLimitDays(sup('SUP-01')), 270);
  assert.equal(legalLimitDays(sup('SUP-02')), 360);
  assert.equal(cfopFor('remessa', sup('SUP-01')), '5.915');
  assert.equal(cfopFor('retorno', sup('SUP-02')), '6.916');
});

test('Atraso só existe a caminho ou na oficina e depois da data prometida', () => {
  for (const a of s.assets) {
    if (isOverdue(a, NOW)) {
      assert.ok(['ida', 'oficina'].includes(a.stage));
      assert.ok(dueRemaining(a, NOW) < 0);
    }
  }
  assert.ok(s.assets.some(a => isOverdue(a, NOW)));
});

test('Ativo encerrado não tem prazo legal correndo nem pendências', () => {
  const closed = s.assets.find(a => a.stage === 'encerrado');
  assert.equal(legalRemaining(closed, NOW, s.suppliers), null);
  assert.equal(closed.tag, null);
});

test('Lacre violado vem primeiro na fila de atenção', () => {
  const first = sortByAttention(s.assets.filter(isOpen), NOW, s.suppliers)[0];
  const flags = flagsOf(first, NOW, s.suppliers);
  assert.ok(flags.includes('critico'));
});

test('Próxima ação segue o dono de cada etapa', () => {
  const by = st => s.assets.find(a => a.stage === st);
  assert.equal(nextAction(by('aguardando_nf'), NOW).persona, 'fiscal');
  assert.equal(nextAction(by('na_usina'), NOW).key, 'return_nf');
  assert.equal(nextAction(by('oficina'), NOW).persona, 'pcm');
});

test('Cada etapa coloca o ativo na mesa de uma pessoa', async () => {
  const { ownerOf, deskCount } = await import('../src/portfolio/logic.ts');
  const by = st => s.assets.find(a => a.stage === st);
  assert.equal(ownerOf(by('planejamento')), 'pcm');
  assert.equal(ownerOf(by('aguardando_nf')), 'fiscal');
  assert.equal(ownerOf(by('ida')), 'logistica');
  assert.equal(ownerOf(by('na_usina')), 'fiscal');
  assert.equal(ownerOf(by('encerrado')), null);
  const open = s.assets.filter(isOpen).length;
  assert.equal(deskCount(s.assets, 'pcm') + deskCount(s.assets, 'fiscal') + deskCount(s.assets, 'logistica'), open);
});
