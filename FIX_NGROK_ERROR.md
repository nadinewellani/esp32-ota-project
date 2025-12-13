# Résolution de l'erreur ngrok ERR_NGROK_8012

## Problème
ngrok ne peut pas se connecter à `localhost:80` car aucun service n'écoute sur ce port.

## Solutions

### Solution 1 : Vérifier que l'ESP32 est démarré
1. **Connectez l'ESP32 à votre ordinateur via USB**
2. **Ouvrez l'IDE Arduino**
3. **Ouvrez le Serial Monitor** (115200 baud)
4. **Vérifiez les messages** :
   - Vous devriez voir : "Serveur web démarré sur le port 80"
   - Vous devriez voir l'IP de l'ESP32

### Solution 2 : Tester la connexion locale
Testez si l'ESP32 répond localement :

**Option A : Dans le navigateur**
```
http://172.20.10.20/update
```
(Remplacez par l'IP de votre ESP32)

**Option B : Avec curl (PowerShell)**
```powershell
curl http://172.20.10.20/update
```

Si ça fonctionne localement mais pas via ngrok, le problème vient de ngrok.

### Solution 3 : Vérifier que ngrok pointe vers la bonne IP
ngrok doit pointer vers l'IP de l'ESP32, pas vers localhost.

**Si l'ESP32 est sur votre réseau local :**
```bash
ngrok http 172.20.10.20:80
```
(Remplacez par l'IP de votre ESP32)

**Si l'ESP32 est connecté directement à votre PC :**
- L'ESP32 doit être accessible sur localhost:80
- Vérifiez que le serveur web de l'ESP32 écoute bien sur le port 80

### Solution 4 : Redémarrer ngrok avec la bonne configuration
1. Arrêtez ngrok (Ctrl+C dans la fenêtre ngrok)
2. Relancez avec l'IP de l'ESP32 :
   ```bash
   C:\ngrok\ngrok.exe http 172.20.10.20:80
   ```
3. Ou si l'ESP32 est sur localhost :
   ```bash
   C:\ngrok\ngrok.exe http localhost:80
   ```

## Ordre de vérification

1. ✅ ESP32 connecté et alimenté
2. ✅ Serial Monitor ouvert et affiche "Serveur web démarré"
3. ✅ Test local réussi (http://IP_ESP32/update fonctionne)
4. ✅ ngrok redémarré avec la bonne configuration
5. ✅ URL ngrok mise à jour dans GitHub

