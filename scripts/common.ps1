$ErrorActionPreference = 'Stop'
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ToolsDir = Join-Path $ProjectRoot '.tools'

function Get-PlatformIOExecutable {
    $localExecutable = Join-Path $ToolsDir 'venv\Scripts\platformio.exe'
    if (Test-Path -LiteralPath $localExecutable) { return $localExecutable }
    $existingExecutable = Join-Path $env:USERPROFILE '.platformio\penv\Scripts\platformio.exe'
    if (Test-Path -LiteralPath $existingExecutable) { return $existingExecutable }
    foreach ($name in @('pio', 'platformio')) {
        $command = Get-Command $name -ErrorAction SilentlyContinue
        if ($command) { return $command.Source }
    }
    throw 'PlatformIO ausente. Execute scripts/setup.ps1 primeiro.'
}

function Invoke-Checked {
    param([string]$Executable, [string[]]$Arguments)
    & $Executable @Arguments
    if ($LASTEXITCODE -ne 0) { throw "Falha no comando: $Executable (codigo $LASTEXITCODE)." }
}

function Get-PythonExecutable {
    # Reutiliza o Python do PlatformIO mesmo quando python.exe nao esta no PATH.
    $candidates = @(
        (Join-Path $ToolsDir 'venv\Scripts\python.exe'),
        (Join-Path $env:USERPROFILE '.platformio\penv\Scripts\python.exe')
    )
    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate) { return $candidate }
    }
    $command = Get-Command python.exe -ErrorAction SilentlyContinue
    if ($command) { return $command.Source }
    throw 'Python ausente. Execute scripts/setup.ps1 primeiro.'
}
