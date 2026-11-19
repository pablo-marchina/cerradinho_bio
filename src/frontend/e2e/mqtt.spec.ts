import {test,expect} from '@playwright/test';

test.skip(process.env.MQTT_E2E!=='1','Requer EMQX, simulador e Vite em modo MQTT.');

test('integra status, posição proposta, violação e resposta do simulador MQTT',async({page})=>{
  await page.goto('/');
  await expect(page.getByText('Modo MQTT com dispositivo simulado.',{exact:true})).toBeVisible();
  await expect(page.getByText('1 equipamento com violação de lacre')).toBeVisible({timeout:10000});

  await page.goto('/dispositivos');
  const battery=page.locator('.card').filter({has:page.getByRole('heading',{name:'Bateria',exact:true})});
  await expect(battery.getByText('68%',{exact:true})).toBeVisible();
  await expect(page.getByText('-21.12, -48.52',{exact:true})).toBeVisible({timeout:10000});
  await expect(page.getByText('Conectado ao broker',{exact:true})).toBeVisible();

  await page.locator('.gt-form select').first().selectOption('authorize');
  await page.getByRole('checkbox',{name:/Confirmo/}).check();
  await page.getByRole('button',{name:'Enviar comando ao simulador'}).click();
  const latest=page.locator('.command-record').first();
  await expect(latest.getByText('Executado pelo simulador',{exact:true})).toBeVisible({timeout:10000});
  await expect(latest.getByText('Comando executado e confirmado pelo simulador MQTT.',{exact:true})).toBeVisible();
});
