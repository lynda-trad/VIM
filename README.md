
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

- Il y a une longueur limite pour les textes, on ne peut pas afficher plus loin que la taille du terminal
qui est 120x50.

- La souris n'est pas prise en compte, le curseur sur le texte ne change pas après un clic de la souris,
le curseur de la souris ne s'affiche pas non plus.


Difficultés rencontrées :

- Le passage du mode normal au mode insertion une fois la ligne de commande commencée.
    La solution trouvée est une commande :i pour passer en mode insertion si la commande a commencé.
    Si elle n'a pas commencé, on peut cliquer sur i.

- Le passage du mode insertion au mode normal. On doit presser ECHAP ( c == 27 ), cela provoque des
conflits avec la détection des flèches qui sont des escape sequence.
    La solution trouvée : ECHAP est remplacé par TAB ( c == 9 ).

- Presser backspace et delete plusieurs fois donnent parfois lieu à des problèmes
d'affichage du buffer, il faut passer en mode normal pour voir ce qu'est réellement le buffer.
Une fois revenu en mode insertion, on peut se déplacer et modifier le fichier normalement.

- Le curseur ne s'arrete pas en bout de ligne, il peut flotter dans le vide. L'écriture dans le vide n'est pas prise
en compte, il faut se mettre sur un caractère pour écrire les caractères qui le suivent.

- Ecriture dans l'éditeur de texte bloquée si on efface tous les caractères du fichier avec DEL (peut être un problème
de memmove). Pas de solution trouvée, le seul moyen de se débloquer est d'effacer le " vide " avec backspace et delete
et d'écrire jusqu'à que quelque chose s'affiche.

- Le fichier mice ne recevait aucune écriture, nous ne pouvions donc pas verifier notre code et modifier le curseur.