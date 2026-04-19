# Liste de courses Fantôme IV

Cette liste suit la route retenue pour Fantôme IV :

- `Daisy Seed 65MB`
- `8 potentiomètres`
- `1 encodeur rotatif cliquable cranté`
- `4 boutons poussoirs`
- `1 écran OLED 128x64` `SSD1306` `1,3"`
- `1 LED MIDI`
- `MIDI In` physique
- sortie audio stéréo

L'idée est d'acheter seulement ce qui sert à notre vrai prototype, sans ouvrir de piste parallèle.

Pour la version détaillée et structurée, voir aussi [BOM.md](./BOM.md).

Important :

- l'USB sert au développement et au flash
- l'usage musical normal de la V1 passe par `MIDI DIN 5 broches`

## À acheter maintenant

- 1 `Daisy Seed 65MB`
- 1 câble `micro-USB` de données
- 1 breadboard ou plaque de prototypage
- un lot de fils Dupont
- 8 potentiomètres linéaires `10k` de panneau
- 1 encodeur rotatif cranté avec bouton poussoir
- 4 boutons poussoirs momentané NO de panneau
- 1 écran `OLED 128x64` `SSD1306` `I2C` `1,3"`
- 1 LED MIDI
- 1 petit lot de résistances et condensateurs de base
- 1 jack TRS stéréo `6,35 mm` pour la sortie audio
- 1 connecteur MIDI In
- 1 optocoupleur `H11L1` ou compatible pour l'entrée MIDI
- 1 résistance `220R`, 1 résistance `270R` et 1 condensateur `100nF` pour le bloc MIDI In de départ
- 1 résistance de LED adaptée à la signalisation MIDI
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
- pour l'écran OLED, privilégie un `1,3"` lisible

## Sources officielles utiles

Produits Daisy et documentation officielle consultés le 19 avril 2026 :

- Daisy Seed : https://electro-smith.com/products/daisy-seed
- Daisy Hardware Overview : https://daisy.audio/hardware/
- Toolchain macOS : https://daisy.audio/tutorials/toolchain-mac/
- C++ Getting Started : https://daisy.audio/tutorials/cpp-dev-env/
