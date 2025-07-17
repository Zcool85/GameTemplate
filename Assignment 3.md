# Assignment 3

Objectifs :

- Apprendre à manipuler les textures / animation.
- Apprendre à maitriser les collisions
- Apprendre à utiliser les tiles : tiles set & tiles map
- Apprendre à manipuler des assets

## Spécifications

Assets :

- Les entités du jeu sont rendues avec des textures et animations variées (ce sont des assets)
- Les assets sont chargés une seule fois tout au début du programme et stockés dans la class Assets
  qui elle-même est stockée dans la classe Game.
- Tous les assets sont définis dans un fichier assets.txt

Note :

- Toutes les positions des entités sont relatives au centre de leur sprite rectangulaire. Cette position
  correspond également au centre de la Bounding Box, s'il y a lieu d'être.
  Cette position est donnée par la fonction sprite.setOrigin() dans le constructeur de la classe Animation.

Joueur :

- L'entité du joueur est représenté par Megaman qui dispose de plusieurs animations : Standing, Run, Air. On doit
  déterminer le bon état du joueur pour pouvoir lui assigner la bonne animation.
- Le joueur peut se déplacer avec les controls suivants :
    + Gauche : Touche 'Q'
    + Droite : Touche 'D'
    + Jump : Touche 'Z'
    + Shoot : Touche espace
- Le joueur peut se déplacer à gauche, à droite ou tirer à n'importe quel moment du jeu. Cela veut dire que
  le joueur peut se déplacer à droite ou à gauche dans les airs
- Le joueur ne peut sauter que s'il est actuellement sur une tile
- Si la touche saut est appuyé, le joueur ne peut pas continuellement sauter. Il ne peut sauter qu'une fois
  à chaque pression du bouton. Si le joueur relâche la touche saut au milieu du saut, le joueur commencera
  à retomber immédiatement.
- Si le joueur se déplace à gauche ou a droite, son sprite fait face à la direction où il va tant que l'autre
  direction n'a pas été appuyée.
- Les balles tirées par le joueur se déplace horizontalement dans la direction du joueur.
- Le joueur entre en collision avec les entités 'Tiles' du niveau et ne peut pas les traverser. le joueur
  reste sur une entité Tile et reste sur place s'il tombe dessus depuis le haut.
- Le joueur n'entre pas en collision avec les décorations.
- Si le joueur tombe en dessous de l'écran, il réapparait au début du jeu.
- Le joueur doit avoir un composant de gravité qui accélère constamment le joueur vers le bas jusqu'à ce
  que le joueur entre en collision avec un Tile.
- Le joueur à une vitesse maximale configurée dans le fichier de configuration du jeu.
- Le joueur dispose d'un CBoundingBox d'une taille spécifié dans le fichier de définition du level.
- Le sprite du joueur et le bounding box sont centré sur la position du joueur.

Animations :

- Les animations sont implémentés en stockant plusieurs frames dans une texture.
- La classe Animation gère l'avancement de l'animation basé sur la vitesse d'animation.
- Il faut implémenter la fonction Animation::update() pour faire avancer l'animation
- Il faut implémenter la fonction Animation::hasEnded() qui retourne vrai si l'animation a
  terminé sa dernière frame. Faux dans le cas contraire.
- Les animations peuvent être infinies (repeating) ou être jouées une seule fois (non-repeating).
- Une entité avec une animation qui ne se répète pas doit être détruite dès lors que l'animation
  est terminée. Fonction hasEnded() == true.

Entité de décoration :

- Les entités d'animation sont simplement dessinés à l'écran et n'interagissent pas avec les autres
  entités du jeu.
- Il est possible de donner une animation aux entités de décoration (nuages, etc.).

Tiles :

- Les tiles sont des entités qui définissent la géométry du niveau et interagissent avec le joueur.
- Les tiles peuvent avoir une animation définie dans le fichier des assets.
- Les tiles ont un CBoundingBox égal à la taille de l'animation.
  tile->get<CAnimation>().animation.getSize()
- L'animation courante affiché pour un tile peut être retrouvé de la manière suivante :
  tile->get<CAnimation>().animation.getName()
- Les tiles ont différents comportements dépendant de l'animation en cours.

Briques Tiles :

- Les tiles briques ont l'animation 'Brick'
- Lorsqu'une brique tile entre en collision avec un tir, ou est hit par le joueur par le bas :
    + Son animation change pour 'Explosion' (non-repeat)
    + Les entités avec une animation non-repeat sont détruites lorsque l'animation est terminée
    + Son composant BoundingBox est supprimé

Question Tiles :
