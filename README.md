# GravitySim
GravitySim est un simulateur de planètes et de gravité codée en c(il est loin d'etre parfait donc je suis ouvert à vos conseils)

<img width="994" height="767" alt="Capture d'écran 2025-10-19 072908" src="https://github.com/user-attachments/assets/84c56014-3d2b-40de-9f0e-32623f957781" />
## Controles
- 0-9 : vitesse de simulation
- ZQSD ou WASD, Espace et Maj : déplacements
- p : Play/pause

## Fonctionnalité
- simulation réaliste des la gravité
- fusion des planètes quand elles se touchent
- création aléatoire des planètes au debut

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
