# Guide Étape par Étape - Configuration Webhook GitHub

## ✅ Étape 1 : Vérifier que l'ESP32 fonctionne

1. **Connectez l'ESP32** à votre ordinateur via USB
2. **Ouvrez l'IDE Arduino**
3. **Ouvrez le Serial Monitor** (115200 baud)
4. **Vérifiez que vous voyez** :
   ```
   Serveur web démarré sur le port 80
   Adresse IP: 192.168.0.138
   ```
5. ✅ **Si vous voyez ces messages, passez à l'étape 2**

---

## ✅ Étape 2 : Tester l'ESP32 localement

1. **Ouvrez votre navigateur**
2. **Tapez dans la barre d'adresse** :
   ```
   http://192.168.0.138/update
   ```
3. **Vous devriez voir** : `{"status":"ok","message":"Vérification de mise à jour déclenchée"}`
4. ✅ **Si ça fonctionne, l'ESP32 répond correctement. Passez à l'étape 3**

---

## ✅ Étape 3 : Lancer ngrok avec l'IP de l'ESP32

### Option A : Utiliser le script (Recommandé)

1. **Double-cliquez sur** : `start_ngrok_192.168.0.138.bat`
2. **Une fenêtre ngrok s'ouvre**
3. **Notez l'URL HTTPS** affichée, par exemple :
   ```
   Forwarding  https://abc123-def456.ngrok.io -> http://192.168.0.138:80
   ```
4. **Copiez l'URL HTTPS** (ex: `https://abc123-def456.ngrok.io`)
5. ✅ **Gardez cette fenêtre ouverte !**

### Option B : Commande manuelle

1. **Ouvrez PowerShell ou CMD**
2. **Tapez** :
   ```bash
   C:\ngrok\ngrok.exe http 192.168.0.138:80
   ```
3. **Notez l'URL HTTPS** affichée
4. **Copiez l'URL HTTPS**

---

## ✅ Étape 4 : Tester l'URL ngrok

1. **Ouvrez votre navigateur**
2. **Tapez l'URL ngrok** + `/webhook` :
   ```
   https://VOTRE-URL-NGROK.ngrok.io/webhook
   ```
   (Remplacez par votre URL réelle)
3. **Si ngrok affiche une page d'avertissement** :
   - Cliquez sur **"Visit Site"** ou **"Continue"**
   - C'est normal la première fois
4. ✅ **Si vous voyez une réponse JSON, ngrok fonctionne. Passez à l'étape 5**

---

## ✅ Étape 5 : Configurer GitHub

1. **Allez sur GitHub.com**
2. **Ouvrez votre repository** : `nadinewellani/esp32-ota-project`
3. **Cliquez sur** : **Settings** (en haut à droite)
4. **Dans le menu de gauche**, cliquez sur : **Webhooks**
5. **Cliquez sur votre webhook existant** (ou "Add webhook" si vous n'en avez pas)
6. **Dans "Payload URL"**, entrez :
   ```
   https://VOTRE-URL-NGROK.ngrok.io/webhook
   ```
   (Remplacez par votre URL ngrok réelle + `/webhook`)
7. **Content type** : `application/x-www-form-urlencoded` (déjà sélectionné)
8. **Which events** : "Just the push event" (déjà sélectionné)
9. **Active** : ✅ coché
10. **Cliquez sur** : **"Update webhook"** (ou "Add webhook")
11. ✅ **Configuration GitHub terminée !**

---

## ✅ Étape 6 : Tester le webhook

### Test 1 : Vérifier dans GitHub

1. **Dans GitHub**, allez dans : **Settings** → **Webhooks**
2. **Cliquez sur votre webhook**
3. **Cliquez sur l'onglet** : **"Recent Deliveries"**
4. **Vous devriez voir** une delivery récente (si vous avez fait un push)
5. **Cliquez sur la delivery la plus récente**
6. **Vérifiez le statut** :
   - ✅ **200 OK** (vert) = Succès !
   - ❌ **Erreur** = Voir l'étape de dépannage

### Test 2 : Faire un nouveau push

1. **Ouvrez PowerShell** dans le dossier `C:\secure`
2. **Tapez les commandes** :
   ```bash
   git add .
   git commit -m "Test webhook final"
   git push origin main
   ```
3. **Attendez quelques secondes**
4. **Vérifiez dans GitHub** :
   - **Settings** → **Webhooks** → **Recent Deliveries**
   - Vous devriez voir une nouvelle delivery avec **200 OK**
5. **Vérifiez dans le Serial Monitor de l'ESP32** :
   - Vous devriez voir : `=== Webhook GitHub reçu ===`
   - Puis : `>>> Déclenchement de la mise à jour suite au webhook...`

---

## ✅ Étape 7 : Vérifier que tout fonctionne

### Checklist finale :

- [ ] ESP32 démarré et connecté au WiFi
- [ ] Serial Monitor affiche "Serveur web démarré sur le port 80"
- [ ] IP ESP32 : 192.168.0.138
- [ ] ngrok lancé et fenêtre ouverte
- [ ] URL ngrok copiée
- [ ] GitHub configuré avec l'URL ngrok + `/webhook`
- [ ] Test local réussi (http://192.168.0.138/update)
- [ ] Test ngrok réussi (https://URL/webhook)
- [ ] Push GitHub réussi
- [ ] Webhook GitHub montre "200 OK"
- [ ] Serial Monitor ESP32 montre "Webhook GitHub reçu"

---

## 🔧 Dépannage

### Le webhook échoue toujours

1. **Vérifiez que ngrok est toujours actif** (fenêtre ouverte)
2. **Vérifiez l'URL dans GitHub** (doit se terminer par `/webhook`)
3. **Vérifiez que l'ESP32 est démarré** (Serial Monitor actif)
4. **Testez manuellement** : `http://192.168.0.138/update` dans le navigateur

### L'URL ngrok a changé

- C'est normal si vous avez redémarré ngrok
- **Solution** : Copiez la nouvelle URL et mettez à jour GitHub

### L'ESP32 ne reçoit pas le webhook

1. **Vérifiez le Serial Monitor** pour voir les messages
2. **Vérifiez que le serveur web est démarré**
3. **Testez localement** : `http://192.168.0.138/update`

---

## 📝 Notes importantes

- ⚠️ **Gardez la fenêtre ngrok ouverte** pendant toute l'utilisation
- ⚠️ **L'URL ngrok change** à chaque redémarrage (compte gratuit)
- ⚠️ **Si vous redémarrez ngrok**, mettez à jour l'URL dans GitHub
- ✅ **Pour une URL fixe**, utilisez un compte ngrok payant

---

## 🎉 Félicitations !

Si toutes les étapes sont complétées avec succès, votre système de mise à jour automatique est opérationnel ! Chaque fois que vous poussez une nouvelle version sur GitHub, l'ESP32 sera automatiquement mis à jour.

