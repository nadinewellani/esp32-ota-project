# Test du Webhook avec POST

## Diagnostic

✅ `/update` fonctionne (GET) → L'ESP32 répond correctement
❌ `/webhook` retourne "Not found" (GET) → Normal, car le webhook attend POST

## Le problème

GitHub envoie une requête **POST** à `/webhook`, mais vous avez testé avec **GET** dans le navigateur. C'est pourquoi vous voyez "Not found".

## Solution : Tester avec POST

### Option 1 : Utiliser curl (PowerShell)

Ouvrez PowerShell et testez :

```powershell
curl -X POST http://192.168.0.138/webhook -H "Content-Type: application/x-www-form-urlencoded" -d "payload={\"ref\":\"refs/heads/main\"}"
```

**Résultat attendu** :
- Si vous voyez `{"status":"ok","message":"Mise à jour déclenchée"}` → Le webhook fonctionne localement
- Si vous voyez "Not found" → Il y a un problème avec le code

### Option 2 : Vérifier dans le Serial Monitor

Quand vous testez avec curl, regardez le Serial Monitor de l'ESP32. Vous devriez voir :
```
=== Webhook GitHub reçu ===
Format: x-www-form-urlencoded
```

## Vérifications ngrok

1. **ngrok est-il actif ?** (fenêtre ouverte)
2. **Quelle URL ngrok voyez-vous ?**
3. **L'URL dans GitHub est-elle** : `https://VOTRE-URL-NGROK.ngrok.io/webhook` ?

## Test complet

1. Testez avec curl (commande ci-dessus)
2. Vérifiez le Serial Monitor
3. Si ça fonctionne localement mais pas via GitHub → Le problème vient de ngrok ou de l'URL dans GitHub

