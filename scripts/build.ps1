param([ValidateSet('demo', 'weekly', 'bancada', 'bancada-alternada')][string]$Environment = 'demo')
. (Join-Path $PSScriptRoot 'common.ps1')

$previousSdk = $env:WASI_SDK_PATH
Push-Location $ProjectRoot
try {
    $pio = Get-PlatformIOExecutable
    # A placa fisica usa o NEO-6M real e dispensa o chip customizado do simulador.
    if ($Environment -notlike 'bancada*') {
        $wokwi = Join-Path $ToolsDir 'wokwi-cli.exe'
        if (!(Test-Path -LiteralPath $wokwi)) { throw 'Execute scripts/setup.ps1 primeiro.' }
        $env:WASI_SDK_PATH = Join-Path $ToolsDir 'wasi-sdk'
        Invoke-Checked $wokwi @('chip', 'compile', 'chips/gps.chip.c', '-o', 'chips/gps.chip.wasm')
    }
    Invoke-Checked $pio @('run', '-e', $Environment)
    Write-Host "Compilacao pronta: $Environment."
    if ($Environment -eq 'weekly') {
        Write-Host 'Selecione src/firmware/prototipo_wokwi/wokwi.weekly.toml com Wokwi: Select Config File antes de simular.'
    }
} finally {
    $env:WASI_SDK_PATH = $previousSdk
    Pop-Location
}
