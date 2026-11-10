$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
Set-Location $ProjectRoot

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    throw "CMake is not installed. Install CMake and Visual Studio Build Tools with the C++ workload."
}

cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed" }

cmake --build build --config Release --parallel
if ($LASTEXITCODE -ne 0) { throw "Build failed" }

$Exe = Join-Path $ProjectRoot "build\Release\ToolzNG.exe"
if (-not (Test-Path $Exe)) { throw "Build finished but ToolzNG.exe was not found at $Exe" }
Write-Host "Build successful: $Exe" -ForegroundColor Green
