---
name: apresentacao-guaratracking
description: Criar ou atualizar a apresentação HTML do GuaráTracking (plano visual aprovado, animações por clique, personas Hoje/Com o Guarátracking, placa 3D, roteiro de fala e deploy na Vercel).
---

# Apresentação do GuaráTracking (slides HTML)

Use este processo para montar ou alterar a apresentação de Sprint do GuaráTracking. Ele foi usado na Sprint 1 (roteiro de 4:40, duas pessoas falando). Para o processo genérico, sem o GuaráTracking, veja a skill `apresentacao-html-animada`.

## 1. Planejar antes de construir (estilo frontend-slides)
- Siga o processo de planejamento da skill frontend-slides (github zarazhangrui/frontend-slides): entenda público, tempo de fala e fontes (roteiro, personas, circuito).
- Escreva um **plano visual** curto (um documento): lista de slides, o que cada clique revela e a animação de cada cena. Mostre ao Arthur e **só construa depois da aprovação**.
- Apresentação com pouca animação, pouca simbologia e sem capa foi reprovada. Todo slide precisa de uma cena animada, não só texto.

## 2. Guia de estilos
- Use o "Guaratracking Guia de Estilos" (zip do Claude Design): fontes Fredoka (marca e títulos) e Atkinson (texto); cores guará #C8232C, semente #161414, polpa #F7F4F1. LEDs: verde #43C35A, amarelo #F2C230, vermelho #E0303A, azul #3D7BFF.
- Sem fonte mono e sem cabeçalhos de canto. Logo no topo dos slides internos; "Guará" escuro e "tracking" vermelho.
- Sinal de GPS (sonar): círculos concêntricos centralizados sob as pernas do logo, com `svg { overflow: visible }` para não serem cortados.
- Banners das personas: /mnt/project-files/personas-imagens/ (3840×2160). Fotos redondas: webp com máscara de círculo mais selo, nunca `border-radius` (corta o selo).

## 3. Motor de slides
- Palco fixo 1920×1080 escalado para a janela; cada slide tem `data-passos`.
- Um mapa `PASSOS` liga classes (t1–t4, a1–a4) a cada clique; `LIMPAR` zera ao trocar de slide. As animações andam pela **seta** (teclado e botões), com CSS transitions e keyframes. Respeite `prefers-reduced-motion`.
- O roteiro de fala vem de `<template id="nX">` e abre com o botão Roteiro (tecla N). Atualize o template sempre que mudar o slide.
- Arquivo único e autocontido (imagens em data:image/webp). Para o Artifact, o HTML é embrulhado em `<!doctype html><html lang="pt-BR"><head>…`.

## 4. Ordem dos slides
Capa (logo, "Guarátracking" letra a letra, sonar) → origem (TAPI + kickoff → 3 personas) → para cada persona **dois slides**: "Hoje" (animação do problema, tópicos vermelhos) e "Com o Guarátracking" (animação da solução, tópicos verdes; o último clique mostra o resumo com a Decisão de produto) → transição para os cinco comportamentos (cada um entra com animação) → circuito 3D → encerramento com a equipe em ordem alfabética, agrupada por função, fotos grandes → slide final de Limitação (bateria versus frequência de envio) e Pergunta prioritária (lacre inteligente ou refinar o rastreador) para a Cerradinho.

## 5. Placa 3D
- Three.js r128 + OrbitControls, num IIFE `placa`. Construída por `chave`, `botao`, `fio` (tubo CatmullRom) e `marca` (marcadores HTML). Estado em `st`, ações em `acao(f)`.
- Reproduza o circuito atual da develop: GPS, chave ON/OFF e regras do firmware. LEDs: verde = lacre ok (pisca: retirada autorizada), amarelo pisca = atraso, fixo = permanência, vermelho = violação, azul = sem rede com fila.
- Chaves: 3 (lacre) amarela, ao lado do botão azul; 4 (rede) azul; 5 (ON/OFF) verde. Fios baixos para não cobrir LEDs. Fundo escuro texturizado; LEDs com halo, sem lavar.
- Começa com a chave 5 desligada e **sem ciclo**; o botão azul inicia o ciclo. Botão "↺ Reiniciar" desliga todas as chaves. Um dia por segundo; atraso após o retorno previsto, permanência no limite, status semanal a cada 7 dias.

## 6. Verificar
- Playwright com swiftshader (`--use-gl=swiftshader`), servindo three.js localmente por `route`. Screenshots de cada slide e estado (plano [espera, cliques]); confira cortes, estouro de texto e a placa 3D. Veja as imagens antes de entregar.

## 7. Entrega e deploy
- Publique o HTML como Artifact no mesmo link e salve cópia em /mnt/project-files/apresentacao/.
- Vercel: projeto guaratracking-apresentacao (time guara-tracking, plano Hobby), https://guaratracking-apresentacao.vercel.app. Troque o `index.html` na pasta local `guaratracking-apresentacao` e rode `npx vercel@latest --prod --yes` nela. O deploy em produção é bloqueado para o Claude: **o Arthur roda o comando**; site desatualizado costuma ser cache do navegador.
- Git: commits `tipo(#NN): título`, autor `arthur.loyola`, em branch (nunca na develop), sem coautoria do Claude e sem link da conversa. Sem push sem o OK do Arthur.
