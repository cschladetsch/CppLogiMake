<#
.SYNOPSIS
    Points people at the GitHub Pages docs site: sets the repo's "Website"
    field and adds a docs badge/link near the top of Readme.md.

.DESCRIPTION
    Idempotent - safe to run more than once. Requires the GitHub CLI (gh),
    authenticated, and run from inside the repo.
#>

param(
    [string]$PagesUrl = "https://cschladetsch.github.io/CppLogiMake/"
)

$ErrorActionPreference = "Stop"

# --- 1. Set the repo's "Website" field (shows under the About box) ---
Write-Host "Setting repo homepage to $PagesUrl ..."
gh repo edit --homepage $PagesUrl
if ($LASTEXITCODE -ne 0) {
    throw "gh repo edit failed - are you authenticated and inside the repo?"
}

# --- 2. Add a docs badge to Readme.md, right after the H1, if not already there ---
$readmePath = Join-Path (git rev-parse --show-toplevel) "Readme.md"
if (-not (Test-Path $readmePath)) {
    throw "Readme.md not found at $readmePath"
}

$badgeLine = "[![Docs](https://img.shields.io/badge/docs-online-blue)]($PagesUrl)"
$lines = Get-Content $readmePath

if ($lines -match [regex]::Escape($badgeLine)) {
    Write-Host "Badge already present in Readme.md - skipping."
} else {
    $h1Index = ($lines | Select-String -Pattern '^# ' | Select-Object -First 1).LineNumber
    if (-not $h1Index) {
        throw "Couldn't find an H1 (line starting with '# ') in Readme.md"
    }

    $newLines = @()
    $newLines += $lines[0..($h1Index - 1)]   # everything up to and including the H1
    $newLines += ""
    $newLines += $badgeLine
    $newLines += $lines[$h1Index..($lines.Count - 1)]  # rest of the file

    Set-Content -Path $readmePath -Value $newLines
    Write-Host "Added docs badge to Readme.md."
}

Write-Host "Done. Review the Readme.md diff, then commit and push."
