# Fantôme IV

Fantôme IV est un projet de synthétiseur numérique polyphonique construit autour d'une Daisy Seed.

L'objectif est de réaliser une V1 solide, jouable et finissable : 4 voix, MIDI In DIN, sortie stéréo, OLED, presets, modulations musicales simples et effets globaux utiles.

## Direction retenue

Le projet est désormais figé sur une base matérielle claire :

- `Daisy Seed 65MB`
- `8 potentiomètres`
- `1 encodeur rotatif cliquable`
- `4 boutons poussoirs de panneau`
- `1 écran OLED 128x64 1,3" I2C`
- `1 LED MIDI`
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

- [docs/Fantome-IV-specs.md](./docs/Fantome-IV-specs.md) : spécifications fonctionnelles V1
- [docs/DOCUMENTATION.md](./docs/DOCUMENTATION.md) : règles de vérité documentaire du projet
- [docs/DECISIONS.md](./docs/DECISIONS.md) : journal des décisions structurantes
- [docs/HARDWARE-REFERENCE.md](./docs/HARDWARE-REFERENCE.md) : référence hardware de la V1
- [docs/ALIMENTATION-POWER-SPEC.md](./docs/ALIMENTATION-POWER-SPEC.md) : stratégie d'alimentation et de masses
- [docs/MECHANICAL-SPEC.md](./docs/MECHANICAL-SPEC.md) : direction mécanique de la V1
- [docs/PINMAP-DAISY.md](./docs/PINMAP-DAISY.md) : brochage retenu de la Daisy Seed
- [docs/MIDI-IN-HARDWARE-SPEC.md](./docs/MIDI-IN-HARDWARE-SPEC.md) : bloc MIDI In détaillé
- [docs/AUDIO-OUT-SPEC.md](./docs/AUDIO-OUT-SPEC.md) : topologie détaillée de sortie audio
- [docs/COMPONENT-REFERENCES.md](./docs/COMPONENT-REFERENCES.md) : références exactes des composants retenus
- [docs/SCHEMA-SPEC.md](./docs/SCHEMA-SPEC.md) : spec de schéma du prototype
- [docs/UI-UX-SPEC.md](./docs/UI-UX-SPEC.md) : spec d'interface utilisateur
- [docs/MIDI-SPEC.md](./docs/MIDI-SPEC.md) : spec MIDI de la V1
- [docs/PHASE-0-AUDIT.md](./docs/PHASE-0-AUDIT.md) : audit d'avancement de la phase 0 documentaire
- [docs/BOM.md](./docs/BOM.md) : nomenclature matérielle du prototype
- [docs/ROADMAP.md](./docs/ROADMAP.md) : feuille de route du projet
- [docs/LISTE-COURSES.md](./docs/LISTE-COURSES.md) : version simple de la liste d'achats
- [index.html](./index.html) : page web publique du projet
- `assets/branding/` : identité visuelle

## Arborescence

- `docs/` : toute la source de vérité documentaire
- `assets/branding/` : logos et identité visuelle
- `index.html` : page publique simple du projet
- `LICENSE` / `LICENSE-CONTENT.md` : licences du dépôt

## Règle de travail

Le projet suit désormais une règle simple :

- la documentation est la source de vérité
- les décisions importantes doivent être écrites
- la phase 1 firmware ne démarre pas tant que la documentation initiale n'est pas assez solide

Voir aussi [docs/DOCUMENTATION.md](./docs/DOCUMENTATION.md).

## Licence

Pour coller à l'intention du projet, on a retenu les licences les plus permissives et simples possibles :

- le code, le firmware et les fichiers logiciels sont sous licence `0BSD`, voir [LICENSE](./LICENSE)
- la documentation, les spécifications, le site et les visuels sont dédiés au domaine public sous `CC0 1.0`, voir [LICENSE-CONTENT.md](./LICENSE-CONTENT.md)

Le but est explicite : donner le maximum de liberté de réutilisation, sans obligation d'attribution pour les contenus, et avec une licence logicielle extrêmement permissive côté code.

## Statut

Le projet est au stade de cadrage :

- specs V1 rédigées
- gouvernance documentaire en place
- décisions structurantes consignées
- référence hardware de travail en place
- spec de schéma en place
- spec UI/UX en place
- spec MIDI en place
- pin map Daisy en place
- bloc MIDI In détaillé en place
- spec détaillée de sortie audio en place
- références exactes de composants en place
- spec d'alimentation en place
- spec mécanique en place
- audit de phase 0 en place
- page projet simplifiée
- base documentaire du dépôt en place
- route matérielle fixée sur Daisy Seed 65MB
- phase 1 firmware volontairement en attente de consolidation documentaire

## Prochaine étape

Solidifier encore la documentation initiale :

- dessiner le schéma V1 à partir des documents désormais figés
- finaliser les derniers détails UI et mécaniques encore ouverts
- cadrer l'alimentation autonome future au-delà du prototype USB
