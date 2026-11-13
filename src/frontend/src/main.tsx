import React from 'react';
import { createRoot } from 'react-dom/client';
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
import { BrowserRouter } from 'react-router-dom';
import { App } from './app/App';
import './styles/index.css';
async function start(){const root=createRoot(document.getElementById('root')!);try{const {worker}=await import('./mocks/browser');await worker.start({onUnhandledRequest:'bypass',serviceWorker:{url:'/mockServiceWorker.js'}});root.render(<React.StrictMode><QueryClientProvider client={new QueryClient()}><BrowserRouter><App/></BrowserRouter></QueryClientProvider></React.StrictMode>);}catch(e){root.render(<main className="startup"><h1>Não foi possível iniciar a demonstração</h1><p>Verifique se o Service Worker foi gerado por npm install e se o site está em localhost ou HTTPS.</p><p>{e instanceof Error?e.message:''}</p></main>);}}
void start();
