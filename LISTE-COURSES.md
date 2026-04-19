# Liste de courses Fantôme IV

Cette liste est pensée pour démarrer vite, sans suracheter trop tôt.

## Recommandation simple

Pour commencer proprement, je recommande :

- option la plus pratique : `Daisy Pod`
- option la moins chère et la plus flexible : `Daisy Seed` + breadboard + composants d'interface

Pour un premier proto firmware, le `Pod` est le meilleur raccourci. Pour un proto proche de la machine finale, le `Seed` nu est plus logique.

## Option A - Démarrage rapide recommandé

À acheter maintenant :

- 1 `Daisy Pod`
- 1 câble `micro-USB` de données
- 1 casque ou enceintes de monitoring
- 1 clavier MIDI ou contrôleur MIDI que tu as déjà
- 1 adaptateur MIDI si nécessaire

Pourquoi :

- le Pod inclut déjà un `Daisy Seed 65MB`
- tu as directement l'audio stéréo, une sortie casque, une entrée MIDI TRS, un encodeur et des boutons
- on peut commencer le firmware sans breadboard ni carte perso

## Option B - Démarrage orienté proto matériel

À acheter maintenant :

- 1 `Daisy Seed` 65MB
- 1 câble `micro-USB` de données
- 1 breadboard ou plaque de prototypage
- un lot de fils Dupont
- 8 à 10 potentiomètres linéaires `10k`
- 1 encodeur rotatif avec bouton poussoir
- 4 à 6 boutons poussoirs momentané
- 1 écran `OLED 128x64` I2C ou SPI
- 1 petit lot de résistances et condensateurs de base
- 2 jacks audio pour les sorties
- 1 connecteur MIDI In

Pourquoi :

- cette option colle mieux à l'interface prévue pour Fantôme IV
- on peut brancher progressivement les vrais contrôles du synthé
- c'est plus de câblage, mais plus proche du produit final

## À acheter plus tard

Pas besoin de les acheter tout de suite :

- PCB custom
- façade définitive
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
- si tu pars sur le Pod, ne rachète pas un Seed séparé tout de suite
- si tu pars sur le Seed nu, prévois un peu de marge sur les potentiomètres et boutons
- pour l'écran OLED, un module simple et courant est préférable au début

## Sources officielles utiles

Produits Daisy et documentation officielle consultés le 19 avril 2026 :

- Daisy Seed : https://electro-smith.com/products/daisy-seed
- Daisy Pod : https://electro-smith.com/products/pod
- Daisy Hardware Overview : https://daisy.audio/hardware/
- Toolchain macOS : https://daisy.audio/tutorials/toolchain-mac/
- C++ Getting Started : https://daisy.audio/tutorials/cpp-dev-env/

## Choix recommandé pour nous

Si l'objectif est de commencer vite avec peu de friction :

- achète un `Daisy Pod`

Si l'objectif est d'approcher rapidement la machine finale avec ses contrôles :

- achète un `Daisy Seed`
- ajoute breadboard, pots, boutons, encodeur et OLED
