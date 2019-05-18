
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


Fonctionnement :

Quand clone est ouvert avec un fichier en argument, on peut le parcourir avec les flèches,
effacer et ajouter du texte.
On peut passer en mode normal pour le sauvegarder, ou repasser en mode Insertion pour continuer l'écriture.

Limites :

- Pour l'édition de texte, selon qu'on ajoute on qu'on retire un char, le curseur n'agit pas de la même façon.
Pour ajouter un caractère il faut se placer sur le caractère avant l'endroit où on veut commencer à ecrire.
Si l'on veut supprimer un caractère avec DEL, il faut se placer sur le caractère à supprimer.
Enfin, pour utiliser backspace il faut se placer sur le caractère après celui qu'on veut effacer.

- La souris n'est pas prise en compte, le curseur sur le texte ne change pas après un clic de la souris,
le curseur de la souris ne s'affiche pas non plus.


Difficultés rencontrées :

- Le passage du mode normal au mode insertion une fois la ligne de commande commencée.
    La solution trouvée : une commande :i pour passer en mode insertion si la commande a commencé.

- Le passage du mode insertion au mode normal. On doit presser ECHAP ( c == 27 ), cela provoque des
conflits avec la détection des flèches qui sont des escape sequence.
    La solution trouvée : ECHAP est remplacé par TAB.

- Presser backspace et delete plusieurs fois donnent parfois lieu à des problèmes
d'affichage du buffer, il faut passer en mode normal pour voir ce qu'est réellement le buffer.
Une fois revenu en mode insertion on peut se déplacer et modifier le fichier.

- Le curseur ne s'arrete pas en bout de ligne, il peut flotter dans le vide.

- Segfault : lorsque l'on veut effacer avec backspace un char alors qu'on est en haut a gauche (dans certains cas).

- Ecriture dans l'éditeur de texte bloquée si on efface tous les caractères du fichier avec DEL (peut être un problème
de curseur)
