# Installation Matériel Fantôme IV

Ce document décrit comment passer de la réception du matériel à un prototype physique propre.

Il ne remplace pas les specs. Il donne l'ordre de lecture et l'ordre d'action.

## Documents à lire impérativement

Avant de brancher quoi que ce soit, lire dans cet ordre :

1. [README.md](../README.md)
   Rôle : vue d'ensemble du projet et état réel du dépôt.

2. [BOM.md](./BOM.md)
   Rôle : quoi acheter exactement.

3. [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
   Rôle : architecture matérielle globale.

4. [PINMAP-DAISY.md](./PINMAP-DAISY.md)
   Rôle : quelles broches Daisy font quoi.

5. [ALIMENTATION-POWER-SPEC.md](./ALIMENTATION-POWER-SPEC.md)
   Rôle : rails, masses, discipline de câblage.

6. [AUDIO-OUT-SPEC.md](./AUDIO-OUT-SPEC.md)
   Rôle : câblage de la sortie stéréo.

7. [MIDI-IN-HARDWARE-SPEC.md](./MIDI-IN-HARDWARE-SPEC.md)
   Rôle : câblage exact du MIDI In DIN.

8. [UI-UX-SPEC.md](./UI-UX-SPEC.md)
   Rôle : comprendre ce que doivent faire les contrôles physiques.

9. [SOFTWARE-ARCHITECTURE.md](./SOFTWARE-ARCHITECTURE.md)
   Rôle : comprendre où le hardware se raccorde au runtime.

## Pré-requis établis

Avant l'assemblage matériel :

- le dépôt doit compiler localement
- les tests doivent passer
- le Mac doit être prêt pour la toolchain Daisy

Références externes utiles :

- [Daisy C++ Getting Started](https://daisy.audio/tutorials/cpp-dev-env/)
- [Daisy Seed Hardware](https://daisy.audio/hardware/Seed/)
- [Daisy Bootloader](https://daisy.audio/tutorials/_a7_Getting-Started-Daisy-Bootloader/)
- [Daisy ADC Inputs](https://daisy.audio/tutorials/_a4_Getting-Started-ADCs/)

## Philosophie de montage

On ne monte pas tout d'un coup.

On procède par couches :

1. valider la `Daisy Seed` seule
2. valider l'alimentation USB et le flash
3. valider l'audio
4. valider un seul potentiomètre
5. valider les `8` potentiomètres
6. valider encodeur, boutons et LED MIDI
7. valider l'écran OLED
8. valider le bloc `MIDI In`
9. seulement après, brancher tout le panneau en même temps

## Step by step

### 1. Réception et contrôle du matériel

À la réception :

- vérifier que la `Daisy Seed` est bien la version `65MB`
- vérifier la référence exacte de l'écran `ER-OLEDM013-1W-I2C`
- vérifier que les `8` potentiomètres ont bien un axe plat `6 mm`
- vérifier que l'encodeur a bien un switch intégré
- vérifier que les boutons sont bien momentané `NO`
- vérifier que le jack audio est bien `TRS stéréo`
- vérifier que l'embase MIDI est bien une `DIN 5` de panneau

Si une référence critique n'est pas la bonne :

- ne pas improviser
- mettre à jour la doc avant de continuer

### 2. Préparer le poste de travail

Préparer :

- le Mac
- le dépôt local
- le câble USB données
- la breadboard
- les fils
- une zone de travail claire
- un casque ou des enceintes de monitoring
- un clavier MIDI

### 3. Valider la Daisy seule

Ordre recommandé :

- connecter la `Daisy Seed` seule au Mac
- installer la toolchain Daisy côté Mac
- flasher un exemple minimal de type `Blink` pour valider USB + bootloader

Objectif :

- vérifier que la carte est saine avant d'ajouter du câblage

Ne pas brancher immédiatement :

- l'audio
- le MIDI DIN
- l'OLED
- tous les contrôles en même temps

### 4. Monter le support physique de la Daisy

Ensuite :

- installer les headers nécessaires
- fixer la carte sur support propre ou breadboard
- garder l'accès facile aux boutons `BOOT` et `RESET`
- éviter toute contrainte mécanique sur le module

### 5. Poser les rails d'alimentation de bench

Sur la breadboard :

- préparer un rail `+3V3A`
- préparer un rail `AGND`
- préparer un rail `+3V3D`
- préparer un rail `DGND`

Règle :

- garder la séparation analogique / numérique lisible
- respecter la jonction unique des masses telle que documentée

Voir :

- [ALIMENTATION-POWER-SPEC.md](./ALIMENTATION-POWER-SPEC.md)

### 6. Câbler la sortie audio

Premier bloc physique à câbler :

- `pin 18` Daisy vers `tip`
- `pin 19` Daisy vers `ring`
- `pin 20 AGND` vers `sleeve`

Objectif :

- valider le chemin audio stéréo avant le reste

Voir :

- [AUDIO-OUT-SPEC.md](./AUDIO-OUT-SPEC.md)

### 7. Câbler un seul potentiomètre de test

Avant d'en brancher huit :

- brancher seulement `POT_2_CUTOFF` sur `A1 / D16`
- haut de piste vers `+3V3A`
- bas de piste vers `AGND`
- curseur vers l'entrée ADC retenue

Objectif :

- valider le schéma de câblage analogique une première fois

Ensuite seulement :

- dupliquer pour `A0` à `A7`

Voir :

- [PINMAP-DAISY.md](./PINMAP-DAISY.md)

### 8. Câbler encodeur, boutons et LED MIDI

Câbler ensuite :

- `D0` = `ENC_A`
- `D1` = `ENC_B`
- `D2` = `ENC_SW`
- `D3` = `BTN_PAGE_PREV`
- `D4` = `BTN_PAGE_NEXT`
- `D5` = `BTN_SHIFT`
- `D6` = `BTN_ACTION`
- `D7` = `LED_MIDI`

Convention :

- boutons actifs bas
- pull-up internes autorisés pour le prototype

### 9. Câbler l'écran OLED

Câbler ensuite :

- `D11` = `OLED_SCL`
- `D12` = `OLED_SDA`
- alimentation OLED sur `+3V3D`
- masse OLED sur `DGND`

Objectif :

- valider ensuite le splash screen et la navigation UI

### 10. Câbler le MIDI In DIN

Ne câbler le MIDI qu'après l'alimentation, l'audio et l'OLED.

Le bloc retenu est :

- embase `Switchcraft 57GB5FX`
- optocoupleur `H11L1M`
- `220R`
- `1k`
- `1N4148`
- `100nF`
- sortie opto vers `D14`

Voir impérativement :

- [MIDI-IN-HARDWARE-SPEC.md](./MIDI-IN-HARDWARE-SPEC.md)

### 11. Premier bring-up Fantôme IV

Quand tout le bench physique est prêt :

- reflasher la Daisy avec le firmware de test retenu
- vérifier le splash screen OLED
- vérifier la sortie audio gauche/droite
- vérifier qu'un potard agit bien sur le paramètre attendu
- vérifier l'encodeur et les `4` boutons
- vérifier la LED MIDI
- vérifier qu'un clavier MIDI déclenche bien des notes via DIN

## Ordre de validation recommandé

Ne jamais tout déboguer en même temps.

Ordre recommandé :

1. USB / bootloader
2. audio out
3. un potard
4. les `8` potards
5. encodeur et boutons
6. OLED
7. MIDI In DIN
8. intégration complète

## Critères de succès du premier prototype

On considère le prototype matériel V1 correctement démarré quand :

- la `Daisy Seed` flash correctement
- l'écran affiche bien le splash screen
- la sortie audio est présente sur les deux canaux
- le cutoff réagit au bon potentiomètre
- l'encodeur navigue dans l'UI
- les `4` boutons répondent
- la LED MIDI clignote à la réception
- un clavier MIDI DIN joue bien le synthé

## Ce qu'il ne faut pas faire

- ne pas alimenter le prototype par plusieurs chemins à la fois sans l'avoir prévu
- ne pas mélanger les masses analogiques et numériques n'importe comment
- ne pas câbler les `8` potards d'un coup sans avoir validé le premier
- ne pas brancher le MIDI In “au jugé” sans suivre la spec
- ne pas supposer qu'un module OLED `1,3"` quelconque est forcément compatible

## Documents liés

- [BOM.md](./BOM.md)
- [PINMAP-DAISY.md](./PINMAP-DAISY.md)
- [ALIMENTATION-POWER-SPEC.md](./ALIMENTATION-POWER-SPEC.md)
- [AUDIO-OUT-SPEC.md](./AUDIO-OUT-SPEC.md)
- [MIDI-IN-HARDWARE-SPEC.md](./MIDI-IN-HARDWARE-SPEC.md)
- [SOFTWARE-ARCHITECTURE.md](./SOFTWARE-ARCHITECTURE.md)
