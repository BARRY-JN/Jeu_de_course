ALLIO David - david.allio@etu.univ-amu.fr
BARRY Jean-Noël - jean-noel.barry@etu.univ-amu.fr


== PROJET JEU DE COURSE 2D EN RÉSEAU ===


-- COMPILATION --

Pour compiler:
>make all 

Pour nettoyer:
>make clean


-- UTILISATION --

Utiliser le menu "Mode" pour sélectionner le mode désiré:

@ Jouer: permer de lancer une partie de 1 à 3 joueurs sur une route sélectionnée (ou la route actuelle)
@ Edition: permet de construire, modifier et sauvegarder la route actuelle

Les autres modes ne sont pas implémentés


-- FAIT --

@ Edition de route avec sauvegarde et choix du mode de rendu (rendu final ou filaire)
@ Chargement d'une route à partir d'un fichier
@ Mode Solo/Multijoueur: Choix du nombre de voiture dans l'interface, compte à rebour, Conduite, pause, freinage progressif quand on n'accélère pas
@ Tracage des zones de collision pour la première voiture


-- CODE --

@ Chaque voiture a une accélération et une vitesse propre.
max_speed et max_accelerator définissent la puissance de la voiture tandis que speed et accelerator définissent la vitesse et l'accélération à l'instant

@ Les variables:

proche,proche_b,x_gen,y_gen,x_genb,y_genb,xb_gen,yb_gen,xb_genb,yb_genb

Sont des variables de debug:
-proche = collision a un coté de la route de façon générale
-x_gen (pour x général),y_gen,x_genb,y_genb permettent d'afficher le segment proche de la voiture sur lequel on va faire plus de test sur les collisions

@ La fenêtre pour jouer et la gestion des sauvegardes se fait dans gui.c 

@ Les collisions sont détectées et gérées dans collision.c 