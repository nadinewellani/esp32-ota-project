@echo off
echo ========================================
echo Demarrage de ngrok
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

echo Demarrage de ngrok pour exposer le port 80...
echo.
echo IMPORTANT: Gardez cette fenetre ouverte !
echo.
echo Une fois ngrok demarre, vous verrez une URL comme:
echo   Forwarding  https://abc123.ngrok.io -> http://localhost:80
echo.
echo Utilisez cette URL dans GitHub:
echo   Payload URL: https://abc123.ngrok.io/webhook
echo.
echo Appuyez sur Ctrl+C pour arreter ngrok
echo.
echo ========================================
echo.

REM Lancer ngrok
C:\ngrok\ngrok.exe http 80

pause

