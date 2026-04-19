# Référence Hardware Fantôme IV

Ce document est la référence hardware de Fantôme IV V1.

Il décrit :

- l'architecture matérielle retenue
- les interfaces physiques du synthé
- la surface de contrôle
- les blocs à implémenter au schéma
- les points ouverts qui doivent être figés avant d'aller plus loin

Ce document ne remplace pas la spec fonctionnelle. Il traduit la V1 en base matérielle exploitable.

## Rôle dans le projet

Ce document sert à :

- préparer le schéma du prototype
- relier les besoins fonctionnels au vrai matériel
- éviter les ambiguïtés entre firmware, UI et câblage
- documenter proprement le projet pour l'open source

## Statut

Statut actuel :

- document de référence actif
- suffisant pour cadrer l'architecture matérielle
- encore incomplet sur certaines références exactes de composants

Les points non figés sont marqués `à figer`.

## Documents liés

- [Fantome-IV-specs.md](./Fantome-IV-specs.md)
- [DOCUMENTATION.md](./DOCUMENTATION.md)
- [DECISIONS.md](./DECISIONS.md)
- [BOM.md](./BOM.md)
- [ROADMAP.md](./ROADMAP.md)

## 1. Architecture matérielle V1 retenue

Fantôme IV V1 repose sur :

- `1 Daisy Seed 65MB`
- `1 entrée MIDI DIN 5 broches`
- `1 sortie ligne stéréo sur jack TRS 6,35 mm`
- `1 écran OLED 128x64`
- `8 potentiomètres`
- `1 encodeur rotatif cliquable`
- `4 boutons`

Principe général :

- l'USB sert au développement, au flash et au debug
- le jeu musical normal passe par l'entrée MIDI DIN
- la sortie audio V1 est une sortie ligne stéréo simple
- la face avant reste volontairement compacte et lisible

## 2. Vue système

```text
Mac (dev only)
    |
    +-- USB -> Daisy Seed 65MB

Clavier MIDI / séquenceur
    |
    +-- MIDI DIN 5 -> optocoupleur -> UART MIDI -> Daisy Seed 65MB

Face avant
    |
    +-- 8 potentiomètres -> ADC
    +-- 1 encodeur + switch -> GPIO
    +-- 4 boutons -> GPIO
    +-- OLED 128x64 -> I2C ou SPI

Daisy Seed 65MB
    |
    +-- audio stéréo -> étage de sortie ligne -> jack TRS 6,35 mm
```

## 3. Interfaces physiques retenues

### 3.1 MIDI

Interface musicale principale :

- `MIDI In DIN 5 broches`

Exigences :

- entrée `opto-isolée`
- fonctionnement identique avec ou sans USB branché
- pas de dépendance à l'USB pour jouer l'instrument

Décision :

- pas d'USB MIDI requis pour la V1
- pas de MIDI Out requis pour la V1

### 3.2 Audio

Sortie retenue :

- `1 sortie ligne stéréo sur jack TRS 6,35 mm`

Exigences :

- niveau propre et stable
- bruit faible
- usage cohérent avec un synthé desktop

Décision :

- pas de sortie casque dédiée en V1
- pas d'entrée audio en V1

### 3.3 Développement

Interface retenue :

- `micro-USB` vers la Daisy

Usage :

- flash du firmware
- alimentation de développement si nécessaire
- debug

Décision :

- l'USB n'est pas l'interface musicale du produit

## 4. Surface de contrôle V1

### 4.1 Contrôles retenus

- `8 potentiomètres`
- `1 encodeur rotatif cliquable`
- `4 boutons`
- `1 OLED 128x64`

### 4.2 Répartition retenue

Contrôles dédiés :

- Pot 1 : Master volume
- Pot 2 : Filter cutoff
- Pot 3 : Filter resonance
- Pot 4 : Amp Attack
- Pot 5 : Amp Decay
- Pot 6 : Amp Sustain
- Pot 7 : Amp Release

Contrôle contextuel :

- Pot 8 : paramètre principal de la page courante

Encodeur :

- navigation dans les pages
- sélection des paramètres de menu
- réglage des paramètres à choix discret ou de précision

Boutons :

- `Page -`
- `Page +`
- `Shift / Back`
- `Preset / Save / Action`

### 4.3 Philosophie UI hardware

La face avant V1 doit privilégier :

- la lisibilité
- la rapidité de jeu
- les paramètres essentiels toujours accessibles
- un menu simple pour les paramètres moins fréquents

## 5. Budget de signaux

Ce budget sert à vérifier que l'architecture matérielle tient sans multiplexeur obligatoire.

### 5.1 Entrées analogiques

Besoin actuel :

- `8` entrées ADC pour les potentiomètres

Décision :

- pas de multiplexeur requis à ce stade

### 5.2 Entrées numériques

Besoin actuel :

- `2` signaux quadrature encodeur
- `1` bouton encodeur
- `4` boutons de face avant

