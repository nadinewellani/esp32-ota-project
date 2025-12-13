@echo off
echo ========================================
echo Demarrage de ngrok pour ESP32
echo ========================================
echo.

REM Vérifier si ngrok est installé
if not exist "C:\ngrok\ngrok.exe" (
    echo ERREUR: ngrok n'est pas installe !
    echo.
    echo Veuillez d'abord executer: install_ngrok.bat
    pause
    exit /b 1
)

echo IMPORTANT: Vous devez d'abord connaitre l'IP de votre ESP32
echo.
echo Dans le Serial Monitor de l'ESP32, cherchez la ligne:
echo   "Adresse IP: XXX.XXX.XXX.XXX"
echo.
set /p ESP32_IP="Entrez l'IP de l'ESP32 (ex: 172.20.10.20): "

if "%ESP32_IP%"=="" (
    echo ERREUR: IP vide !
    pause
    exit /b 1
)

echo.
echo Demarrage de ngrok pour exposer %ESP32_IP%:80...
echo.
echo IMPORTANT: Gardez cette fenetre ouverte !
echo.
echo Une fois ngrok demarre, vous verrez une URL comme:
echo   Forwarding  https://abc123.ngrok.io -> http://%ESP32_IP%:80
echo.
echo Utilisez cette URL dans GitHub:
echo   Payload URL: https://abc123.ngrok.io/webhook
echo.
echo Appuyez sur Ctrl+C pour arreter ngrok
echo.
echo ========================================
echo.

REM Lancer ngrok avec l'IP de l'ESP32
C:\ngrok\ngrok.exe http %ESP32_IP%:80

pause

