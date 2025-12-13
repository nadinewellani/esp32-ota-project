# Configuration du Webhook GitHub avec ngrok

## Étape 1 : Trouver l'URL ngrok

### Option A : Dans le terminal où ngrok tourne
Vous devriez voir quelque chose comme :
```
Forwarding  https://abc123-def456.ngrok.io -> http://localhost:80
```

**Copiez l'URL HTTPS** (ex: `https://abc123-def456.ngrok.io`)

### Option B : Dans l'interface web ngrok
1. Ouvrez votre navigateur sur `http://127.0.0.1:4040`
2. Cliquez sur l'onglet "Status" ou regardez la section "Forwarding"
3. Vous verrez l'URL HTTPS publique

## Étape 2 : Configurer GitHub

1. Allez dans votre repository GitHub
2. **Settings** → **Webhooks** → Cliquez sur votre webhook existant (ou "Add webhook")
3. Dans **Payload URL**, entrez :
   ```
   https://VOTRE-URL-NGROK.ngrok.io/webhook
   ```
   (Remplacez par votre URL ngrok réelle)

4. **Content type** : `application/x-www-form-urlencoded`
5. **Which events** : "Just the push event"
6. Cliquez sur **Update webhook** (ou "Add webhook")

## Étape 3 : Tester

1. Faites un petit changement dans votre repository
2. Commitez et poussez sur la branche `main` :
   ```bash
   git add .
   git commit -m "Test webhook"
   git push origin main
   ```

3. Vérifiez dans GitHub :
   - Allez dans **Settings** → **Webhooks** → Cliquez sur votre webhook
   - Regardez "Recent Deliveries"
   - Vous devriez voir un statut "200 OK" (vert)

4. Vérifiez dans le Serial Monitor de l'ESP32 :
   - Vous devriez voir : "Webhook GitHub reçu !"
   - Puis : "Déclenchement de la mise à jour suite au webhook..."

## Dépannage

### Le webhook échoue toujours
- Vérifiez que ngrok est toujours actif (fenêtre ouverte)
- Vérifiez que l'ESP32 est démarré et connecté au WiFi
- Vérifiez l'URL dans GitHub (doit être HTTPS, pas HTTP)
- Vérifiez que l'URL se termine par `/webhook`

### L'ESP32 ne reçoit pas le webhook
- Vérifiez le Serial Monitor pour voir les messages
- Testez manuellement : `http://VOTRE-IP-ESP32/update` dans votre navigateur
- Vérifiez que le serveur web est démarré (message au démarrage)

### L'URL ngrok change à chaque fois
- C'est normal avec un compte gratuit
- Vous devez mettre à jour l'URL dans GitHub à chaque redémarrage de ngrok
- Pour une URL fixe, utilisez un compte ngrok payant

