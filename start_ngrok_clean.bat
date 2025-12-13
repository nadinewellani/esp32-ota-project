@echo off
echo ========================================
echo Demarrage de ngrok pour ESP32 (Nettoyage)
echo ========================================
echo.

REM Arrêter toutes les instances de ngrok
echo Arret des instances ngrok existantes...
taskkill /F /IM ngrok.exe >nul 2>&1
timeout /t 2 /nobreak >nul

REM Vérifier si ngrok est installé
if not exist "C:\ngrok\ngrok.exe" (
    echo ERREUR: ngrok n'est pas installe !
    echo.
    echo Veuillez d'abord executer: install_ngrok.bat
    pause
    exit /b 1
)

echo Demarrage de ngrok pour exposer 192.168.0.138:80...
echo.
echo IMPORTANT: Gardez cette fenetre ouverte !
echo.
echo Une fois ngrok demarre, vous verrez une URL comme:
echo   Forwarding  https://abc123.ngrok.io -> http://192.168.0.138:80
echo.
echo Utilisez cette URL dans GitHub:
echo   Payload URL: https://abc123.ngrok.io/webhook
echo.
echo Appuyez sur Ctrl+C pour arreter ngrok
echo.
echo ========================================
echo.

REM Lancer ngrok avec l'IP de l'ESP32
C:\ngrok\ngrok.exe http 192.168.0.138:80

pause

