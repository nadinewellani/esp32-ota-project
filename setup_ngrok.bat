@echo off
echo ========================================
echo Configuration de ngrok
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

echo Etape 1: Configuration de l'authtoken
echo.
echo Pour obtenir votre authtoken:
echo 1. Allez sur https://ngrok.com et creez un compte gratuit
echo 2. Connectez-vous sur https://dashboard.ngrok.com/get-started/your-authtoken
echo 3. Copiez votre authtoken
echo.
set /p AUTHTOKEN="Entrez votre authtoken ngrok: "

if "%AUTHTOKEN%"=="" (
    echo ERREUR: Authtoken vide !
    pause
    exit /b 1
)

echo.
echo Configuration de l'authtoken...
C:\ngrok\ngrok.exe config add-authtoken %AUTHTOKEN%

if %ERRORLEVEL% == 0 (
    echo.
    echo ========================================
    echo Configuration reussie !
    echo ========================================
    echo.
    echo Vous pouvez maintenant lancer ngrok avec:
    echo   C:\ngrok\ngrok.exe http 80
    echo.
) else (
    echo.
    echo ERREUR lors de la configuration
    pause
    exit /b 1
)

pause

