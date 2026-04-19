# Fantôme IV

Fantôme IV est un projet de synthétiseur numérique polyphonique construit autour d'une Daisy Seed.

L'objectif est de réaliser une V1 solide, jouable et finissable : 4 voix, MIDI In DIN, sortie stéréo, OLED, presets, modulations musicales simples et effets globaux utiles.

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
- ADSR VCA + AR filtre par voix
- LFO pitch, LFO filtre, Sample & Hold filtre
- chorus, delay et reverb globaux
- interface OLED + encodeur + boutons + potentiomètres
- firmware en C++ sur Daisy Seed

## Contenu du dépôt

- [Fantome-IV-specs.md](./Fantome-IV-specs.md) : spécifications fonctionnelles V1
- [DOCUMENTATION.md](./DOCUMENTATION.md) : règles de vérité documentaire du projet
- [DECISIONS.md](./DECISIONS.md) : journal des décisions structurantes
- [BOM.md](./BOM.md) : nomenclature matérielle du prototype
- [ROADMAP.md](./ROADMAP.md) : feuille de route du projet
- [LISTE-COURSES.md](./LISTE-COURSES.md) : version simple de la liste d'achats
- [index.html](./index.html) : page web publique du projet
- `F-IV-Logo.png` / `F-IV-Logo.webp` : identité visuelle

## Règle de travail

Le projet suit désormais une règle simple :

- la documentation est la source de vérité
- les décisions importantes doivent être écrites
- la phase 1 firmware ne démarre pas tant que la documentation initiale n'est pas assez solide

Voir aussi [DOCUMENTATION.md](./DOCUMENTATION.md).

## Licence

Pour garder quelque chose de simple et correct juridiquement :

- le code, le firmware et les fichiers logiciels sont sous licence MIT, voir [LICENSE](./LICENSE)
- la documentation, les spécifications, le site et les visuels sont sous licence CC BY 4.0, voir [LICENSE-CONTENT.md](./LICENSE-CONTENT.md)

Creative Commons est une bonne licence pour les contenus, mais ce n'est pas le meilleur choix pour du code. Pour le logiciel, MIT est une base ouverte, simple et standard.

## Statut

Le projet est au stade de cadrage :

- specs V1 rédigées
- gouvernance documentaire en place
- décisions structurantes consignées
- page projet simplifiée
- base documentaire du dépôt en place
- route matérielle fixée sur Daisy Seed 65MB
- phase 1 firmware volontairement en attente de consolidation documentaire

## Prochaine étape

Solidifier encore la documentation initiale :

- verrouiller les derniers besoins fonctionnels ambigus
- compléter la documentation hardware de référence
- détailler la stratégie MIDI et presets
- figer le niveau de détail attendu avant la phase 1
