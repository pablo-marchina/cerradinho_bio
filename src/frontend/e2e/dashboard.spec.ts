import {test,expect} from '@playwright/test';
test('visão geral, pesquisa e detalhe',async({page})=>{await page.goto('/');await expect(page.getByRole('heading',{name:'Visão geral'})).toBeVisible();await expect(page.getByText('Modo demonstração.')).toBeVisible();await page.goto('/equipamentos?q=GT-0042');await expect(page.locator('a:visible').filter({hasText:'Motor trifásico'}).first()).toBeVisible();await page.locator('a:visible').filter({hasText:'Motor trifásico'}).first().click();await expect(page.getByRole('heading',{name:'Motor trifásico'})).toBeVisible();await expect(page.getByRole('heading',{name:'Última posição válida'})).toBeVisible();});
test('comando sem resposta não apresenta execução',async({page})=>{await page.goto('/dispositivos');await expect(page.getByRole('heading',{name:'Central de comandos'})).toBeVisible();await page.locator('.gt-form select').first().selectOption('authorize');await page.getByLabel('Resposta de teste').selectOption('timeout');await page.getByLabel('Justificativa').fill('Teste de ausência de resposta');await page.getByRole('checkbox',{name:/Confirmo/}).check();await page.getByRole('button',{name:'Enviar comando simulado'}).click();await expect(page.getByText('Nenhuma resposta simulada; nenhuma alteração aplicada.')).toBeVisible({timeout:12000});await expect(page.getByText('Executado (simulado)',{exact:true})).toHaveCount(0);});
test('GNSS sem fix preserva posição válida',async({page})=>{await page.goto('/dispositivos');await expect(page.getByText('Ponto A (simulado)')).toBeVisible();await page.getByLabel('Estímulo').selectOption('no_fix');await page.getByRole('button',{name:'Aplicar estímulo'}).click();await expect(page.locator('.status').getByText('GNSS sem fix',{exact:true})).toBeVisible();await expect(page.getByText('Ponto A (simulado)')).toBeVisible();});
test('layout sem overflow global nas sete rotas',async({page})=>{for(const path of ['/','/equipamentos','/alertas','/mapa','/ciclos','/dispositivos','/relatorios']){await page.goto(path);await expect(page.locator('main h1')).toBeVisible();await expect(page.getByText('Modo demonstração.')).toBeVisible();expect(await page.evaluate(()=>document.documentElement.scrollWidth<=window.innerWidth+1)).toBeTruthy();}});

test('bateria na lista e gráfico de alertas com dados textuais',async({page},testInfo)=>{
  await page.goto('/equipamentos');
  if(testInfo.project.name==='mobile'){
    await expect(page.locator('.mobile-assets').getByText(/Bateria: (\d+%|Sem leitura|Falha)/).first()).toBeVisible();
  }else{
    await expect(page.locator('.table-wrap th').getByText('Bateria')).toBeVisible();
    await expect(page.locator('.table-wrap td').getByText(/^(\d+%|Sem leitura|Falha)$/).first()).toBeVisible();
  }
  await page.goto('/relatorios');
  const alerts=page.locator('.card').filter({has:page.getByRole('heading',{name:'Alertas por tipo'})});
  await expect(alerts.locator('svg.recharts-surface')).toBeVisible();
  await alerts.getByText('Dados do gráfico').click();
  await expect(alerts.getByText(/Violação de lacre: /).first()).toBeVisible();
});


test('cadastro sem ciclo não aparece entre ciclos encerrados',async({page})=>{
  await page.goto('/equipamentos');
  await page.getByLabel('ID do ativo',{exact:true}).fill('AT-0100');
  await page.getByLabel('Nome',{exact:true}).fill('Equipamento novo sem ciclo');
  await page.getByLabel('TAG inicial',{exact:true}).fill('GT-0100');
  await page.getByRole('button',{name:'Cadastrar (simulado)',exact:true}).click();
  await expect(page.getByText('Equipamento criado.',{exact:false})).toBeVisible();

  // Navegue pela aplicação: recarregar descartaria o equipamento recém-cadastrado.
  const menu=page.getByRole('button',{name:'Abrir menu',exact:true});
  if(await menu.isVisible())await menu.click();
  await page.getByRole('link',{name:'Ciclos e retornos',exact:true}).click();
  const asset=page.getByRole('link',{name:'Equipamento novo sem ciclo',exact:true});
  await expect(asset).toBeVisible();
  await expect(page.getByText('Sem ciclo',{exact:true})).toBeVisible();
  await page.locator('.filter-field select').first().selectOption('closed');
  await expect(asset).toHaveCount(0);
  await page.locator('.filter-field select').first().selectOption('all');
  await expect(asset).toBeVisible();
});

test('formulário de autorização inicia com validade de três dias',async({page})=>{
  await page.goto('/dispositivos');
  await page.locator('.filter-field select').first().selectOption('AT-0055');
  await page.locator('.gt-form select').first().selectOption('authorize');
  await expect(page.getByLabel('Validade simulada (min)',{exact:true})).toHaveValue('4320');
});

test('reaplicação recusa lacre aberto e só confirma após fechamento',async({page})=>{
  await page.goto('/dispositivos');
  const seal=page.locator('.card').filter({has:page.getByRole('heading',{name:'Lacre',exact:true})});
  const latestCommand=page.locator('.command-record').first();
  await page.getByLabel('Estímulo').selectOption('reconnect');
  await page.getByRole('button',{name:'Aplicar estímulo',exact:true}).click();
  await expect(page.getByText('Conectado ao broker · simulado',{exact:true})).toBeVisible();
  await page.getByLabel('Estímulo').selectOption('open_seal');
  await page.getByRole('button',{name:'Aplicar estímulo',exact:true}).click();
  await expect(seal.getByText('Violado',{exact:true})).toBeVisible();

  await page.locator('.gt-form select').first().selectOption('reapply');
  await page.getByLabel('Justificativa',{exact:true}).fill('Conferência do lacre de demonstração');
  await page.getByRole('button',{name:'Enviar comando simulado',exact:true}).click();
  await expect(latestCommand.getByText('Feche o lacre antes de confirmar a reaplicação.',{exact:true})).toBeVisible({timeout:12000});
  await expect(seal.getByText('Violado',{exact:true})).toBeVisible();
  await expect(page.getByText('Executado (simulado)',{exact:true})).toHaveCount(0);

  await page.getByLabel('Estímulo').selectOption('close_seal');
  await page.getByRole('button',{name:'Aplicar estímulo',exact:true}).click();
  await expect(seal.getByText('Aguardando reaplicação',{exact:true})).toBeVisible();
  await page.getByRole('button',{name:'Enviar comando simulado',exact:true}).click();
  await expect(latestCommand.getByText('Operação aplicada somente no simulador.',{exact:true})).toBeVisible({timeout:12000});
  await expect(seal.getByText('Íntegro',{exact:true})).toBeVisible();
  await expect(page.locator('.command-record')).toHaveCount(2);
});
