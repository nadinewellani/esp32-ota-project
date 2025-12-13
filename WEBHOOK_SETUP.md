# Configuration du Webhook GitHub pour Mise à Jour Automatique

Ce guide explique comment configurer GitHub pour envoyer automatiquement des notifications à l'ESP32 lorsqu'une nouvelle version du firmware est disponible.

## Méthode 1: Webhook Direct (avec tunneling)

### Étape 1: Installer un service de tunneling

**Option A: ngrok (recommandé)**
```bash
# Télécharger ngrok depuis https://ngrok.com
# Installer et lancer
ngrok http 80
```

**Option B: localtunnel**
```bash
npm install -g localtunnel
lt --port 80
```

### Étape 2: Configurer le webhook GitHub

1. Allez dans votre repository GitHub
2. **Settings** → **Webhooks** → **Add webhook**
3. **Payload URL**: `https://votre-url-ngrok.ngrok.io/webhook`
4. **Content type**: `application/json`
5. **Events**: Sélectionnez **"Just the push event"**
6. Cliquez sur **Add webhook**

### Étape 3: Tester

1. Faites un commit et push sur la branche `main`
2. GitHub enverra un webhook à votre ESP32
3. L'ESP32 vérifiera automatiquement les mises à jour

## Méthode 2: Serveur Intermédiaire (recommandé pour production)

Créez un serveur Node.js/Python qui:
1. Reçoit le webhook GitHub
2. Envoie une requête HTTP à l'ESP32

**Exemple Node.js:**
```javascript
const express = require('express');
const axios = require('axios');
const app = express();

app.use(express.json());

app.post('/github-webhook', (req, res) => {
  const esp32IP = '192.168.1.100'; // IP de votre ESP32
  axios.post(`http://${esp32IP}/webhook`, req.body)
    .then(() => {
      console.log('Webhook transféré à ESP32');
      res.sendStatus(200);
    })
    .catch(err => {
      console.error('Erreur:', err);
      res.sendStatus(500);
    });
});

app.listen(3000, () => {
  console.log('Serveur webhook sur le port 3000');
});
```

## Méthode 3: Port Forwarding (si IP publique)

Si votre ESP32 a une IP publique:

1. Configurez le port forwarding sur votre routeur (port 80)
2. Utilisez votre IP publique dans le webhook GitHub
3. **Payload URL**: `http://votre-ip-publique/webhook`

## Endpoints Disponibles

### GET /update
Force une vérification immédiate de mise à jour
```bash
curl http://<IP_ESP32>/update
```

### POST /webhook
Reçoit les webhooks GitHub
```bash
curl -X POST http://<IP_ESP32>/webhook \
  -H "Content-Type: application/json" \
  -d '{"ref":"refs/heads/main"}'
```

### GET /status
Affiche le statut du système (JSON)
```bash
curl http://<IP_ESP32>/status
```

### GET /
Page d'accueil avec informations et liens

## Polling Automatique

Même sans webhook, l'ESP32 vérifie automatiquement les mises à jour:
- **Toutes les 10 secondes** après une vérification forcée
- **Toutes les 30 secondes** en mode normal

## Sécurité (Optionnel)

Pour sécuriser le webhook, vous pouvez ajouter une authentification:

1. Ajoutez un token secret dans le code:
```cpp
const char* WEBHOOK_SECRET = "votre-secret-token";
```

2. Vérifiez le token dans le handler:
```cpp
String token = server.arg("token");
if (token != WEBHOOK_SECRET) {
  server.send(401, "text/plain", "Unauthorized");
  return;
}
```

3. Utilisez le token dans l'URL du webhook GitHub:
```
http://<IP_ESP32>/webhook?token=votre-secret-token
```

## Dépannage

### Le webhook ne fonctionne pas
- Vérifiez que l'ESP32 est accessible depuis Internet
- Vérifiez les logs GitHub (Settings → Webhooks → Recent Deliveries)
- Vérifiez les logs série de l'ESP32

### L'ESP32 ne reçoit pas les webhooks
- Vérifiez que le serveur web est démarré (message dans Serial Monitor)
- Vérifiez l'IP de l'ESP32
- Testez avec curl: `curl http://<IP_ESP32>/update`

### Les mises à jour ne se déclenchent pas
- Vérifiez que `update.json` est à jour sur GitHub
- Vérifiez que la version dans `update.json` est différente de `currentVersion`
- Vérifiez les logs série pour les erreurs

