# Script PowerShell pour calculer le SHA256 du firmware
Write-Host "Calcul du SHA256 du firmware2.ino.bin..." -ForegroundColor Green
$hash = Get-FileHash -Path "firmware2\build\esp32.esp32.esp32\firmware2.ino.bin" -Algorithm SHA256
Write-Host "SHA256: $($hash.Hash)" -ForegroundColor Yellow
Write-Host "Taille: $((Get-Item 'firmware2\build\esp32.esp32.esp32\firmware2.ino.bin').Length) octets" -ForegroundColor Yellow

# Générer le JSON pour update.json
$json = @{
    version = "2.0"
    url = "https://raw.githubusercontent.com/nadinewellani/esp32-ota-project/main/firmware2.ino.bin"
    sha256 = $hash.Hash.ToLower()
} | ConvertTo-Json

Write-Host "`nContenu pour update.json:" -ForegroundColor Green
Write-Host $json -ForegroundColor Cyan

