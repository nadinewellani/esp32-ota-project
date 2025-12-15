# Test manuel du webhook pour déclencher la mise à jour
Write-Host "Test manuel du webhook ESP32..." -ForegroundColor Green

$uri = "http://192.168.0.138/webhook"
$body = "payload={`"ref`":`"refs/heads/main`",`"before`":`"abc123`",`"after`":`"def456`"}"

try {
    Write-Host "Envoi du webhook..." -ForegroundColor Yellow
    $response = Invoke-WebRequest -Uri $uri -Method POST -ContentType "application/x-www-form-urlencoded" -Body $body
    Write-Host "`nSUCCES !" -ForegroundColor Green
    Write-Host "Status Code: $($response.StatusCode)" -ForegroundColor Yellow
    Write-Host "Response: $($response.Content)" -ForegroundColor Cyan
    Write-Host "`nLe webhook a ete envoye avec succes !" -ForegroundColor Green
    Write-Host "Verifiez le Serial Monitor de l'ESP32 pour voir les messages." -ForegroundColor Yellow
} catch {
    Write-Host "`nERREUR !" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
    Write-Host "`nVerifiez que:" -ForegroundColor Yellow
    Write-Host "1. L'ESP32 est demarre et connecte au WiFi" -ForegroundColor Yellow
    Write-Host "2. L'IP de l'ESP32 est 192.168.0.138" -ForegroundColor Yellow
    Write-Host "3. Le serveur web est actif (Serial Monitor affiche 'Serveur web demarre')" -ForegroundColor Yellow
}

