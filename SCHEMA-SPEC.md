# Spec de Schéma Fantôme IV

Ce document décrit ce que le schéma électronique de Fantôme IV V1 doit contenir.

Il ne donne pas encore un schéma fini composant par composant, mais fixe :

- les blocs à dessiner
- les interconnexions obligatoires
- les exigences de conception
- les points à figer avant validation

## Rôle

Ce document sert à passer de la référence hardware à un vrai schéma exploitable.

Il complète :

- [Fantome-IV-specs.md](./Fantome-IV-specs.md)
- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [BOM.md](./BOM.md)

## Statut

Statut actuel :

- document de travail actif
- suffisant pour préparer la structure du schéma V1
- incomplet sur certaines références exactes et sur le brochage final de la Daisy

## 1. Objectif du schéma V1

Le schéma V1 doit permettre :

- de jouer Fantôme IV via `MIDI DIN 5 broches`
- d'afficher l'UI sur `OLED SSD1306 128x64 1,3"`
- de lire `8 potentiomètres`
- de lire `1 encodeur rotatif cliquable cranté`
- de lire `4 boutons poussoirs momentané NO`
- d'afficher une `LED MIDI`
- de sortir l'audio en `ligne stéréo TRS 6,35 mm`
- de programmer et débugger la Daisy via `micro-USB`

## 2. Blocs obligatoires du schéma

Le schéma V1 doit comporter au minimum les blocs suivants :

1. `Daisy Seed 65MB`
2. `alimentation et découplage`
3. `MIDI In DIN 5 broches`
4. `sortie audio stéréo`
5. `OLED SSD1306 128x64 1,3"`
6. `8 potentiomètres`
7. `1 encodeur + switch`
8. `4 boutons poussoirs momentané NO`
9. `LED MIDI`
10. `headers / points de debug / accès de programmation` si utile

## 3. Bloc Daisy Seed 65MB

### 3.1 Rôle

La Daisy est le cœur du synthé :

- DSP
- traitement MIDI
- lecture des contrôles
- pilotage de l'écran
- sortie audio

### 3.2 Le schéma doit montrer

- l'alimentation de la carte
- les masses utilisées
- les liaisons ADC
- les liaisons GPIO
- la liaison UART MIDI
- la liaison écran
- les sorties audio

### 3.3 À figer

- brochage exact de chaque fonction
- stratégie mécanique de montage
- nécessité ou non de headers amovibles

## 4. Bloc alimentation

### 4.1 Objectif

Garantir un fonctionnement stable et silencieux.

### 4.2 Le schéma doit expliciter

- l'arrivée d'alimentation du prototype
- les liaisons `3V3`, `3V3_A`, `AGND`, `DGND` si utilisées
- le découplage local nécessaire autour des sous-blocs
- la stratégie de masse générale

### 4.3 Exigences

- pas d'ambiguïté entre domaine audio et numérique
- éviter les retours de masse sales dans la sortie audio
- garder l'USB comme interface de développement, pas comme hypothèse d'usage musical

### 4.4 À figer

- mode d'alimentation final de la version non prototype
- éventuelles protections supplémentaires

## 5. Bloc MIDI In DIN

### 5.1 Fonction

Recevoir le MIDI externe comme interface musicale principale.

### 5.2 Le schéma doit contenir

- un connecteur `DIN 5 broches`
- un optocoupleur de type `H11L1` ou compatible
- les résistances du circuit MIDI In
- un condensateur de découplage local `100nF`
- un indicateur `LED MIDI`
- la liaison vers une entrée UART RX de la Daisy

### 5.3 Exigences

- isolation optique
- topologie MIDI In standard
- comportement stable même si l'USB est branché

### 5.4 À figer

- référence exacte de l'optocoupleur si une alternative au `H11L1` est retenue
- valeurs exactes des résistances
- valeur de résistance de la LED MIDI
- broche UART retenue

## 6. Bloc sortie audio

### 6.1 Fonction

Fournir la sortie ligne stéréo du synthé.

### 6.2 Le schéma doit contenir

- les deux sorties audio de la Daisy
- le conditionnement analogique nécessaire
- le connecteur `jack TRS 6,35 mm`

### 6.3 Exigences

- sortie ligne stéréo
- niveau cohérent
- faible bruit
- pas de comportement dépendant de la présence de l'USB

### 6.4 À figer

- topologie exacte de l'étage de sortie
- composants passifs du conditionnement
- niveau de sortie cible

## 7. Bloc OLED

### 7.1 Fonction

Afficher l'interface utilisateur de Fantôme IV.

### 7.2 Le schéma doit contenir

- le module `OLED 128x64`
- son contrôleur `SSD1306`
- son alimentation
- son bus de communication

### 7.3 Exigences

