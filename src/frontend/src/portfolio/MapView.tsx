import { useEffect, useRef, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import maplibregl from 'maplibre-gl';
import 'maplibre-gl/dist/maplibre-gl.css';
import { PLANT } from './seed';
import { statusOf } from './ui';
import { usePortfolio } from './store';
import { supplierOf } from './logic';
import type { PortfolioAsset } from './types';

/**
 * Mapa real (OpenStreetMap) da região da usina em Goiás e das oficinas.
 * Os tiles vêm da internet; sem rede ou sem WebGL, a tela mostra um aviso e a lista continua valendo.
 * As posições dos ativos são fictícias.
 */
export function MapView({ assets, height = 420 }: { assets: PortfolioAsset[]; height?: number }) {
  const { state } = usePortfolio();
  const el = useRef<HTMLDivElement>(null);
  const map = useRef<maplibregl.Map | null>(null);
  const markers = useRef<maplibregl.Marker[]>([]);
  const [error, setError] = useState('');
  const nav = useNavigate();
  const navRef = useRef(nav);
  const fitted = useRef(false);
  navRef.current = nav;

  useEffect(() => {
    if (!el.current) return;
    try {
      const m = new maplibregl.Map({
        container: el.current,
        style: {
          version: 8,
          sources: { osm: { type: 'raster', tiles: ['https://tile.openstreetmap.org/{z}/{x}/{y}.png'], tileSize: 256, attribution: '© OpenStreetMap' } },
          layers: [{ id: 'osm', type: 'raster', source: 'osm', paint: { 'raster-saturation': -0.85, 'raster-contrast': -0.05 } }],
        },
        center: [PLANT.lng + 2.5, PLANT.lat - 1],
        zoom: 5.6,
        attributionControl: { compact: true },
        cooperativeGestures: true,
      });
      m.addControl(new maplibregl.NavigationControl({ showCompass: false }), 'top-right');
      m.on('error', () => setError('Não foi possível carregar o mapa base. Confira a conexão com a internet.'));
      map.current = m;
      return () => {
        markers.current.forEach(x => x.remove());
        m.remove();
        map.current = null;
      };
    } catch {
      setError('Este navegador não abriu o mapa (WebGL indisponível).');
    }
  }, []);

  useEffect(() => {
    const m = map.current;
    if (!m) return;
    markers.current.forEach(x => x.remove());
    markers.current = [];
    const add = (node: HTMLElement, lng: number, lat: number, popup?: string) => {
      const mk = new maplibregl.Marker({ element: node }).setLngLat([lng, lat]);
      if (popup) mk.setPopup(new maplibregl.Popup({ offset: 14, closeButton: false }).setHTML(popup));
      mk.addTo(m);
      markers.current.push(mk);
    };
    // Oficinas: círculo terra com a quantidade de ativos em reparo.
    const atSupplier = new Map<string, PortfolioAsset[]>();
    for (const a of assets) if (a.stage === 'oficina' && a.supplierId) atSupplier.set(a.supplierId, [...(atSupplier.get(a.supplierId) ?? []), a]);
    for (const s of state.suppliers) {
      const list = atSupplier.get(s.id) ?? [];
      const alerts = list.filter(a => statusOf(a, state.now, state.suppliers).tone === 'alert').length;
      const node = document.createElement('div');
      node.className = `mk-supplier ${alerts ? 'has-alert' : ''}`;
      node.style.setProperty('--s', `${28 + Math.sqrt(list.length) * 7}px`);
      node.innerHTML = `<b>${list.length}</b>${alerts ? `<i>${alerts}</i>` : ''}`;
      add(node, s.lng, s.lat, `<strong>${s.name}</strong><br>${s.city}/${s.uf}<br>${list.length} em reparo${alerts ? ` · <span class="t-alert">${alerts} com alerta</span>` : ''}`);
    }
    // Ativos na estrada: caminhão; vermelho se houver alerta.
    for (const a of assets.filter(x => (x.stage === 'ida' || x.stage === 'retorno') && x.position)) {
      const tone = statusOf(a, state.now, state.suppliers).tone;
      const node = document.createElement('button');
      node.className = `mk-truck ${tone === 'alert' ? 'alert' : ''}`;
      node.setAttribute('aria-label', `${a.name}, ${a.stage === 'ida' ? 'indo para' : 'voltando de'} ${supplierOf(a, state.suppliers)?.city ?? ''}`);
      node.innerHTML = a.stage === 'ida' ? '→' : '←';
      node.onclick = () => navRef.current(`/ativos/${a.id}`);
      add(node, a.position!.lng, a.position!.lat);
    }
    // Usina: a origem e o destino final.
    const plant = document.createElement('div');
    plant.className = 'mk-plant';
    plant.innerHTML = '<span>Usina</span>';
    if (!fitted.current) {
      const b = new maplibregl.LngLatBounds([PLANT.lng, PLANT.lat], [PLANT.lng, PLANT.lat]);
      for (const s of state.suppliers) b.extend([s.lng, s.lat]);
      m.fitBounds(b, { padding: 48, duration: 0 });
      fitted.current = true;
    }
    add(plant, PLANT.lng, PLANT.lat, `<strong>${PLANT.label}</strong><br>${assets.filter(a => a.stage === 'na_usina').length} aguardando baixa fiscal`);
  }, [assets, state.suppliers, state.now]);

  return (
    <div className="map-wrap" style={{ height }}>
      <div ref={el} className="map-el" role="region" aria-label="Mapa da usina, das oficinas e dos ativos em trânsito" />
      {error && <p className="map-error">{error}</p>}
    </div>
  );
}
