param([ValidateRange(1, 3600)][int]$Seconds = 240)
. (Join-Path $PSScriptRoot 'common.ps1')
Push-Location $ProjectRoot
try {
    $python = Get-PythonExecutable
    Invoke-Checked $python @('scripts/capture-serial.py', '--seconds', "$Seconds")
} finally { Pop-Location }
