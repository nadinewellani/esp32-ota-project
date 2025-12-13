# Script de test pour le webhook ESP32
Write-Host "Test du webhook ESP32..." -ForegroundColor Green

$uri = "http://192.168.0.138/webhook"
$body = "payload={`"ref`":`"refs/heads/main`"}"

try {
    $response = Invoke-WebRequest -Uri $uri -Method POST -ContentType "application/x-www-form-urlencoded" -Body $body
    Write-Host "`nSUCCES !" -ForegroundColor Green
    Write-Host "Status Code: $($response.StatusCode)" -ForegroundColor Yellow
    Write-Host "Response: $($response.Content)" -ForegroundColor Cyan
} catch {
    Write-Host "`nERREUR !" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
}

Write-Host "`nVerifiez le Serial Monitor de l'ESP32 pour voir les messages." -ForegroundColor Yellow

