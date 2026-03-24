param(
    [ValidateSet("status", "branch", "commit-push")]
    [string]$Action = "status",

    [string]$Branch = "mylearn",

    [string[]]$Submodules = @(),

    [string]$Message = "update submodules",

    [switch]$IncludeRootChanges,

    [switch]$NoPush
)

$ErrorActionPreference = "Stop"
$script:ScriptRoot = if ($PSScriptRoot) { $PSScriptRoot } else { Split-Path -Parent $PSCommandPath }

function Run-Git {
    param(
        [string]$Repo,
        [string[]]$GitArgs
    )

    Push-Location $Repo
    try {
        & git @GitArgs
        if ($LASTEXITCODE -ne 0) {
            throw "git $($GitArgs -join ' ') failed in $Repo"
        }
    }
    finally {
        Pop-Location
    }
}

function Get-GitOutput {
    param(
        [string]$Repo,
        [string[]]$GitArgs,
        [switch]$AllowFailure
    )

    Push-Location $Repo
    try {
        $output = & git @GitArgs 2>&1
        $code = $LASTEXITCODE
    }
    finally {
        Pop-Location
    }

    if ($code -ne 0 -and -not $AllowFailure) {
        throw "git $($GitArgs -join ' ') failed in $Repo`n$output"
    }

    return ,$output
}

function Get-RepoRoot {
    $rootCandidate = Split-Path -Parent $script:ScriptRoot
    $root = Get-GitOutput -Repo $rootCandidate -GitArgs @("rev-parse", "--show-toplevel")
    return ($root | Select-Object -First 1).Trim()
}

function Get-AllSubmodules {
    param([string]$RepoRoot)

    $lines = Get-GitOutput -Repo $RepoRoot -GitArgs @("config", "--file", ".gitmodules", "--get-regexp", "path") -AllowFailure

    $paths = @()
    foreach ($line in $lines) {
        if (-not $line) {
            continue
        }

        $parts = $line -split '\s+', 2
        if ($parts.Length -eq 2) {
            $paths += $parts[1].Trim()
        }
    }

    return $paths
}

function Test-RepoDirty {
    param([string]$Repo)

    $status = Get-GitOutput -Repo $Repo -GitArgs @("status", "--porcelain")
    return [bool]($status | Where-Object { $_.Trim() -ne "" })
}

function Get-SelectedSubmodules {
    param(
        [string]$RepoRoot,
        [string[]]$Requested
    )

    if ($Requested.Count -gt 0) {
        return $Requested
    }

    $selected = @()
    foreach ($path in (Get-AllSubmodules -RepoRoot $RepoRoot)) {
        $full = Join-Path $RepoRoot $path
        if (Test-Path $full -and (Test-RepoDirty -Repo $full)) {
            $selected += $path
        }
    }

    return $selected
}

function Ensure-Branch {
    param(
        [string]$Repo,
        [string]$BranchName
    )

    $current = (Get-GitOutput -Repo $Repo -GitArgs @("rev-parse", "--abbrev-ref", "HEAD") | Select-Object -First 1).Trim()
    if ($current -eq $BranchName) {
        return
    }

    $localExists = $true
    try {
        Get-GitOutput -Repo $Repo -GitArgs @("rev-parse", "--verify", $BranchName) | Out-Null
    }
    catch {
        $localExists = $false
    }

    if ($localExists) {
        Run-Git -Repo $Repo -GitArgs @("checkout", $BranchName)
        return
    }

    $remoteExists = $true
    try {
        Get-GitOutput -Repo $Repo -GitArgs @("ls-remote", "--exit-code", "--heads", "origin", $BranchName) | Out-Null
    }
    catch {
        $remoteExists = $false
    }

    if ($remoteExists) {
        Run-Git -Repo $Repo -GitArgs @("checkout", "-b", $BranchName, "--track", "origin/$BranchName")
    }
    else {
        Run-Git -Repo $Repo -GitArgs @("checkout", "-b", $BranchName)
    }
}

function Commit-IfNeeded {
    param(
        [string]$Repo,
        [string]$CommitMessage
    )

    Run-Git -Repo $Repo -GitArgs @("add", "-A")
    $staged = Get-GitOutput -Repo $Repo -GitArgs @("diff", "--cached", "--name-only")

    if (-not ($staged | Where-Object { $_.Trim() -ne "" })) {
        return $false
    }

    Run-Git -Repo $Repo -GitArgs @("commit", "-m", $CommitMessage)
    return $true
}

$repoRoot = Get-RepoRoot
$selected = Get-SelectedSubmodules -RepoRoot $repoRoot -Requested $Submodules

if ($Action -eq "status") {
    Write-Host "Root repo: $repoRoot"
    Write-Host "Target branch: $Branch"
    Write-Host ""

    foreach ($path in (Get-AllSubmodules -RepoRoot $repoRoot)) {
        $full = Join-Path $repoRoot $path
        if (-not (Test-Path $full)) {
            continue
        }

        $branchName = (Get-GitOutput -Repo $full -GitArgs @("rev-parse", "--abbrev-ref", "HEAD") | Select-Object -First 1).Trim()
        $dirty = Test-RepoDirty -Repo $full
        $mark = if ($dirty) { "DIRTY" } else { "clean" }
        Write-Host ("{0,-35}  {1,-15}  {2}" -f $path, $branchName, $mark)
    }

    exit 0
}

if ($selected.Count -eq 0) {
    throw "No submodules selected, and no dirty submodules were auto-detected."
}

if ($Action -eq "branch") {
    foreach ($path in $selected) {
        $full = Join-Path $repoRoot $path
        Write-Host "Switching $path to branch $Branch"
        Ensure-Branch -Repo $full -BranchName $Branch
    }

    exit 0
}

if ($Action -eq "commit-push") {
    foreach ($path in $selected) {
        $full = Join-Path $repoRoot $path

        Write-Host "Preparing $path on branch $Branch"
        Ensure-Branch -Repo $full -BranchName $Branch

        $committed = Commit-IfNeeded -Repo $full -CommitMessage $Message

        if ($committed -and -not $NoPush) {
            Run-Git -Repo $full -GitArgs @("push", "-u", "origin", $Branch)
        }
    }

    if ($IncludeRootChanges) {
        Run-Git -Repo $repoRoot -GitArgs @("add", "-A")
    }
    else {
        $addArgs = @("add") + $selected
        Run-Git -Repo $repoRoot -GitArgs $addArgs
    }

    $rootCommitted = Commit-IfNeeded -Repo $repoRoot -CommitMessage $Message

    if ($rootCommitted -and -not $NoPush) {
        Run-Git -Repo $repoRoot -GitArgs @("push", "-u", "origin", $Branch)
    }

    exit 0
}
