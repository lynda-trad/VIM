Projet 2019 - Clone de vim

Sources : clone.c editor.c terminal.c mode.c clone.h editor.h terminal.h mode.h
Executable : ./clone

Membres du groupe :

Lynda Trad                                  - u21604852

Nadeesha Hatharasingha Koditthuwakku Ar     - u21610524

Participation : 

Lynda : 
- passage du mode canonique au mode non canonique
- passage du mode Normal au mode Insertion
- parsing de la commande en mode Normal
- commandes :q :w et :w fichier en mode Normal
- afficher le character pressé
- afficher le fichier en argument
- detecter backspace et suppr

Nadeesha :
- détecter quelle flèche a été pressée
- fonctions pour bouger le curseur dans un fichier donné en argument en mode Insertion
- afficher le curseur
- comportement du buffer et du curseur si on presse backspace et suppr


Difficultés rencontrées :
- le passage du mode normal au mode insertion une fois la ligne de commande commencée.
- passage du mode insertion au mode normal, on doit presser ECHAP ( c == 27 ),
cela provoque conflits avec la détection des flèches.