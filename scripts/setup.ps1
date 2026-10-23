param([switch]$InstallExtensions)
. (Join-Path $PSScriptRoot 'common.ps1')

Push-Location $ProjectRoot
try {
    New-Item -ItemType Directory -Path $ToolsDir -Force | Out-Null
    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
    try { $pio = Get-PlatformIOExecutable }
    catch {
        $python = Get-Command python -ErrorAction SilentlyContinue
        if (!$python) { throw 'Instale Python 3.10+ e habilite sua entrada no PATH.' }
        Invoke-Checked $python.Source @('-m', 'venv', (Join-Path $ToolsDir 'venv'))
        $pythonLocal = Join-Path $ToolsDir 'venv\Scripts\python.exe'
        Invoke-Checked $pythonLocal @('-m', 'pip', 'install', 'platformio==6.2.0')
        $pio = Get-PlatformIOExecutable
    }

    $wokwi = Join-Path $ToolsDir 'wokwi-cli.exe'
    $expectedHash = 'a0f91a48ba54a91215f62a38041f4da6f6cb093b402f16481e9ab986767473cd'
    if (!(Test-Path -LiteralPath $wokwi)) {
        Write-Host 'Baixando Wokwi CLI v0.29.0 da distribuicao oficial...'
        Invoke-WebRequest -UseBasicParsing -Uri 'https://github.com/wokwi/wokwi-cli/releases/download/v0.29.0/wokwi-cli-win-x64.exe' -OutFile $wokwi
    }
    if ((Get-FileHash -LiteralPath $wokwi -Algorithm SHA256).Hash.ToLowerInvariant() -ne $expectedHash) {
        throw 'A verificacao SHA256 do Wokwi CLI falhou. Nao execute esse arquivo.'
    }

    $sdk = Join-Path $ToolsDir 'wasi-sdk'
    $sdkMarker = Join-Path $sdk '.setup-complete'
    if (!(Test-Path -LiteralPath $sdkMarker)) {
        $archive = Join-Path $ToolsDir 'wasi-sdk-25.0-windows.tar.gz'
        if (!(Test-Path -LiteralPath $archive)) {
            Write-Host 'Baixando WASI SDK 25.0 para compilar o GPS...'
            Invoke-WebRequest -UseBasicParsing -Uri 'https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-25/wasi-sdk-25.0-x86_64-windows.tar.gz' -OutFile $archive
        }
        New-Item -ItemType Directory -Path $sdk -Force | Out-Null
        Invoke-Checked 'tar.exe' @('-xzf', $archive, '-C', $sdk, '--strip-components=1')
        if (!(Test-Path -LiteralPath (Join-Path $sdk 'bin\clang.exe'))) {
            throw 'O pacote WASI SDK nao forneceu clang.exe no caminho esperado.'
        }
        Set-Content -LiteralPath $sdkMarker -Value 'wasi-sdk-25.0-x86_64-windows'
    }
    $apiHeader = Join-Path $ProjectRoot 'chips\wokwi-api.h'
    if (!(Test-Path -LiteralPath $apiHeader)) {
        Invoke-WebRequest -UseBasicParsing -Uri 'https://wokwi.com/api/chips/wokwi-api.h' -OutFile $apiHeader
    }

    Write-Host 'Instalando as dependencias do ESP32 na pasta local .tools...'
    Invoke-Checked $pio @('pkg', 'install', '-e', 'demo', '-e', 'weekly')
    if ($InstallExtensions) {
        Invoke-Checked 'code.cmd' @('--install-extension', 'platformio.platformio-ide')
        Invoke-Checked 'code.cmd' @('--install-extension', 'wokwi.wokwi-vscode')
    }
    Write-Host 'Setup pronto. Execute scripts/build.ps1 e scripts/verify.ps1.'
    Write-Host 'No VS Code, instale as extensoes recomendadas e ative sua propria licenca Wokwi.'
} finally { Pop-Location }
