# Configuration d'un Tunnel pour le Webhook GitHub

## Problème
GitHub ne peut pas accéder à une adresse IP locale (172.20.10.20). Il faut utiliser un service de tunneling.

## Solution 1: ngrok (Recommandé)

### Installation
1. Téléchargez ngrok depuis https://ngrok.com/download
2. Créez un compte gratuit
3. Installez ngrok

### Utilisation
```bash
# Lancer ngrok pour exposer le port 80
ngrok http 80
```

### Configuration GitHub
1. Copiez l'URL HTTPS fournie par ngrok (ex: `https://abc123.ngrok.io`)
2. Dans GitHub → Settings → Webhooks → Edit
3. Payload URL: `https://abc123.ngrok.io/webhook`
4. Sauvegardez

### Note
- L'URL ngrok change à chaque redémarrage (gratuit)
- Pour une URL fixe, utilisez un compte ngrok payant

## Solution 2: localtunnel (Gratuit, URL changeante)

### Installation
```bash
npm install -g localtunnel
```

### Utilisation
```bash
# Lancer localtunnel
lt --port 80
```

### Configuration GitHub
1. Utilisez l'URL fournie par localtunnel
2. Payload URL: `https://votre-url.loca.lt/webhook`

## Solution 3: Serveur Intermédiaire (Permanent)

Créez un serveur Node.js/Python qui:
1. Reçoit le webhook GitHub (accessible depuis Internet)
2. Transmet la requête à l'ESP32 (sur votre réseau local)

### Exemple Node.js
```javascript
const express = require('express');
const axios = require('axios');
const app = express();

app.use(express.json());

app.post('/github-webhook', (req, res) => {
  const esp32IP = '172.20.10.20'; // IP de votre ESP32
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

Déployez ce serveur sur un service cloud (Heroku, Railway, etc.) et utilisez son URL dans GitHub.

## Solution 4: Port Forwarding (Si IP publique)

Si votre routeur a une IP publique:
1. Configurez le port forwarding (port 80 → IP ESP32)
2. Utilisez votre IP publique dans le webhook GitHub

**Attention**: Exposer l'ESP32 directement sur Internet n'est pas recommandé pour la sécurité.

