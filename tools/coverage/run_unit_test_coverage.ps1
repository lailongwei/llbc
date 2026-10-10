param(
    [string]$BuildDir = "build",
    [string]$Configuration = "Debug"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$root = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
$buildPath = (Resolve-Path (Join-Path $root $BuildDir)).Path
$binaryPath = Join-Path $root "output/$Configuration"
$outputPath = Join-Path $root "output"
$reportPath = Join-Path $outputPath "coverage.cobertura.xml"
$settingsPath = Join-Path $outputPath "windows-code-coverage.config"

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio/Installer/vswhere.exe"
if (-not (Test-Path $vswhere)) {
    throw "vswhere.exe was not found"
}

$visualStudio = & $vswhere -latest -products * -property installationPath
if ($LASTEXITCODE -ne 0 -or -not $visualStudio) {
    throw "Visual Studio installation was not found"
}

$coverageTool = Join-Path $visualStudio.Trim() `
    "Common7/IDE/Extensions/Microsoft/CodeCoverage.Console/Microsoft.CodeCoverage.Console.exe"
if (-not (Test-Path $coverageTool)) {
    throw "Microsoft.CodeCoverage.Console.exe was not found at $coverageTool"
}
if (-not (Test-Path $binaryPath)) {
    throw "Coverage binary directory was not found at $binaryPath"
}

New-Item -ItemType Directory -Force -Path $outputPath | Out-Null
$escapedBinaryPath = [System.Security.SecurityElement]::Escape($binaryPath)
@"
<ModulePaths>
  <IncludeDirectories>
    <Directory>$escapedBinaryPath</Directory>
  </IncludeDirectories>
</ModulePaths>
"@ | Set-Content -Path $settingsPath -Encoding utf8

$ctest = (Get-Command ctest -ErrorAction Stop).Source
Write-Host "==> coverage tool : $coverageTool"
Write-Host "==> unit-test dir : $buildPath"
Write-Host "==> report        : $reportPath"

& $coverageTool collect `
    --settings $settingsPath `
    --output $reportPath `
    --output-format cobertura `
    --nologo `
    $ctest `
    --test-dir $buildPath `
    --build-config $Configuration `
    --output-on-failure `
    --no-tests=error
$testResult = $LASTEXITCODE

if (Test-Path $reportPath) {
    $platform = if ($env:COVERAGE_PLATFORM) { $env:COVERAGE_PLATFORM } else { "Windows" }
    $compiler = if ($env:COVERAGE_COMPILER) { $env:COVERAGE_COMPILER } else { "MSVC" }
    $summaryPath = if ($env:COVERAGE_SUMMARY_FILE) {
        $env:COVERAGE_SUMMARY_FILE
    } else {
        Join-Path $outputPath "coverage-summary.json"
    }
    $summaryScript = Join-Path $PSScriptRoot "coverage_summary.py"
    & python $summaryScript summarize `
        --input $reportPath `
        --format cobertura `
        --source-root $root `
        --markers-dir (Join-Path $root "tests/unit_test") `
        --platform $platform `
        --compiler $compiler `
        --backend microsoft `
        --output $summaryPath
    if ($LASTEXITCODE -ne 0) {
        throw "Coverage summary normalization failed"
    }
    Write-Host "==> summary       : $summaryPath"
} elseif ($testResult -eq 0) {
    throw "Coverage collection succeeded but did not create $reportPath"
}

exit $testResult
