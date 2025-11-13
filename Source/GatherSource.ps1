$SourcePath = "C:\UEProjects\HW1again\Source"
$OutputFile = "source_files.txt"

if (Test-Path $OutputFile) { Remove-Item $OutputFile }

Get-ChildItem $SourcePath -Recurse -File | ForEach-Object {
    if ($_.Extension -match '\.(h|cpp|cs|txt|ini)$') {
        Add-Content $OutputFile "=== $($_.FullName) ==="
        try { Get-Content $_.FullName -Raw | Add-Content $OutputFile }
        catch { Add-Content $OutputFile "ERROR READING FILE" }
        Add-Content $OutputFile "`n"
    }
}

Write-Host "Done! Check $OutputFile"