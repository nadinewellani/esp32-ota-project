# Dépannage - Webhook Statut Rouge

## Étape 1 : Voir l'erreur exacte

1. Dans GitHub → Settings → Webhooks
2. Cliquez sur votre webhook
3. Cliquez sur l'onglet "Recent Deliveries"
4. Cliquez sur la delivery la plus récente (celle en haut avec le statut rouge)
5. Regardez la section "Response" ou "Request"
6. Notez le message d'erreur exact

## Erreurs courantes et solutions

### Erreur 1 : "failed to connect to host"
**Cause** : ngrok n'est pas actif ou URL incorrecte
**Solution** :
- Vérifiez que la fenêtre ngrok est ouverte
- Vérifiez l'URL dans ngrok (elle peut avoir changé)
- Mettez à jour l'URL dans GitHub

### Erreur 2 : "timeout"
**Cause** : L'ESP32 ne répond pas
**Solution** :
- Vérifiez que l'ESP32 est démarré
- Vérifiez le Serial Monitor
- Testez localement : http://192.168.0.138/update

### Erreur 3 : "404 Not Found"
**Cause** : L'URL ne se termine pas par `/webhook`
**Solution** :
- Vérifiez que l'URL dans GitHub se termine par `/webhook`
- Format correct : `https://URL.ngrok.io/webhook`

### Erreur 4 : "502 Bad Gateway" ou "ERR_NGROK_8012"
**Cause** : ngrok ne peut pas se connecter à l'ESP32
**Solution** :
- Vérifiez que l'ESP32 est sur l'IP 192.168.0.138
- Redémarrez ngrok avec la bonne IP
- Testez localement d'abord

## Checklist de vérification

- [ ] ngrok est actif (fenêtre ouverte)
- [ ] URL ngrok dans GitHub correspond à l'URL actuelle de ngrok
- [ ] URL se termine par `/webhook`
- [ ] ESP32 est démarré et connecté au WiFi
- [ ] Serial Monitor affiche "Serveur web démarré"
- [ ] Test local fonctionne : http://192.168.0.138/update

