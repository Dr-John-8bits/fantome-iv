# Liste de courses Fantôme IV

Cette liste suit la route retenue pour Fantôme IV :

- `Daisy Seed 65MB`
- `8 potentiomètres`
- `1 encodeur rotatif cliquable cranté`
- `4 boutons poussoirs`
- `1 écran OLED 128x64` `1,3"` `I2C`
- `1 LED MIDI`
- `MIDI In` physique
- sortie audio stéréo

L'idée est d'acheter seulement ce qui sert à notre vrai prototype, sans ouvrir de piste parallèle.

Pour la version détaillée, exacte et référencée, voir [BOM.md](./BOM.md).

Pour les références exactes retenues, voir aussi [COMPONENT-REFERENCES.md](./COMPONENT-REFERENCES.md).

Pour l'ordre de lecture et de montage, voir aussi [INSTALLATION-MATERIEL.md](./INSTALLATION-MATERIEL.md).

Important :

- l'USB sert au développement et au flash
- l'usage musical normal de la V1 passe par `MIDI DIN 5 broches`

## À acheter maintenant

- 1 `Daisy Seed 65MB`
- 1 câble `micro-USB` de données
- 1 ou 2 breadboards
- un lot de fils Dupont
- 10 potentiomètres `Alpha RD901F-40-15F-B10K-00DL1`
- 2 encodeurs `Bourns PEC11R-4225F-S0024`
- 6 boutons poussoirs de panneau `APEM IPR3FAD7/1`
- 1 à 2 écrans `BuyDisplay ER-OLEDM013-1W-I2C`
- 1 LED MIDI
- 1 petit lot de résistances et condensateurs de base
- 1 jack `Switchcraft 155`
- 1 connecteur MIDI In `Switchcraft 57GB5FX`
- 2 optocoupleurs `onsemi H11L1M`
- 1 résistance `220R`, 1 résistance `1k`, 1 diode `1N4148` et 1 condensateur `100nF` pour le bloc MIDI In
- 1 résistance `1k` pour la LED MIDI
- 1 casque ou enceintes de monitoring
- 1 clavier MIDI ou contrôleur MIDI que tu as déjà

Pourquoi cette base :

- elle colle exactement à l'interface prévue pour Fantôme IV
- on peut brancher progressivement les vrais contrôles du synthé
- elle évite de développer sur un matériel intermédiaire différent du projet final

## Quantités du projet

Pour Fantôme IV V1 lui-même, la cible actuelle est :

- `8 potentiomètres`
- `1 encodeur rotatif cliquable cranté`
- `4 boutons poussoirs`
- `1 LED MIDI`

Conseil achat :

- acheter `10 potentiomètres` si tu veux `2` pièces de marge
- acheter `2 encodeurs` si tu veux `1` pièce de secours
- acheter `6 boutons` si tu veux `2` pièces de rab

## À acheter plus tard

Pas besoin de les acheter tout de suite :

- PCB custom
- boîtier
- knobs définitifs
- alimentation définitive
- étage de sortie audio final
- carte ou circuit MIDI final
- éventuel ampli casque final

## Outil utile mais non obligatoire

- 1 `ST-Link V3 Mini` pour debug

Ce n'est pas obligatoire pour flasher la carte, parce que Daisy peut être programmée en USB, mais c'est très utile si on veut debugger proprement.

## Conseils pratiques

- prends un vrai câble USB de données, pas seulement charge
- prévois un peu de marge sur les potentiomètres et boutons
- vise directement des composants de panneau si possible
- pour l'écran OLED, vise directement la référence retenue pour éviter les modules `1,3"` ambigus

## Références de travail

- [BOM.md](./BOM.md)
- [INSTALLATION-MATERIEL.md](./INSTALLATION-MATERIEL.md)

## Sources officielles utiles

Produits Daisy et documentation officielle consultés le 19 avril 2026 :

- Daisy Seed : https://electro-smith.com/products/daisy-seed
- Daisy Hardware Overview : https://daisy.audio/hardware/
- Toolchain macOS : https://daisy.audio/tutorials/toolchain-mac/
- C++ Getting Started : https://daisy.audio/tutorials/cpp-dev-env/
