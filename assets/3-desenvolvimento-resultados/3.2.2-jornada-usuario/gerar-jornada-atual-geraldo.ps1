param([string]$Destino = $PSScriptRoot)
Add-Type -AssemblyName System.Drawing
$ErrorActionPreference = 'Stop'
$w=2940; $h=2470
$bmp=[System.Drawing.Bitmap]::new($w,$h)
$g=[System.Drawing.Graphics]::FromImage($bmp)
$g.SmoothingMode='AntiAlias'; $g.TextRenderingHint='AntiAliasGridFit'
$svg=[System.Text.StringBuilder]::new()
[void]$svg.AppendLine('<svg xmlns="http://www.w3.org/2000/svg" width="2940" height="2470" viewBox="0 0 2940 2470"><title>Jornada atual de Geraldo — ciclo de manutenção externa</title><desc>Seis fases com ações, pensamentos, contatos, sentimentos e oportunidades e curva emocional ilustrativa.</desc>')

# Fontes do guia de estilos, carregadas apenas para esta renderização.
$fontes=[System.Drawing.Text.PrivateFontCollection]::new()
Get-ChildItem (Join-Path $PSScriptRoot 'fontes') -Filter '*.ttf' | ForEach-Object { $fontes.AddFontFile($_.FullName) }
$cssFontes=[System.Text.StringBuilder]::new()
$dados=[Convert]::ToBase64String([System.IO.File]::ReadAllBytes((Join-Path $PSScriptRoot 'fontes/atkinson-hyperlegible-400.ttf')))
[void]$cssFontes.AppendLine("@font-face{font-family:'Atkinson Hyperlegible';font-weight:400;src:url(data:font/ttf;base64,$dados) format('truetype');}")
$dados=[Convert]::ToBase64String([System.IO.File]::ReadAllBytes((Join-Path $PSScriptRoot 'fontes/atkinson-hyperlegible-700.ttf')))
[void]$cssFontes.AppendLine("@font-face{font-family:'Atkinson Hyperlegible';font-weight:700;src:url(data:font/ttf;base64,$dados) format('truetype');}")
$dados=[Convert]::ToBase64String([System.IO.File]::ReadAllBytes((Join-Path $PSScriptRoot 'fontes/fredoka-600.ttf')))
[void]$cssFontes.AppendLine("@font-face{font-family:'Fredoka';font-weight:600;src:url(data:font/ttf;base64,$dados) format('truetype');}")
$dados=[Convert]::ToBase64String([System.IO.File]::ReadAllBytes((Join-Path $PSScriptRoot 'fontes/jetbrains-mono-400.ttf')))
[void]$cssFontes.AppendLine("@font-face{font-family:'JetBrains Mono';font-weight:400;src:url(data:font/ttf;base64,$dados) format('truetype');}")
[void]$svg.AppendLine("<defs><style>$cssFontes</style></defs>")
function Cor($c) { [System.Drawing.ColorTranslator]::FromHtml($c) }
function Ret($x,$y,$a,$b,$c) {
 $brush=[System.Drawing.SolidBrush]::new((Cor $c)); $g.FillRectangle($brush,$x,$y,$a,$b); $brush.Dispose()
 [void]$svg.AppendLine("<rect x='$x' y='$y' width='$a' height='$b' fill='$c'/>")
}
function Linha($x,$y,$a,$b,$c='#D6D1CC',$esp=1) {
 $pen=[System.Drawing.Pen]::new((Cor $c),$esp); $pen.StartCap='Round'; $pen.EndCap='Round'; $g.DrawLine($pen,$x,$y,$a,$b); $pen.Dispose()
 [void]$svg.AppendLine("<line x1='$x' y1='$y' x2='$a' y2='$b' stroke='$c' stroke-width='$esp' stroke-linecap='round'/>")
}
function Bola($x,$y,$r,$c) {
 $brush=[System.Drawing.SolidBrush]::new((Cor $c)); $g.FillEllipse($brush,($x-$r),($y-$r),($r*2),($r*2)); $brush.Dispose()
 [void]$svg.AppendLine("<circle cx='$x' cy='$y' r='$r' fill='$c'/>")
}
function Texto($s,$x,$y,$larg=265,$tam=20,$c='#161414',$negrito=$false,$familia='Atkinson Hyperlegible') {
 # Aumento uniforme de cinco pontos em todos os textos do mapa.
 $tam += 5
 $estilo=if($negrito){[System.Drawing.FontStyle]::Bold}else{[System.Drawing.FontStyle]::Regular}
 $face=$fontes.Families | Where-Object { $_.Name -like "$familia*" } | Select-Object -First 1
 if(-not $face){throw "Fonte ausente: $familia"}
 $font=[System.Drawing.Font]::new($face,$tam,$estilo,[System.Drawing.GraphicsUnit]::Pixel)
 $brush=[System.Drawing.SolidBrush]::new((Cor $c)); $linha=''; $pos=$y
 foreach($palavra in $s.Split(' ')) {
  $nova=if($linha){"$linha $palavra"}else{$palavra}
  if($linha -and $g.MeasureString($nova,$font).Width -gt $larg) {
   $g.DrawString($linha,$font,$brush,[float]$x,[float]$pos)
   $esc=[System.Security.SecurityElement]::Escape($linha)
   [void]$svg.AppendLine("<text x='$x' y='$($pos+$tam)' font-family='$familia, sans-serif' font-size='$tam' font-weight='$(if($familia -eq 'Fredoka'){600}elseif($negrito){700}else{400})' fill='$c'>$esc</text>")
   $pos+=$tam*1.4; $linha=$palavra
  }else{$linha=$nova}
 }
 if($linha){
  $g.DrawString($linha,$font,$brush,[float]$x,[float]$pos)
  $esc=[System.Security.SecurityElement]::Escape($linha)
  [void]$svg.AppendLine("<text x='$x' y='$($pos+$tam)' font-family='$familia, sans-serif' font-size='$tam' font-weight='$(if($familia -eq 'Fredoka'){600}elseif($negrito){700}else{400})' fill='$c'>$esc</text>")
 }
 $font.Dispose(); $brush.Dispose()
}
# Ícones vetoriais próprios, sem dependência de fontes de símbolos.
function Icone($tipo,$x,$y) {
 $c='#C8232C'
 switch($tipo){
  0 { Linha $x ($y+16) ($x+28) ($y+16) $c 3; Linha ($x+18) ($y+6) ($x+28) ($y+16) $c 3; Linha ($x+18) ($y+26) ($x+28) ($y+16) $c 3 }
  1 { Bola ($x+15) ($y+12) 12 $c; Ret ($x+9) ($y+23) 12 5 $c; Linha ($x+9) ($y+32) ($x+21) ($y+32) $c 3 }
  2 { Bola ($x+4) ($y+16) 5 $c; Bola ($x+26) ($y+4) 5 $c; Bola ($x+26) ($y+28) 5 $c; Linha ($x+4) ($y+16) ($x+26) ($y+4) $c 2; Linha ($x+4) ($y+16) ($x+26) ($y+28) $c 2 }
  3 { Bola ($x+16) ($y+16) 16 $c; Bola ($x+11) ($y+12) 2 '#FFFFFF'; Bola ($x+21) ($y+12) 2 '#FFFFFF'; Linha ($x+9) ($y+22) ($x+23) ($y+22) '#FFFFFF' 2 }
  4 { Linha ($x+16) $y $x ($y+30) $c 3; Linha $x ($y+30) ($x+32) ($y+30) $c 3; Linha ($x+32) ($y+30) ($x+16) $y $c 3; Linha ($x+16) ($y+9) ($x+16) ($y+20) $c 3; Bola ($x+16) ($y+25) 2 $c }
  5 { Linha ($x+16) $y ($x+16) ($y+32) $c 3; Linha $x ($y+16) ($x+32) ($y+16) $c 3; Bola ($x+16) ($y+16) 5 $c }
 }
}
$fases=@(
 @('01 · Preparar','Antes da saída','Recebe a demanda e o motor; confere identificação e dados na planilha disponível.','O produto e os dados estão certos? Preciso combinar embalagem e transporte.','Motor AT-001; mecânico; OM/SAP; planilha; transportadora. Embalagem: responsável a validar.','Atenção e dúvida sobre os dados.','Unificar identificação, responsável e dados do ativo.'),
 @('02 · Escolher oficina','Antes da saída','Avalia o reparo, cota fornecedores, escolhe a Oficina Skibidy e abre a solicitação no SAP.','A oficina consegue reparar e cumprir o prazo?','PCM; fornecedores; Oficina Skibidy; solicitação no SAP.','Cautela na escolha.','Reunir histórico de cumprimento dos fornecedores.'),
 @('03 · Aprovar e enviar','D0 = emissão da NF','Gestor aprova; Fiscal emite a NF; Geraldo imprime, junta ao motor e entrega à transportadora.','A NF e o destino conferem? A coleta foi combinada?','Gestor; Fiscal; PCM; SAP/Gama Soft; NF; transportadora terceirizada.','Alívio parcial após a saída.','Associar ativo, NF, destino e prazo.'),
 @('04 · Acompanhar reparo','D+n · após a saída','Busca confirmar a chegada; cobra status e previsão por telefone/e-mail e atualiza os controles disponíveis.','O motor chegou? A data será cumprida? Estou esquecendo algum item?','Oficina; transportadora; telefone/e-mail; planilha/SharePoint; SAP; régua fiscal. Confirmação de chegada: hipótese.','Insegurança e frustração com cobranças.','Dar visibilidade aos itens fora e às pendências.'),
 @('05 · Tratar atraso','D+n · se houver atraso','Decide entre trazer de volta, usar sobressalente ou prorrogar; envolve o Fiscal na regularização.','Qual alternativa reduz o impacto? Há tempo para regularizar a remessa?','Fornecedor; PCM; Fiscal; NF/SAP; telefone/e-mail; sobressalente, se disponível.','Frustração com falta de dados para decidir.','Reunir prazo e histórico para decidir antes do vencimento.'),
 @('06 · Retornar e encerrar','D+r · retorno e baixa','Transportadora devolve; Geraldo confere e avisa o Fiscal. O Fiscal lança a NF de retorno e dá baixa; Geraldo confirma.','É o mesmo motor? Está conforme? O Fiscal lançou a NF de retorno?','Transportadora; motor; PCM; Fiscal; NF de retorno; SAP/Gama Soft.','Alívio condicionado à confirmação fiscal.','Distinguir retorno físico, conferência e baixa fiscal.')
)
# Calcula cada linha pelo maior texto entre as seis colunas, com margem de 16 px.
$fonteMedida=[System.Drawing.Font]::new(($fontes.Families | Where-Object Name -EQ 'Atkinson Hyperlegible' | Select-Object -First 1),34,[System.Drawing.FontStyle]::Regular,[System.Drawing.GraphicsUnit]::Pixel)
$alturas=@(); $posicoes=@(); $proximoY=420
for($j=0;$j -lt 5;$j++){
 $maior=128
 foreach($fase in $fases){
  $linha=''; $quantidade=1
  foreach($palavra in $fase[$j+2].Split(' ')){
   $nova=if($linha){"$linha $palavra"}else{$palavra}
   if($linha -and $g.MeasureString($nova,$fonteMedida).Width -gt 414){ $quantidade++; $linha=$palavra }else{ $linha=$nova }
  }
  $maior=[Math]::Max($maior,[Math]::Ceiling($quantidade*34*1.4+32))
 }
 $posicoes+= $proximoY; $alturas+= $maior; $proximoY+= $maior
}
$fonteMedida.Dispose()
$fimTabela=$proximoY; $tituloCurva=$fimTabela+30; $centroCurva=$tituloCurva+180
$legendaY=$centroCurva+120; $valesY=$legendaY+50; $picosY=$valesY+50
$h=$picosY+70
$g.Dispose(); $bmp.Dispose()
$bmp=[System.Drawing.Bitmap]::new($w,$h); $g=[System.Drawing.Graphics]::FromImage($bmp)
$g.SmoothingMode='AntiAlias'; $g.TextRenderingHint='AntiAliasGridFit'
[void]$svg.Replace('height="2470" viewBox="0 0 2940 2470"',"height='$h' viewBox='0 0 2940 $h'")
Ret 0 0 $w $h '#F7F4F1'
Ret 0 0 $w 180 '#161414'
Texto 'Geraldo — analista de PCM / jornada atual' 40 20 2700 64 '#C8232C' $false 'Fredoka'
Texto 'Da demanda de reparo externo à conferência e regularização fiscal' 42 110 2800 37 '#F7F4F1'
Texto 'Cenário fictício: motor AT-001 → Oficina Skibidy | AS IS: SAP, planilhas, telefone e e-mail' 40 206 2860 31
Texto 'D0 = emissão da NF; D+n e D+r são marcos relativos, sem duração observada. Embalagem e confirmação de chegada: validar com o parceiro.' 40 252 2860 29
$inicio=310; $cab=110; $rotulos=@('Ação','Pensa','Touchpoints','Sente','Oportunidades')
Ret 0 $inicio 240 ($fimTabela-$inicio) '#F7F4F1'
for($i=0;$i -lt 6;$i++){
 $x=240+$i*450
 Ret $x $inicio 450 $cab '#C8232C'
 Texto $fases[$i][0] ($x+16) ($inicio+16) 414 30 '#FFFFFF' $false 'Fredoka'
 Texto $fases[$i][1] ($x+16) ($inicio+61) 414 24 '#F7F4F1' $false 'JetBrains Mono'
 for($j=0;$j -lt 5;$j++){
  $y=$posicoes[$j]
  $fundo=if($j -eq 4){'#F7F4F1'}elseif($i%2 -eq 0){'#FFFFFF'}else{'#F7F4F1'}
  Ret $x $y 450 $alturas[$j] $fundo
  Texto $fases[$i][$j+2] ($x+16) ($y+16) 414 29
  Linha $x $y ($x+450) $y
 }
 Linha $x $inicio $x $fimTabela
}
for($j=0;$j -lt 5;$j++){
 $y=$posicoes[$j]
 Icone $(if($j -eq 4){5}else{$j}) 22 ($y+25)
 Texto $rotulos[$j] 20 ($y+73) 220 24 '#161414' $true
}
Texto 'CURVA EMOCIONAL · hipótese ilustrativa, sem medição com usuários' 30 $tituloCurva 2800 30 '#C8232C' $false 'Fredoka'
$valores=@(0,0,1,-2,-2,2)
for($v=-2;$v -le 2;$v++){
 $y=$centroCurva-$v*48
 Linha 240 $y 2900 $y '#D6D1CC'
 Texto "$(if($v -gt 0){'+'})$v" 140 ($y-16) 80 24
}
for($i=0;$i -lt 6;$i++){
 $x=465+$i*450; $y=$centroCurva-$valores[$i]*48
 if($i -gt 0){ Linha ($x-450) ($centroCurva-$valores[$i-1]*48) $x $y '#C8232C' 4 }
 Bola $x $y 7 '#C8232C'
}
Texto '−2 frustração intensa | −1 insegurança | 0 atenção | +1 alívio parcial | +2 alívio com encerramento' 30 $legendaY 2870 29
Texto 'Vales: cobrança e esquecimento (04); atraso sem dados para decidir (05).' 30 $valesY 2870 29
Texto 'Picos: alívio após aprovação e envio (03); retorno conferido e baixa fiscal confirmada (06).' 30 $picosY 2870 29
[void]$svg.AppendLine('</svg>')
[System.IO.Directory]::CreateDirectory($Destino) | Out-Null
[System.IO.File]::WriteAllText((Join-Path $Destino 'jornadaAtualGeraldo.svg'),$svg.ToString(),[System.Text.UTF8Encoding]::new($false))
$bmp.Save((Join-Path $Destino 'jornadaAtualGeraldo.png'),[System.Drawing.Imaging.ImageFormat]::Png)
$g.Dispose(); $bmp.Dispose(); $fontes.Dispose()
Write-Output "SVG e PNG gerados; alturas das linhas: $($alturas -join ', '); altura total: $h."
