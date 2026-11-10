$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
Set-Location $ProjectRoot

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    throw "Git is not installed. Install Git for Windows first."
}
if (-not (Get-Command gh -ErrorAction SilentlyContinue)) {
    throw "GitHub CLI is not installed. Install it from https://cli.github.com/"
}

gh auth status
if ($LASTEXITCODE -ne 0) { throw "Run 'gh auth login' first." }

$Remote = "https://github.com/bboroski/ToolNG.git"
$Existing = git remote get-url origin 2>$null
if ($LASTEXITCODE -eq 0 -and $Existing -ne $Remote) {
    throw "The existing origin points to '$Existing', not '$Remote'. Refusing to overwrite it."
}
if ($LASTEXITCODE -ne 0) {
    git remote add origin $Remote
    if ($LASTEXITCODE -ne 0) { throw "Could not add origin remote." }
}

if (-not (Test-Path (Join-Path $ProjectRoot ".git"))) {
    git init --initial-branch=main
}
git add .
if ($LASTEXITCODE -ne 0) { throw "git add failed." }

git rev-parse --verify HEAD *> $null
if ($LASTEXITCODE -ne 0) {
    $Login = gh api user --jq .login
    git config user.name $Login
    git config user.email "$Login@users.noreply.github.com"
    git commit -m "Publish ToolzNG Duck Edition 0.2.0"
    if ($LASTEXITCODE -ne 0) { throw "Initial commit failed." }
}

git branch -M main
git push -u origin main
if ($LASTEXITCODE -ne 0) { throw "Push failed. Check GitHub write access and branch protection." }
Write-Host "ToolzNG was pushed to https://github.com/bboroski/ToolNG" -ForegroundColor Green
