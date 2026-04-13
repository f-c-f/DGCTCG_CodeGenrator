# Run ILTranslator.exe (same folder) with a minimal IL model and verify metadata outputs
# (obligation_map.json with merged mcdc/condition/decision, branch_relation.json, distance_map, obligation_map.csv).
# Usage: cd to this script's directory, then: .\run_metadata_json_test.ps1
#
# Note: ArgumentParser only accepts two-token flags like "-l tcg_state_search", not "-l=tcg_state_search".

$ErrorActionPreference = "Stop"

$exeDir = $PSScriptRoot
$exe = Join-Path $exeDir "ILTranslator.exe"
$ilPath = Join-Path $exeDir "test_metadata\DynamicCompressionModel.xml"
$outDir = Join-Path $exeDir "test_metadata\_generated_out"

if (-not (Test-Path -LiteralPath $exe)) {
    Write-Error "ILTranslator.exe not found: $exe (build and place exe under Src\exe\ILTranslator\)"
}

if (-not (Test-Path -LiteralPath $ilPath)) {
    Write-Error "Test IL file not found: $ilPath"
}

$resDir = Join-Path $exeDir "ResourceCode"
if (-not (Test-Path -LiteralPath $resDir)) {
    Write-Error "ResourceCode not found: $resDir (TCG mode copies from exe directory)"
}

if (Test-Path -LiteralPath $outDir) {
    Remove-Item -LiteralPath $outDir -Recurse -Force
}
New-Item -ItemType Directory -Path $outDir | Out-Null

$argList = @(
    "-l", "tcg_hybrid",
    "-i", $ilPath,
    "-o", $outDir,
    "-bdg", "true",
    "-adg", "true",
    "-ic", "true",
    "-ca", "8",
    "-c", "false"
)


Write-Host ("Running: {0} {1}" -f $exe, ($argList -join " "))
$p = Start-Process -FilePath $exe -ArgumentList $argList -WorkingDirectory $exeDir -Wait -PassThru -NoNewWindow
# ??? main ?????? 1?? Windows ?? 0=?? ???
if ($p.ExitCode -lt 0) {
    Write-Error "ILTranslator failed (exit code: $($p.ExitCode))"
}

$obl = Join-Path $outDir "obligation_map.json"
$rel = Join-Path $outDir "branch_relation.json"

if (-not (Test-Path -LiteralPath $obl)) {
    Write-Error "Missing: obligation_map.json"
}
if (-not (Test-Path -LiteralPath $rel)) {
    Write-Error "Missing: branch_relation.json"
}

$oblText = Get-Content -LiteralPath $obl -Raw -Encoding UTF8
$relText = Get-Content -LiteralPath $rel -Raw -Encoding UTF8

if ($oblText -notmatch '"obligations"') {
    Write-Error "obligation_map.json invalid (no obligations key)"
}
if ($oblText -notmatch '"mcdc_decisions"') {
    Write-Error "obligation_map.json invalid (no mcdc_decisions key)"
}
if ($oblText -notmatch '"condition_pairs"') {
    Write-Error "obligation_map.json invalid (no condition_pairs key)"
}
if ($oblText -notmatch '"decision_pairs"') {
    Write-Error "obligation_map.json invalid (no decision_pairs key)"
}

$legacy = @("mcdc_map.json", "condition_map.json", "decision_map.json")
foreach ($f in $legacy) {
    if (Test-Path -LiteralPath (Join-Path $outDir $f)) {
        Write-Error "Unexpected legacy file (should be merged into obligation_map.json): $f"
    }
}

$dist = Join-Path $outDir "distance_map.json"
if (-not (Test-Path -LiteralPath $dist)) {
    Write-Error "Missing: distance_map.json"
}
$csv = Join-Path $outDir "obligation_map.csv"
if (-not (Test-Path -LiteralPath $csv)) {
    Write-Error "Missing: obligation_map.csv"
}
if ($relText -notmatch '"branches"') {
    Write-Error "branch_relation.json invalid (no branches key)"
}

Write-Host "OK: metadata JSON files generated."
Write-Host "  $obl"
Write-Host "  $rel"
Write-Host ""
Write-Host "obligation_map.json preview (first 400 chars):"
Write-Host $oblText.Substring(0, [Math]::Min(400, $oblText.Length))