- écran très lisible
- câblage simple
- stabilité d'affichage

### 7.4 Décision actuelle

- `I2C` retenu pour simplifier le prototype, sauf blocage matériel majeur découvert plus tard
- format `1,3"` retenu pour privilégier la lisibilité

### 7.5 À figer

- référence exacte du module `1,3"`
- pull-up externes si nécessaires
- brochage Daisy retenu

## 8. Bloc potentiomètres

### 8.1 Fonction

Lire les `8` contrôles analogiques de la face avant.

### 8.2 Le schéma doit contenir

- `8 potentiomètres 10k linéaires`
- câblage vers `3V3_A` et masse
- les `8` wipers vers les ADC de la Daisy

### 8.3 Exigences

- plage stable `0 -> 3,3 V`
- lecture simple
- pas de multiplexeur obligatoire en V1
- format de panneau privilégié pour le prototype

### 8.4 Répartition de fonction

- Pot 1 : Master volume
- Pot 2 : Filter cutoff
- Pot 3 : Filter resonance
- Pot 4 : Amp Attack
- Pot 5 : Amp Decay
- Pot 6 : Amp Sustain
- Pot 7 : Amp Release
- Pot 8 : paramètre contextuel

## 9. Bloc encodeur

### 9.1 Fonction

Gérer la navigation et l'édition fine.

### 9.2 Le schéma doit contenir

- les deux sorties quadrature de l'encodeur
- le bouton poussoir intégré
- le câblage GPIO correspondant

### 9.3 Exigences

- lecture fiable
- sensation nette
- rebonds gérables proprement
- encodeur cranté privilégié
- format de panneau privilégié

### 9.4 À figer

- pull-up internes ou externes
- référence mécanique exacte

## 10. Bloc boutons

### 10.1 Fonction

Piloter la navigation et les actions de l'interface.

### 10.2 Le schéma doit contenir

- `4 boutons` momentané NO
- câblage GPIO
- liaison au `GND` si les pull-up internes de la Daisy sont retenus pour le prototype
- format de panneau privilégié

### 10.3 Fonctions retenues

- `Page -`
- `Page +`
- `Shift / Back`
- `Preset / Save / Action`

### 10.4 À figer

- validation finale des pull-up internes ou externes
- éventuel filtrage ou anti-rebond matériel
- référence exacte des boutons de panneau

## 11. Bloc LED MIDI

### 11.1 Fonction

Afficher l'activité MIDI en façade.

### 11.2 Le schéma doit contenir

- `1 LED MIDI`
- sa résistance série
- sa liaison GPIO

### 11.3 Exigences

- comportement simple et lisible
- utile pour debug et confirmation visuelle de réception MIDI

## 12. Budget de brochage

Le schéma final devra allouer au minimum :

- `8 ADC`
- `8 GPIO numériques` pour encodeur + switch + boutons + LED MIDI
- `1 RX UART`
- `1 bus écran`
- `2 sorties audio`

Le brochage exact doit être validé avant routage ou proto soudé.

## 13. Conventions de nommage recommandées

Pour garder un schéma lisible, on recommande les noms de nets suivants :

- `MIDI_IN_DIN`
- `MIDI_RX`
- `OLED_SCL`
- `OLED_SDA`
- `POT_1` à `POT_8`
- `ENC_A`
- `ENC_B`
- `ENC_SW`
- `BTN_PAGE_PREV`
- `BTN_PAGE_NEXT`
- `BTN_SHIFT`
- `BTN_ACTION`
- `AUDIO_L`
- `AUDIO_R`
- `LINE_OUT_L`
- `LINE_OUT_R`

## 14. Ce que le schéma ne doit pas contenir en V1

- `CV/Gate`
- `Audio In`
- `MIDI Out`
- `USB host MIDI`
- `sortie casque dédiée`
- `multiplexeur de potentiomètres` tant que le budget de pins tient

## 15. Checklist de validation du schéma

Avant de considérer le schéma comme acceptable, vérifier :

- le MIDI In est bien en DIN 5 et opto-isolé
- l'USB est clairement séparé de l'usage musical
- les 8 potentiomètres ont tous une entrée ADC dédiée
- l'encodeur et les boutons ont tous une lecture définie
- l'écran a une alimentation et un bus cohérents
- la sortie audio est bien stéréo ligne
- les masses et alimentations sont explicitement lisibles
- les points encore ouverts sont listés

## 16. Gate documentaire

Le passage à un vrai dessin de schéma détaillé est autorisé quand :

- ce document n'a plus de contradiction avec la spec fonctionnelle
- la référence hardware est cohérente
- la BOM supporte les blocs décrits ici
- les points à figer restants sont suffisamment réduits pour ne pas bloquer le dessin
