@echo off
echo ========================================
echo Installation de ngrok pour Windows
echo ========================================
echo.

REM Vérifier si ngrok est déjà installé
where ngrok >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo ngrok est deja installe !
    ngrok version
    echo.
    echo Pour lancer ngrok, utilisez: ngrok http 80
    pause
    exit /b
)

echo Etape 1: Telechargement de ngrok...
echo.

REM Créer un dossier temporaire
if not exist "%TEMP%\ngrok_install" mkdir "%TEMP%\ngrok_install"
cd /d "%TEMP%\ngrok_install"

REM Télécharger ngrok (version Windows 64-bit)
echo Telechargement en cours...
powershell -Command "Invoke-WebRequest -Uri 'https://bin.equinox.io/c/bNyj1mQVY4c/ngrok-v3-stable-windows-amd64.zip' -OutFile 'ngrok.zip'"

if not exist "ngrok.zip" (
    echo ERREUR: Echec du telechargement
    pause
    exit /b 1
)

echo.
echo Etape 2: Extraction de ngrok...
powershell -Command "Expand-Archive -Path 'ngrok.zip' -DestinationPath '.' -Force"

if not exist "ngrok.exe" (
    echo ERREUR: ngrok.exe non trouve apres extraction
    pause
    exit /b 1
)

echo.
echo Etape 3: Installation de ngrok...
echo.

REM Copier ngrok.exe dans un dossier accessible (par exemple C:\ngrok)
if not exist "C:\ngrok" mkdir "C:\ngrok"
copy /Y ngrok.exe "C:\ngrok\ngrok.exe" >nul

REM Ajouter au PATH (optionnel, pour cette session)
set PATH=%PATH%;C:\ngrok

echo.
echo ========================================
echo Installation terminee !
echo ========================================
echo.
echo ngrok a ete installe dans: C:\ngrok
echo.
echo IMPORTANT: Vous devez maintenant:
echo 1. Creer un compte gratuit sur https://ngrok.com
echo 2. Obtenir votre authtoken depuis https://dashboard.ngrok.com/get-started/your-authtoken
echo 3. Executer: ngrok config add-authtoken VOTRE_TOKEN
echo.
echo Pour lancer ngrok et exposer le port 80:
echo   C:\ngrok\ngrok.exe http 80
echo.
echo Ou ajoutez C:\ngrok au PATH systeme pour utiliser simplement: ngrok http 80
echo.

REM Nettoyer
cd /d %TEMP%
rmdir /s /q "%TEMP%\ngrok_install" 2>nul

pause

