# Vérification - ESP32 ne détecte pas le déclenchement GitHub

## Diagnostic

✅ **Test manuel fonctionne** → L'ESP32 reçoit bien les webhooks localement
❌ **GitHub ne déclenche pas** → Problème avec ngrok ou configuration GitHub

## Vérifications à faire

### 1. Vérifier le Serial Monitor

Dans le Serial Monitor de l'ESP32, après le push GitHub, voyez-vous :
```
=== Webhook GitHub reçu ===
```

**Si OUI** → L'ESP32 reçoit le webhook, le problème est ailleurs
**Si NON** → GitHub n'arrive pas à envoyer le webhook

### 2. Vérifier ngrok

1. **La fenêtre ngrok est-elle ouverte ?**
2. **Dans la fenêtre ngrok, voyez-vous des requêtes HTTP ?**
   - Quand GitHub envoie le webhook, vous devriez voir une ligne avec POST /webhook
3. **Quelle URL ngrok voyez-vous actuellement ?**

### 3. Vérifier GitHub

1. **GitHub → Settings → Webhooks → Votre webhook**
2. **Cliquez sur "Recent Deliveries"**
3. **Cliquez sur la delivery la plus récente**
4. **Vérifiez le statut** :
   - ✅ **200 OK** (vert) = GitHub a réussi à envoyer
   - ❌ **Failed** (rouge) = GitHub n'a pas réussi à envoyer

### 4. Vérifier l'URL dans GitHub

1. **GitHub → Settings → Webhooks → Votre webhook**
2. **L'URL doit être** : `https://VOTRE-URL-NGROK.ngrok.io/webhook`
3. **Elle doit correspondre exactement à l'URL affichée dans ngrok**

## Solutions

### Si ngrok n'est pas actif
1. Relancez ngrok : `C:\ngrok\ngrok.exe http 192.168.0.138:80`
2. Copiez la nouvelle URL
3. Mettez à jour GitHub

### Si l'URL dans GitHub est incorrecte
1. Vérifiez que l'URL se termine par `/webhook`
2. Vérifiez qu'elle correspond à l'URL ngrok actuelle
3. Mettez à jour si nécessaire

### Si GitHub montre "Failed"
1. Cliquez sur la delivery pour voir l'erreur exacte
2. Vérifiez que ngrok est actif
3. Testez l'URL ngrok dans votre navigateur

