. (Join-Path $PSScriptRoot 'common.ps1')
Push-Location $ProjectRoot
try {
    New-Item -ItemType Directory -Path (Join-Path $ProjectRoot 'build') -Force | Out-Null
    $clang = Join-Path $ToolsDir 'wasi-sdk\bin\clang.exe'
    if (!(Test-Path -LiteralPath $clang)) { throw 'Execute scripts/setup.ps1 primeiro.' }
    Invoke-Checked $clang @('tests/model_tests.c', '-O2', '-Wall', '-Wextra', '-lm', '-o', 'build/model-tests.wasm')
    Invoke-Checked 'node.exe' @('--disable-warning=ExperimentalWarning', 'scripts/run-wasi-tests.cjs', 'build/model-tests.wasm')
    $python = Get-PythonExecutable
    Invoke-Checked $python @('scripts/validate-project.py')
    # Verificador oficial: consulta o catalogo atual de componentes do Wokwi.
    Invoke-Checked (Join-Path $ToolsDir 'wokwi-cli.exe') @('lint', '--warnings-as-errors')
    Write-Host 'PASS: testes locais e verificador oficial do diagrama.'
} finally { Pop-Location }
