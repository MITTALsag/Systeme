#!/bin/bash

# Fonction pour exécuter une commande avec débogage sans afficher set -x/set +x
# Fonction pour exécuter une commande avec débogage
debug_cmd() {
    echo "+ $*"
    ./shell 
    echo $@
}

# Test de base : Commandes existantes
echo "=== Test 1: Commande existante ==="
echo "Test de la commande ls"
debug_cmd ls
echo ""

# Test d'une commande inexistante
echo "=== Test 2: Commande inexistante ==="
echo "Test d'une commande qui n'existe pas"
debug_cmd foobar
echo ""

# Test de commande avec argument
echo "=== Test 3: Commande avec argument ==="
echo "Test de la commande echo"
debug_cmd echo "Hello World"
echo ""

# Redirection de sortie avec >
echo "=== Test 4: Redirection de sortie (>) ==="
debug_cmd 'echo "Hello" > output.txt'
echo "Vérification du contenu de output.txt"
debug_cmd cat output.txt
echo ""

# Redirection de sortie avec écrasement >
echo "=== Test 5: Redirection de sortie (écrasement) ==="
debug_cmd 'echo "World" > output.txt'
echo "Vérification du contenu de output.txt (écrasé)"
debug_cmd cat output.txt
echo ""

# Redirection d'entrée avec <
echo "=== Test 6: Redirection d'entrée (<) ==="
debug_cmd 'echo "Hello from input" > input.txt'
debug_cmd cat < input.txt
echo ""


# Commande d'échec (fichier introuvable)
echo "=== Test 7: Commande d'échec (fichier introuvable) ==="
echo "Test avec fichier introuvable"
debug_cmd cat non_existent_file
echo ""

# Permission refusée
echo "=== Test 8: Permission refusée ==="
debug_cmd touch restricted_file
debug_cmd chmod 000 restricted_file
debug_cmd cat restricted_file
echo ""

# Redirection d'entrée et de sortie simultanée
echo "=== Test 9: Redirection d'entrée et de sortie simultanée ==="
debug_cmd 'echo "Input for file" > input.txt'
debug_cmd 'cat < input.txt > output.txt'
echo "Vérification du contenu de output.txt"
debug_cmd cat output.txt
echo ""


# Test de commande longue
echo "=== Test 10: Commande longue ==="
echo "Test long running command"
debug_cmd sleep 5
echo "Fin du test de commande longue"
echo ""

# Test de segmentation (programme inexistant)
echo "=== Test 11: Segmentation - programme inexistant ==="
debug_cmd ./non_existing_program
echo ""

# Test de redirection d'entrée et de sortie avec une commande non trouvée
echo "=== Test 12: Redirection avec une commande non trouvée ==="
debug_cmd non_existing_command < input.txt > output.txt
echo ""

# Nettoyage des fichiers temporaires
debug_cmd rm -f input.txt output.txt restricted_file

# Test de exit
echo "=== Test 16: Commande exit ==="
echo "Test de la commande exit"
debug_cmd exit 0
