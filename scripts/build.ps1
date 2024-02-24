Set-StrictMode -Version Latest

$scriptDir = [System.IO.Path]::GetDirectoryName($MyInvocation.MyCommand.Definition)
$cMLFileName = "CMakeLists.txt"
$buildDirName = "build"

# --- Function declaration ---
function Write-Error {
    param (
        [string]$message
    )
    Write-Host -ForegroundColor Red $message
}

function Test-CMakeExists {
    try {
        Get-Command cmake -ErrorAction Stop | Out-Null
        Write-Host "OK"
        return $true
    }
    catch {
        return $false
    }
}

function Test-ProjectRoot {
    param (
        [string]$projRoot
    )
    $Private:cMLPath = Join-Path -Path $projRoot -ChildPath $cMLFileName
    if (!(Test-Path -Path $Private:cMLPath)) {
        Write-Host "ERROR"
        Write-Error "$($cMLFileName) was not found in the root!"
        return $false
    }
    Write-Host "OK"
    return $true
}

function Get-ProjectRoot {
    param (
        [string]$scriptDir
    )
    $dir = Split-Path $scriptDir -Leaf
    Write-Debug "Checking if this script is located under 'scripts' directory."
    if ($dir.ToLower() -eq "scripts") {
        Write-Debug "TRUE. Assuming the project dir is parent dir."
        return Split-Path -Parent $PSScriptRoot
    } else {
        Write-Debug "FALSE. Assuming the project dir is script's dir."
        return $PSScriptRoot
    }
}

# --- Main ---

# Get project root
$projRoot = Get-ProjectRoot $scriptDir
Write-Host "Building a project under [$($projRoot)]"

# Validation
Write-Host -NoNewline "Checking if CMake is installed..."
if (!(Test-CMakeExists)) {
    Exit 1
}
Write-Host -NoNewline "Verifying that $($cMLFileName) exists in the project root..."
if (!(Test-ProjectRoot $projRoot)) {
    Exit 1
}

# Get or create a directory for CMake caches
$buildDir = Join-Path -Path $projRoot -ChildPath $buildDirName
if (!(Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

Write-Host "Running CMake...`n"

cmake -S $projRoot -B $buildDir
cmake --build $buildDir

Write-Host "`nFinished"
