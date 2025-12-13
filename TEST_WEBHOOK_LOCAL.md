# Test du Webhook Localement

## Le payload GitHub est correct

Le JSON que vous avez partagé montre que GitHub envoie bien le bon payload avec :
- `"ref": "refs/heads/main"` ✅
- Événement push ✅
- Toutes les données nécessaires ✅

## Le problème : La livraison échoue

Le statut rouge signifie que GitHub n'a pas reçu de réponse "200 OK" de l'ESP32.

## Tests à faire

### Test 1 : Vérifier que l'ESP32 répond localement

1. **Ouvrez votre navigateur**
2. **Tapez** : `http://192.168.0.138/update`
3. **Vous devriez voir** : `{"status":"ok","message":"Vérification de mise à jour déclenchée"}`

Si ça ne fonctionne pas → L'ESP32 ne répond pas

### Test 2 : Vérifier ngrok

1. **Vérifiez que ngrok est actif** (fenêtre ouverte)
2. **Quelle URL ngrok voyez-vous actuellement ?**
3. **Testez l'URL ngrok dans votre navigateur** :
   ```
   https://VOTRE-URL-NGROK.ngrok.io/webhook
   ```
4. **Si ngrok affiche une page d'avertissement**, cliquez sur "Visit Site"

### Test 3 : Vérifier l'URL dans GitHub

1. **GitHub → Settings → Webhooks → Votre webhook**
2. **Vérifiez que l'URL est** : `https://VOTRE-URL-NGROK.ngrok.io/webhook`
3. **L'URL doit correspondre exactement à celle affichée dans ngrok**

### Test 4 : Vérifier le Serial Monitor

Dans le Serial Monitor de l'ESP32, cherchez :
- `=== Webhook GitHub reçu ===`
- Si vous ne voyez rien → L'ESP32 ne reçoit pas le webhook

## Solutions

### Si l'ESP32 ne répond pas localement

1. Vérifiez que l'ESP32 est démarré
2. Vérifiez que le Serial Monitor affiche "Serveur web démarré"
3. Redémarrez l'ESP32 si nécessaire

### Si ngrok ne fonctionne pas

1. Arrêtez ngrok (Ctrl+C)
2. Relancez : `C:\ngrok\ngrok.exe http 192.168.0.138:80`
3. Copiez la nouvelle URL
4. Mettez à jour GitHub

### Si l'URL dans GitHub est incorrecte

1. Vérifiez que l'URL se termine par `/webhook`
2. Vérifiez que l'URL correspond à celle de ngrok
3. Mettez à jour si nécessaire

