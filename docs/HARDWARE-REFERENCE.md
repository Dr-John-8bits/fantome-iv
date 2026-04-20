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
- désormais beaucoup plus précis sur le brochage, le MIDI In, la sortie audio et les références critiques

Les points non figés sont marqués `à figer`.

## Documents liés

- [Fantome-IV-specs.md](./Fantome-IV-specs.md)
- [DOCUMENTATION.md](./DOCUMENTATION.md)
- [DECISIONS.md](./DECISIONS.md)
- [ALIMENTATION-POWER-SPEC.md](./ALIMENTATION-POWER-SPEC.md)
- [MECHANICAL-SPEC.md](./MECHANICAL-SPEC.md)
- [PINMAP-DAISY.md](./PINMAP-DAISY.md)
- [MIDI-IN-HARDWARE-SPEC.md](./MIDI-IN-HARDWARE-SPEC.md)
- [AUDIO-OUT-SPEC.md](./AUDIO-OUT-SPEC.md)
- [COMPONENT-REFERENCES.md](./COMPONENT-REFERENCES.md)
- [BOM.md](./BOM.md)
- [ROADMAP.md](./ROADMAP.md)

## 1. Architecture matérielle V1 retenue

Fantôme IV V1 repose sur :

- `1 Daisy Seed 65MB`
- `1 entrée MIDI DIN 5 broches`
- `1 sortie ligne stéréo sur jack TRS 6,35 mm`
- `1 écran OLED 128x64` `1,3"` en `I2C`
- `8 potentiomètres` de panneau
- `1 encodeur rotatif cliquable cranté`
- `4 boutons poussoirs momentané NO`
- `1 LED MIDI`

Principe général :

- l'USB sert au développement, au flash et au debug
- le jeu musical normal passe par l'entrée MIDI DIN
- la sortie audio V1 est une sortie ligne stéréo simple
- la face avant vise un prototype proche de la façade finale
- la lisibilité passe avant la compacité extrême

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
    +-- 4 boutons poussoirs -> GPIO
    +-- OLED 128x64 1,3" -> I2C
    +-- LED MIDI -> GPIO

Daisy Seed 65MB
    |
    +-- audio stéréo ligne -> jack TRS 6,35 mm
```

## 3. Interfaces physiques retenues

### 3.1 MIDI

Interface musicale principale :

- `MIDI In DIN 5 broches`

Exigences :

- entrée `opto-isolée`
- fonctionnement identique avec ou sans USB branché
- pas de dépendance à l'USB pour jouer l'instrument
- indicateur visuel d'activité MIDI souhaité en façade

Décision :

- pas d'USB MIDI requis pour la V1
- pas de MIDI Out requis pour la V1
- `1 LED MIDI` de signalisation retenue

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
- alimentation du prototype
- debug

Décision :

- l'USB n'est pas l'interface musicale du produit
- l'USB est la source d'alimentation retenue pour le prototype initial

## 4. Surface de contrôle V1

### 4.1 Contrôles retenus

- `8 potentiomètres`
- `1 encodeur rotatif cliquable cranté`
- `4 boutons poussoirs momentané NO`
- `1 OLED 128x64` `1,3"` en `I2C`
- `1 LED MIDI`

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

Note :

- ces `4` boutons sont distincts du bouton poussoir intégré à l'encodeur

### 4.3 Philosophie UI hardware

La face avant V1 doit privilégier :

- la lisibilité
- la rapidité de jeu
- les paramètres essentiels toujours accessibles
- un menu simple pour les paramètres moins fréquents
- une implantation proche de la façade finale

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
- `1` LED MIDI

Total minimal :

- `8` entrées / GPIO numériques

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
- optocoupleur `H11L1M`
- résistance de boucle `220R`
- pull-up sortie optocoupleur `1k`
- diode inverse `1N4148`
- découplage local de l'optocoupleur
- sortie GPIO vers LED MIDI
- liaison RX vers la Daisy

### 6.4 Référence de conception

Ce bloc devra suivre une topologie MIDI In standard.

Référence prototype retenue :

- connecteur `Switchcraft 57GB5FX`
- optocoupleur `onsemi H11L1M`
- résistance de boucle `220R`
- pull-up sortie opto `1k`
- diode `1N4148`
- condensateur de découplage local `100nF`
- LED MIDI de signalisation en façade avec résistance série `1k`

À figer au schéma :

- option éventuelle de couplage `DIN pin 2 -> châssis`

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
- jack TRS 6,35 mm

### 7.4 Topologie retenue

Pour la V1 :

- `AUDIO OUT L` Daisy vers `tip`
- `AUDIO OUT R` Daisy vers `ring`
- `AGND` Daisy vers `sleeve`
- aucun étage actif externe ajouté
- aucun reconditionnement analogique dupliqué hors Seed

## 8. Bloc interface utilisateur

### 8.1 Potentiomètres

Exigences :

- `10k` linéaires recommandés
- réponse stable
- montage cohérent avec une future façade
- format de panneau privilégié pour le prototype

Référence retenue :

- `Alpha RD901F-40-15F-B10K-00DL1`
- axe plat `6 mm`
- longueur d'axe `15 mm`

### 8.2 Encodeur

Exigences :

- encodeur incrémental
- bouton poussoir intégré
- sensation lisible et suffisamment ferme
- encodeur cranté privilégié
- format de panneau privilégié

Référence retenue :

- `Bourns PEC11R-4225F-S0024`
- `24 PPR`
- encodeur de panneau à axe plat `6 mm`

### 8.3 Boutons

Exigences :

- 4 boutons momentané NO
- sensation claire
- lecture fiable
- vrais boutons de panneau privilégiés

Référence retenue :

- `APEM IPR3FAD7/1`
- bouton momentané `NO`
- montage panneau

### 8.4 Écran OLED

Exigences :

- `128x64`
- format `1,3"` retenu
- très lisible
- pilotable simplement

Référence retenue :

- module `BuyDisplay ER-OLEDM013-1W-I2C`
- contrôleur `SH1106`

À figer :

- implantation mécanique

## 9. Alimentation et stratégie de bruit

Le détail de référence de ce sujet vit désormais dans [ALIMENTATION-POWER-SPEC.md](./ALIMENTATION-POWER-SPEC.md).

### 9.1 Situation prototype

Au prototype :

- l'USB peut servir au développement
- l'USB sert aussi à l'alimentation du prototype
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

Le brochage détaillé est désormais figé dans [PINMAP-DAISY.md](./PINMAP-DAISY.md).

Familles de signaux retenues :

- `ADC x8` sur `A0` à `A7`
- `GPIO x8` sur `D0` à `D7`
- `UART RX x1` sur `D14`
- `I2C x1` sur `D11/D12`
- `audio out stéréo x2` sur `18/19`

## 11. Prototype matériel visé

Les règles d'implantation et de montage de référence vivent désormais dans [MECHANICAL-SPEC.md](./MECHANICAL-SPEC.md).

Ordre de mise en œuvre recommandé :

1. Daisy Seed + USB + audio de base
2. MIDI DIN In
3. OLED
4. encodeur et boutons
5. 8 potentiomètres
6. intégration mécanique progressive

## 12. Points ouverts à figer

Avant de considérer la base hardware comme totalement verrouillée, il reste à figer :

- stratégie d'alimentation finale
- support mécanique de la Daisy
- conventions de câblage finales si PCB custom

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
