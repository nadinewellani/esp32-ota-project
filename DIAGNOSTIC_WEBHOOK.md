# Diagnostic - ESP32 ne reçoit pas le webhook

## Problème identifié
L'ESP32 ne reçoit pas le webhook GitHub (pas de message dans Serial Monitor).

## Tests à faire dans l'ordre

### Test 1 : L'ESP32 répond-il localement ?

1. **Ouvrez votre navigateur**
2. **Tapez** : `http://192.168.0.138/update`
3. **Résultat attendu** : `{"status":"ok","message":"Vérification de mise à jour déclenchée"}`

**Si ça ne fonctionne pas** :
- L'ESP32 ne répond pas
- Vérifiez que l'ESP32 est démarré
- Vérifiez le Serial Monitor pour voir les messages

**Si ça fonctionne** → Passez au Test 2

### Test 2 : ngrok est-il actif ?

1. **Vérifiez que la fenêtre ngrok est ouverte**
2. **Quelle URL ngrok voyez-vous ?**
3. **Dans la fenêtre ngrok, voyez-vous des requêtes HTTP ?**

**Si ngrok n'est pas actif** :
- Relancez ngrok : `C:\ngrok\ngrok.exe http 192.168.0.138:80`

**Si ngrok est actif** → Passez au Test 3

### Test 3 : Tester l'URL ngrok directement

1. **Ouvrez votre navigateur**
2. **Tapez** : `https://VOTRE-URL-NGROK.ngrok.io/webhook`
   (Remplacez par votre URL ngrok réelle)
3. **Résultat** :
   - Si vous voyez une page ngrok → Cliquez sur "Visit Site"
   - Si vous voyez une erreur → ngrok ne peut pas atteindre l'ESP32
   - Si vous voyez une réponse JSON → ngrok fonctionne

### Test 4 : Vérifier l'URL dans GitHub

1. **GitHub → Settings → Webhooks → Votre webhook**
2. **Vérifiez que l'URL est exactement** : `https://VOTRE-URL-NGROK.ngrok.io/webhook`
3. **L'URL doit correspondre à celle affichée dans ngrok**

## Solutions selon les résultats

### Si Test 1 échoue (ESP32 ne répond pas localement)
- Redémarrez l'ESP32
- Vérifiez que le Serial Monitor affiche "Serveur web démarré"
- Vérifiez l'IP de l'ESP32 (peut avoir changé)

### Si Test 2 échoue (ngrok n'est pas actif)
- Relancez ngrok avec la bonne IP
- Utilisez : `C:\ngrok\ngrok.exe http 192.168.0.138:80`

### Si Test 3 échoue (ngrok ne peut pas atteindre l'ESP32)
- Vérifiez que l'IP de l'ESP32 est toujours 192.168.0.138
- Redémarrez ngrok avec la bonne IP
- Vérifiez que l'ESP32 et votre PC sont sur le même réseau WiFi

### Si Test 4 échoue (URL incorrecte dans GitHub)
- Mettez à jour l'URL dans GitHub
- Assurez-vous qu'elle se termine par `/webhook`
- Assurez-vous qu'elle correspond à l'URL ngrok actuelle

