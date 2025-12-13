# Dépannage du Webhook GitHub

## Problème détecté
Tous les webhooks montrent des icônes d'avertissement rouges, indiquant des échecs de livraison.

## Étapes de diagnostic

### 1. Vérifier les détails d'une delivery
Dans GitHub :
- Cliquez sur une delivery (par exemple la plus récente)
- Regardez la section "Response" pour voir l'erreur exacte
- Les erreurs communes sont :
  - "failed to connect to host" → ngrok n'est pas actif ou URL incorrecte
  - "timeout" → L'ESP32 ne répond pas
  - "404 Not Found" → L'URL ne se termine pas par `/webhook`

### 2. Vérifier que ngrok est actif
- La fenêtre ngrok doit être ouverte
- Vérifiez l'URL dans le terminal ngrok
- L'URL peut avoir changé si ngrok a été redémarré

### 3. Vérifier l'URL dans GitHub
- L'URL doit être : `https://sarah-uninjurious-will.ngrok-free.dev/webhook`
- Doit se terminer par `/webhook`
- Doit être HTTPS (pas HTTP)

### 4. Vérifier l'ESP32
- L'ESP32 doit être démarré
- Le Serial Monitor doit afficher : "Serveur web démarré sur le port 80"
- L'ESP32 doit être connecté au WiFi

### 5. Tester manuellement
Testez l'URL ngrok dans votre navigateur :
```
https://sarah-uninjurious-will.ngrok-free.dev/webhook
```

Si ngrok affiche une page d'avertissement, cliquez sur "Visit Site" la première fois.

## Solutions courantes

### Solution 1 : Redémarrer ngrok
1. Arrêtez ngrok (Ctrl+C)
2. Relancez : `C:\ngrok\ngrok.exe http 80`
3. Copiez la nouvelle URL
4. Mettez à jour l'URL dans GitHub

### Solution 2 : Vérifier l'ESP32
1. Ouvrez le Serial Monitor
2. Vérifiez que le serveur web est démarré
3. Vérifiez l'IP de l'ESP32
4. Testez manuellement : `http://<IP_ESP32>/update`

### Solution 3 : Vérifier la page ngrok
Si vous voyez une page ngrok au lieu d'une réponse de l'ESP32 :
- C'est normal la première fois
- Cliquez sur "Visit Site"
- GitHub pourra ensuite accéder au webhook

