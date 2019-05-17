Projet 2019 - Clone de vim

Sources : clone.c editor.c terminal.c mode.c clone.h editor.h terminal.h mode.h
Executable : ./clone

Membres du groupe :

Lynda Trad                                  - u21604852

Nadeesha Hatharasingha Koditthuwakku Ar     - u21610524

Participation : 

Lynda : 

- passage du mode canonique au mode non canonique
- passage d'un mode à l'autre
- gestion du mode normal (parsing et execution de la commande)
- afficher le fichier en argument
- detecter backspace et DEL
- comportement du buffer et du curseur si on presse backspace et suppr
- écrire sur une feuille blanche
- se déplacer dans un texte et le modifier

Nadeesha :

- détecter quelle flèche a été pressée
- fonctions pour bouger le curseur dans un fichier en mode Insertion 
- fonctions pour avoir la position courante ou le nombre de caractères dans une ligne du buffer.
- afficher le curseur
- début de la mise en place du curseur de la souris


Difficultés rencontrées :

- le passage du mode normal au mode insertion une fois la ligne de commande commencée;
    solution trouvée : une commande :i pour passer en mode insertion si la commande a commencé.
- passage du mode insertion au mode normal, on doit presser ECHAP ( c == 27 ), cela provoque des
conflits avec la détection des flèches;
    solution trouvée : ECHAP est remplacé par TAB.
- le curseur ne s'arrete pas en bout de ligne apart lorsqu'on ecrit sur une feuille blanche.
- presser backspace et delete plusieurs fois donnent parfois lieu à des problèmes
d'affichage du buffer, il faut passer en mode normal pour voir ce qu'est réellement le buffer.
- backspace : segfault lorsqu'on est en haut à gauche,
pour effacer un caractere avec backspace il faut se mettre sur le caractère à sa droite.