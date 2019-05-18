Projet 2019 - Clone de vim

Sources : clone.c editor.c terminal.c mode.c mice.c clone.h editor.h terminal.h mode.h mice.h
Executable : ./clone

Membres du groupe :

Lynda Trad                                  - u21604852

Nadeesha Hatharasingha Koditthuwakku Ar     - u21610524

Participation : 

Lynda : 

- passage du mode canonique au mode non canonique
- passage d'un mode à l'autre ( Normal, Insertion )
- gestion du mode normal (parsing et execution de la commande)
- afficher le fichier en argument
- detection de backspace et DEL et leur comportement sur le buffer et le curseur
- écrire sur une feuille blanche
- se déplacer dans un texte et le modifier
- poll pour manipuler fleches et souris en même temps
- modifications dans les fonctions impliquant le curseur

Nadeesha :

- détecter quelle flèche a été pressée
- afficher le curseur
- fonctions pour bouger le curseur selon les flèches
- fonctions pour avoir la position courante ou le nombre de caractères dans une ligne du buffer.
- début de la mise en place du curseur de la souris


Difficultés rencontrées :

- le passage du mode normal au mode insertion une fois la ligne de commande commencée;
    solution trouvée : une commande :i pour passer en mode insertion si la commande a commencé.

- passage du mode insertion au mode normal, on doit presser ECHAP ( c == 27 ), cela provoque des
conflits avec la détection des flèches;
    solution trouvée : ECHAP est remplacé par TAB.

- presser backspace et delete plusieurs fois donnent parfois lieu à des problèmes
d'affichage du buffer, il faut passer en mode normal pour voir ce qu'est réellement le buffer.

- le curseur ne s'arrete pas en bout de ligne

- Segfault : lorsque l'on veut descendre sur une ligne qui est vide; lorsque l'on veut effacer avec backspace un char
 alors qu'on est en haut a gauche (dans certains cas);

- Ecriture dans l'éditeur de texte bloquée si on efface tous les caractères du fichier avec DEL (peut être un problème
de curseur)