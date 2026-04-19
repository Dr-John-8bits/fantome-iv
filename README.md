# Fantôme IV

Fantôme IV est un projet de synthétiseur numérique polyphonique construit autour d'une Daisy Seed.

L'objectif est de réaliser une V1 solide, jouable et finissable : 4 voix, MIDI In, sortie stéréo, OLED, presets, modulations musicales simples et effets globaux utiles.

## Direction retenue

Le projet est désormais figé sur une base matérielle claire :

- `Daisy Seed 65MB`
- `8 potentiomètres`
- `1 encodeur rotatif cliquable`
- `4 boutons`
- `1 écran OLED 128x64`
- `MIDI In` physique
- sortie audio stéréo

## Vision

- 4 voix de polyphonie
- 2 oscillateurs par voix
- filtre low-pass résonant par voix
- ADSR VCA + ADSR filtre par voix
- LFO pitch, LFO filtre, Sample & Hold filtre
- chorus, delay et reverb globaux
- interface OLED + encodeur + boutons + potentiomètres
- firmware en C++ sur Daisy Seed

## Contenu du dépôt

- [Fantome-IV-specs.md](./Fantome-IV-specs.md) : spécifications fonctionnelles V1
- [BOM.md](./BOM.md) : nomenclature matérielle du prototype
- [ROADMAP.md](./ROADMAP.md) : feuille de route du projet
- [LISTE-COURSES.md](./LISTE-COURSES.md) : version simple de la liste d'achats
- [index.html](./index.html) : page web publique du projet
- `F-IV-Logo.png` / `F-IV-Logo.webp` : identité visuelle

## Licence

Pour garder quelque chose de simple et correct juridiquement :

- le code, le firmware et les fichiers logiciels sont sous licence MIT, voir [LICENSE](./LICENSE)
- la documentation, les spécifications, le site et les visuels sont sous licence CC BY 4.0, voir [LICENSE-CONTENT.md](./LICENSE-CONTENT.md)

Creative Commons est une bonne licence pour les contenus, mais ce n'est pas le meilleur choix pour du code. Pour le logiciel, MIT est une base ouverte, simple et standard.

## Statut

Le projet est au stade de cadrage :

- specs V1 rédigées
- page projet simplifiée
- base documentaire du dépôt en place
- route matérielle fixée sur Daisy Seed 65MB
- architecture firmware à démarrer

## Prochaine étape

Mettre en place l'ossature firmware :

- moteur audio
- gestion de voix
- oscillateurs
- enveloppes
- filtre
- MIDI
- stockage des paramètres
