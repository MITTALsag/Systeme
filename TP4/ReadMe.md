# 🐚 Mini-Shell - Rapport de Projet

## 👥 Membres du binôme  
MITTAL Sagar


## 🔹 Instructions d'utilisation : 
Faire `make` dans src puis `./shell` et enfin vous pouvez éxécuter des commande bash.


## 🚀 Fonctionnalités mises en œuvre


### 🔹 1. Prompt et boucle principale  
✅ Ajout d'un prompt : `"shell>"` avant `fgets()`.  
✅ Modification du programme pour exécuter les commandes en boucle : ajout de `while(1)` dans le main.  
✅ Gestion de `exit` et `Ctrl+D` pour quitter le shell proprement.  


### 🔹 2. Exécution des commandes simple sans redirection
✅ Création de `shell-functionalities.c` et .h pour mettre le code des futurs questions.  
✅ Création de `Fork()` (c'est la même fonction que le cours).  
✅ Implémentation de `exec_cmd(char* cmd, char** options);`  
   - Crée un processus enfant (`Fork()`).  
   - Gère les erreurs et retourne le code de sortie de la commande exécutée.  
   - Le processus parent attend la fin du processus enfant.  


### 🔹 3. Gestion des redirections (`>` et `<`)
✅ Création de fonctions `gestion_redirection_entree(char** options)` et `gestion_redirection_sorie(char** options)`.
   - Cherche si il y a des `">"` ou `"<"` et change les descripteurs de fichier en conséquence.  
      (on suppose que c'est le processus fils qui appelle ces fonctions).

✅ Création de `exec_cmd_dans_fils(char* cmd, char** options)`.
   - Éxécute simplement la commande `cmd` avec les options `options`.
   - Après avoir fais la gestion de redirections (en utilisant `gestion_redirection_entree` et `gestion_redirection_sorie`).  
      (on suppose que c'est le processus fils qui appelle cette fonction).

✅ Création de `gestion_code_retour_fils(pid_t fils, char* cmd)`/
   - Fais le `wait` et la gestion de code de retours du fils.
      (on suppose que c'est le père (shell) qui appelle cette fonction).

✅ En plus des options vu en cours on rajoute `O_TRUNC` dans la redirection `">"` pour écraser le fichier si il n'étais pas vide.

✅ Modification de la commande `exec_cmd`.
   - Elle fais désormais appelle à `exec_cmd_dans_fils` dans la partie fils et à `gestion_code_retour_fils` dans la partie père.

### 🔹 4. Gestion des tubes (`|`)
✅ Création de `exec_cmd_simple(char* cmd, char** options)`.
   - Elle va exécuter (en utilisant `exec_cmd_dans_fils` et `gestion_code_retour_fils`) la commande `cmd`.
   - Elle fais un `Fork()`.

✅ Modification de la commande `exec_cmd`.
   - On initialise un tableau de tableau de chaîne de caractères (`char***`) pour stocker les commandes séparé par 
      des pipes ("commande atomiques").  
      (on utilise pour cela la fonction `trouve_tube(char **tokens, const char* tube)` fournit).
   - Si il n'y a pas de pipe alors appelle `exec_cmd_simple`. Sinon appelle `exec_cmd_pipe` (voir plus loins).
   - `exec_cmd` ne fais plus appelle à `Fork`.
 
✅ Création de `exec_cmd_pipe(char*** atomic_cmd, int nb_atomic_cmd)`.
   - Cette fonction est appelé par `exec_cmd`.
   - Elle cree le bon nombre de pipes (fonction `pipe`) et le bon nombre de fils (fonction `Fork`).
   - Dans la partie des fils : elle fais la gestion des descripteurs de fichier inutiles et redirige les entrée/sortie 
      des commandes dans les pipe correspondant.  
      Et enfin éxécute les commandes avec `exec_cmd_dans_fils`.
   - Dans la partie du père : elle ferme les descrpteurs de fichiers (à ne pas oublier), 
      puis elle attend tous les fils via `gestion_code_retour_fils`.
   
### 🔹 5. Gestion des processus en arrière-plan  
✅ Création de `test_arriere_plan(char** options, int taille_options)`.
   - renvoie vrais si le dernier élément de `options` est `&` et faux sinon.

✅ Modification de `exec_cmd`.
   - initialisation d'une variable `background` qui est vrais si la commande doit être éxécuter en arrière plan.\
      (si `test_arriere_plan` a retourné vrais sur la commande taper par l'utilisateur).

✅ Modification de `exec_cmd_pipe` et de `exec_cmd_simple`.
   - On passe un argument `bool backgroud` en plus, qui est vrais si il faut éxécuter la commande en arrière plan.
   - Dans la partie du père, on attend pas le fils si `backgroud = true`.
      (On ne fais plus appelle à `gestion_code_retour_fils` dans ce cas).

### 🔹 6. Gestion des processus zombie  
✅ Création de `signal-handlers.c` pour la gestions des signals.

✅ Création de `sigchild_handler(int sig)` (dans signal-handlers.c).
   - Cette fonction fais un waitpid avec :
      - -1 comme 1er argument pour attendre n'importe quel processus.
      - WNOHANG comme 3ème argument pour que l'instruction waitpid ne soit pas bloquante.  
      (elle fais un while car les signaux du même type ne sont pas enregistré s'il arrivent en même temps).

✅ On fais appelle a `signal(SIGCHLD, sigchild_handler)` dans le mais pour re-maper la gestion du signal `SIGCHLD`.

✅ Création d'une variable globale `sigchild_handler_use`.
   - Cette variable globale servira dans `exec_cmd_pipe` et `exec_cmd_simple` pour savoir si on utilise `sigchild_handler`
      à la récéption de SIGCHLD ou non.
      (si on fais une commande en avant plan il ne faut pas utiliser ce handler à la récéption de SIGCHLD).
   - Par défaut cette variable est à vrais.

✅ Modification de `exec_cmd_pipe` et de `exec_cmd_simple`.
   - Dans la partie du père, quand `background` est à faux, on passe `sigchild_handler_use` à faux puis on appelle `gestion_code_retour_fils` et on remet `sigchild_handler_use` à vrais. Cela permet d'ignorer le handler pour une commande en avant plan.
   - Pour le cas où un processus en arrière plan s'est terminer avant la fin du processu en avant plan ce qui pourrais entrainer un zombie.  
      On ajoute un appelle (manuel) à `sigchild_handler` à la fin pour faire le "ramasse miettes".
---

## 🐞 Bugs connus  

### ❌ 1. Message d'erreur incorrect pour une commande inexistante  
🔻 **Problème :**  
```shell
shell> j
j: No such file or directory
```
🔹 **Attendu :**  
```shell
j: Command not found: j
```

### ❌ 2. Affichage du prompt avec les commandes en arrière-plan  
🔻 **Problème :**  
Lorsqu'une commande (qui à un affichage à faire) est exécutée en arrière-plan (`echo "quelque chose" &`), le prompt s'affiche avant la sortie de la commande, créant un blanc après son exécution. Il faut taper sur Entrée et on reviens à la normal.  
🔻 **Problème :**  
```shell
shell> echo Salut &
shell> Salut

```
🔹 **Attendu :**  
```shell
shell> echo Salut & 
Salut
shell> 
```

---