Total minimal :

- `7` entrées / GPIO numériques

### 5.3 Communication et interfaces

Besoin actuel :

- `1` RX UART pour MIDI DIN
- `1` bus OLED
- `1` liaison USB pour développement
- `2` sorties audio analogiques

## 6. Bloc MIDI DIN

### 6.1 Rôle

Ce bloc reçoit le MIDI entrant depuis un clavier, séquenceur ou autre contrôleur externe.

### 6.2 Exigences V1

- connecteur `DIN 5 broches`
- entrée `opto-isolée`
- liaison vers l'UART de la Daisy
- robustesse et comportement stable

### 6.3 Contenu attendu du bloc

Au minimum :

- connecteur DIN 5
- optocoupleur
- résistances du circuit MIDI
- liaison RX vers la Daisy

### 6.4 Référence de conception

Ce bloc devra suivre une topologie MIDI In standard.

À figer au schéma :

- référence exacte de l'optocoupleur
- valeurs exactes des résistances
- filtrage ou protection complémentaire éventuels
- broche Daisy retenue pour l'UART MIDI

## 7. Bloc sortie audio

### 7.1 Rôle

Ce bloc fournit la sortie audio finale du synthé.

### 7.2 Exigences V1

- sortie ligne stéréo
- niveau maîtrisé
- faible bruit
- comportement stable

### 7.3 Contenu attendu du bloc

Au minimum :

- liaison depuis les sorties audio de la Daisy
- étage de sortie simple si nécessaire
- jack TRS 6,35 mm

### 7.4 À figer

- topologie exacte de l'étage de sortie
- niveau cible de sortie
- éventuel filtrage analogique complémentaire
- stratégie de masse et blindage local

## 8. Bloc interface utilisateur

### 8.1 Potentiomètres

Exigences :

- `10k` linéaires recommandés
- réponse stable
- montage cohérent avec une future façade

À figer :

- format mécanique
- type d'axe
- fixation PCB ou panneau

### 8.2 Encodeur

Exigences :

- encodeur incrémental
- bouton poussoir intégré
- sensation lisible et suffisamment ferme

À figer :

- nombre d'impulsions
- type de filetage
- empreinte mécanique

### 8.3 Boutons

Exigences :

- 4 boutons momentané NO
- sensation claire
- lecture fiable

À figer :

- format
- type de montage
- hauteur compatible avec la future façade

### 8.4 Écran OLED

Exigences :

- `128x64`
- très lisible
- pilotable simplement

À figer :

- `I2C` ou `SPI`
- référence exacte du module
- implantation mécanique

## 9. Alimentation et stratégie de bruit

### 9.1 Situation prototype

Au prototype :

- l'USB peut servir au développement
- l'instrument doit rester pensé comme un appareil autonome côté comportement

### 9.2 Exigences générales

- masse soignée
- séparation logique entre zones audio et numérique autant que possible
- découplage correct
- attention particulière au bruit ramené par l'USB

### 9.3 Décision V1

- ne pas faire dépendre la qualité de jeu de la présence de l'USB
- privilégier le MIDI DIN pour l'usage musical normal

## 10. Brochage de travail

Le brochage détaillé n'est pas encore figé dans ce document.

En revanche, les familles de signaux sont déjà verrouillées :

- `ADC x8` pour les potentiomètres
- `GPIO x7 minimum` pour encodeur + switch + boutons
- `UART RX x1` pour le MIDI DIN
- `bus OLED x1`
- `audio out stéréo x2`

À faire avant schéma final :

- attribuer les broches Daisy exactes
- vérifier les conflits entre ADC, UART, OLED et GPIO
- documenter ce brochage dans une version suivante de ce document

## 11. Prototype matériel visé

Ordre de mise en œuvre recommandé :

1. Daisy Seed + USB + audio de base
2. MIDI DIN In
3. OLED
4. encodeur et boutons
5. 8 potentiomètres
6. intégration mécanique progressive

## 12. Points ouverts à figer

Avant de considérer la base hardware comme totalement verrouillée, il reste à figer :

- référence exacte de l'optocoupleur MIDI
- type exact d'écran OLED
- brochage Daisy détaillé
- topologie exacte de la sortie audio
- stratégie d'alimentation finale
- conventions mécaniques des potentiomètres, boutons et encodeur

## 13. Critère de solidité documentaire hardware

On considérera la base hardware suffisamment documentée pour passer sereinement à la suite quand :

- les blocs MIDI, audio, UI et alimentation sont décrits
- la BOM couvre le prototype réel
- le rôle de l'USB et du MIDI DIN est clairement séparé
- la surface de contrôle est figée
- les points non figés sont explicitement listés

## 14. Règle de maintenance

Si le schéma final, la BOM ou les choix mécaniques changent, ce document doit être mis à jour.

Ce document n'est pas décoratif.

Il fait partie de la source de vérité du projet.
