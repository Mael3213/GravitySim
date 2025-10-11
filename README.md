# GravitySim
GravitySim est un simulateur de planètes et de gravité codée en c par moi même(donc il est un peu brouillon)

## Controles
- 0-9 : vitesse de simulation
- ZQSD ou WASD, Espace et Maj : déplacements
- p : Play/pause

## Fonctionnalité
- simulation réaliste des la gravité
- fusion des planètes quand elles se touchent

## Bibliothèques
- Glut.h
- Math.h
- Windows.h

## Requis
- GCC pour compiler
- Windows pour executer sans GCC

## Modification planètes
Pour le moment il n'y a pas de fonctionnalité qui pourrai modifier les planètes donc il faut :
- modifier la variable planète (avec le format {x,y,z,vx,vy,vz,masse,r,g,b})
- modifier la variable nb_planete pour qu'elle corresponde au nombre de planètes
