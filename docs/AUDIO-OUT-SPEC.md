# Spec Sortie Audio Fantôme IV

Ce document fige la topologie de sortie audio V1 de Fantôme IV.

## Rôle

La sortie audio V1 doit fournir :

- une sortie ligne stéréo simple
- exploitable immédiatement
- cohérente avec la `Daisy Seed`
- sans complexifier inutilement le prototype

## Base technique Daisy

La `Daisy Seed` embarque déjà sa propre condition de sortie ligne.

Les documents officiels Daisy indiquent :

- `AUDIO OUT L` sur la broche `18`
- `AUDIO OUT R` sur la broche `19`
- `AGND` sur la broche `20`
- un niveau ligne nominal d'environ `1 Vrms` à pleine échelle

Le schéma de la Seed montre en plus, sur chaque canal :

- un condensateur de liaison `4,7 uF`
- une résistance `47 kohms` vers `AGND`
- une résistance série `100 ohms`

## Décision retenue pour Fantôme IV V1

La topologie retenue est volontairement simple :

- aucune carte de sortie active supplémentaire en V1
- aucune recopie de condensateurs de liaison déjà présents sur la Seed
- aucune atténuation passive supplémentaire
- aucun ampli casque
- routage direct des sorties ligne de la Seed vers un jack `TRS 6,35 mm`

## Topologie exacte retenue

```text
Daisy pin 18 / AUDIO OUT L ----> Tip du jack TRS 6,35 mm
Daisy pin 19 / AUDIO OUT R ----> Ring du jack TRS 6,35 mm
Daisy pin 20 / AGND -----------> Sleeve du jack TRS 6,35 mm
```

## Référence de connecteur retenue

Le connecteur de référence V1 est :

- `Switchcraft 155`

Pourquoi :

- vrai composant de panneau
- format `6,35 mm TRS` standard
- crédible pour un instrument desktop
- cohérent avec un prototype proche de la façade finale

## Niveau et usage retenus

La sortie V1 est une :

- `sortie ligne stéréo`

Elle n'est pas pensée comme :

- une sortie casque
- une sortie symétrique
- une sortie adaptée à de longs câbles de scène

Usage visé :

- interface audio
- console
- moniteurs actifs
- chaîne de test de bureau

## Consignes de câblage

Pour limiter le bruit :

- garder les liaisons `AUDIO OUT L/R` aussi courtes que possible
- éviter de faire transiter l'audio dans de longues zones de breadboard si possible
- éloigner le chemin audio des liaisons `USB`, `OLED I2C` et `MIDI`
- si la distance vers le jack devient significative, préférer un câblage torsadé ou semi-blindé

## Masse retenue

Pour la V1 :

- le retour audio de la sortie jack utilise `AGND` de la Seed

Ce point est important :

- on ne fait pas revenir la sortie audio sur une masse numérique choisie au hasard

La stratégie châssis métallique définitive reste un sujet ultérieur.

Pour le prototype V1, la règle est simple :

- `sleeve -> AGND`

## Ce qui n'est pas retenu en V1

- pas de driver casque dédié
- pas d'ampli op de buffer ajouté juste par principe
- pas de sommation mono
- pas de jack séparé `L` / `R`

## Pourquoi cette topologie est la bonne pour la V1

Cette topologie est retenue parce qu'elle :

- s'appuie sur le conditionnement déjà prévu par la Seed
- réduit le nombre de composants externes
- réduit le risque d'erreur analogique au prototype
- reste cohérente avec le besoin réel de Fantôme IV

## Conditions de remise en question

Cette topologie ne devra être revue que si :

- on ajoute une vraie sortie casque
- on vise une autre norme de niveau
- on découvre au prototype un problème réel de bruit ou de charge

À ce stade, ce n'est pas le cas.
