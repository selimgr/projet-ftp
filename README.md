# Projet FTP
Durant ce projet, notre objectif était de créer un serveur FTP (File Transfer Protocol).

Un serveur FTP permet l’échange de fichiers entre un serveur, contenant les fichiers, et un client. Le serveur peut fournir des fichiers au client et le client peut également en fournir au serveur qui les stockera dans un répertoire.

Dans un premier temps, nous avons implémenté le serveur FTP en version concurrente avec un pool de processus. C’est à dire que le serveur crée X processus fils qui vont tous interagir avec un client. Le processus père interagit également avec un client. Donc pour X processus fils, on permet à X+1 clients de se connecter au serveur.

Dans un second temps, on s’est intéressé à une autre architecture d’implémentation avec répartiteur de charge.

Un client se connecte à un serveur maître, qui va « rediriger » le client vers un des serveur esclave disponible dans un ordre type « tourniquet ».

Par exemple, pour 2 serveurs esclave et 3 clients, le client 1 est servi par le serveur esclave 1, le client 2 est servi par le serveur esclave 2 et le client 3 sera servi par le serveur esclave 1, ainsi de suite..

## Lancement des programmes
Les fichiers du serveur FTP sont présents dans le répertoire `_FTPROOT` et les fichiers reçus par le client sont envoyés dans le répertoire `_CLIENTROOT`.

Pour chaque étape, les commandes sont à exécutées dans leur répertoire respectif.
### POUR L’ÉTAPE 1 & 2 (dossier « Étape 1 » et dossier « Étape 2 »):
1. faire make dans le répertoire « Étape 1 »
2. faire `./ftpserver &` pour lancer le serveur en arrière-plan
3. faire `./ftpclient localhost` pour lancer le client

### POUR L’ÉTAPE 3 (dossier « Étape 3 »):
1. faire make dans le répertoire « Étape 1 »
2. faire `./ftpserver &` pour lancer le serveur en arrière-plan
3. faire `./ftpslaves localhost &` pour lancer les serveurs esclaves en arrière-plan
4. faire `./ftpclient localhost` pour lancer le client
Pour le client, les commandes disponibles sont: get suivi d’un nom de fichier et bye pour terminer la connexion.
