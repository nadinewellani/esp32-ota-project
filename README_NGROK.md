# Guide d'installation de ngrok pour Windows

## Installation rapide

### Étape 1 : Installer ngrok
Double-cliquez sur `install_ngrok.bat`

Ce script va :
- Télécharger ngrok automatiquement
- L'installer dans `C:\ngrok`
- Vous donner les instructions pour la suite

### Étape 2 : Configurer ngrok (première fois uniquement)
1. Créez un compte gratuit sur https://ngrok.com
2. Connectez-vous et allez sur https://dashboard.ngrok.com/get-started/your-authtoken
3. Copiez votre authtoken
4. Double-cliquez sur `setup_ngrok.bat`
5. Collez votre authtoken quand demandé

### Étape 3 : Lancer ngrok
Double-cliquez sur `start_ngrok.bat`

Vous verrez une URL comme :
```
Forwarding  https://abc123-def456.ngrok.io -> http://localhost:80
```

### Étape 4 : Configurer GitHub
1. Dans GitHub → Settings → Webhooks → Edit
2. Payload URL : `https://abc123-def456.ngrok.io/webhook`
   (Utilisez l'URL HTTPS affichée par ngrok)
3. Sauvegardez

## Commandes manuelles (alternative)

Si vous préférez installer manuellement :

### Télécharger ngrok
```cmd
cd %TEMP%
powershell -Command "Invoke-WebRequest -Uri 'https://bin.equinox.io/c/bNyj1mQVY4c/ngrok-v3-stable-windows-amd64.zip' -OutFile 'ngrok.zip'"
```

### Extraire
```cmd
powershell -Command "Expand-Archive -Path 'ngrok.zip' -DestinationPath 'C:\ngrok' -Force"
```

### Configurer l'authtoken
```cmd
C:\ngrok\ngrok.exe config add-authtoken VOTRE_AUTHTOKEN
```

### Lancer ngrok
```cmd
C:\ngrok\ngrok.exe http 80
```

## Notes importantes

- **URL changeante** : Avec un compte gratuit, l'URL ngrok change à chaque redémarrage
- **Garder la fenêtre ouverte** : ngrok doit rester actif pour que le webhook fonctionne
- **URL HTTPS** : Utilisez toujours l'URL HTTPS (pas HTTP) dans GitHub
- **Port 80** : Assurez-vous que l'ESP32 écoute bien sur le port 80

## Dépannage

### "ngrok n'est pas reconnu"
Ajoutez `C:\ngrok` à votre PATH système :
1. Panneau de configuration → Système → Paramètres système avancés
2. Variables d'environnement → Path → Modifier
3. Ajoutez : `C:\ngrok`

### "Failed to connect"
- Vérifiez que l'ESP32 est bien démarré
- Vérifiez que le serveur web écoute sur le port 80
- Vérifiez l'IP de l'ESP32 dans le Serial Monitor

### "Tunnel closed"
- Redémarrez ngrok
- Vérifiez votre connexion Internet
- Vérifiez que votre authtoken est valide

