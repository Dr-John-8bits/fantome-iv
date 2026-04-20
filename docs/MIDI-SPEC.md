# Spec MIDI Fantôme IV

Ce document définit le comportement MIDI de Fantôme IV V1.

Il décrit :

- les messages MIDI reçus
- le comportement attendu du synthé face à ces messages
- la table de `CC`
- le comportement des `Program Change`
- la logique de `MIDI Clock`

## Rôle

Ce document sert à figer le contrat MIDI de la V1.

Il complète :

- [Fantome-IV-specs.md](./Fantome-IV-specs.md)
- [UI-UX-SPEC.md](./UI-UX-SPEC.md)
- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [DECISIONS.md](./DECISIONS.md)

## Statut

Statut actuel :

- document de référence actif
- suffisamment précis pour guider la V1
- quelques détails secondaires pourront encore être enrichis plus tard

## 1. Principes MIDI V1

Fantôme IV V1 doit être pleinement jouable via :

- `MIDI In DIN 5 broches`

L'USB n'est pas l'interface musicale principale.

Le contrat MIDI V1 privilégie :

- la fiabilité
- la simplicité
- un petit nombre de `CC` stables et utiles
- un support propre des `Program Change`
- une synchro `MIDI Clock` claire

## 2. Canal MIDI

### 2.1 Règle générale

Le synthé reçoit sur un seul canal MIDI sélectionnable par l'utilisateur.

### 2.2 Valeurs

- canaux supportés : `1` à `16`
- valeur par défaut recommandée au démarrage : `1`

### 2.3 Hors scope V1

- mode Omni
- réception multicanale
- multitimbralité

## 3. Messages MIDI reçus

### 3.1 Obligatoires en V1

- `Note On`
- `Note Off`
- `Velocity`
- `Pitch Bend`
- `Control Change`
- `Program Change`
- `Mod Wheel`
- `Sustain Pedal`
- `MIDI Clock`
- `Start`
- `Stop`
- `Continue`
- `All Notes Off`
- `Panic` si disponible via CC standard ou action interne

### 3.2 Règles de parsing

- `Note On` avec vélocité `0` est traité comme `Note Off`
- les messages reçus sur un autre canal sont ignorés
- les messages non supportés sont ignorés proprement

## 4. Jeu clavier

### 4.1 Notes

Le clavier MIDI doit permettre de :

- jouer les voix du synthé normalement
- profiter des modes `Poly`, `Mono`, `Unison`, `Legato`

### 4.2 Velocity

Décision V1 :

- la vélocité agit sur l'amplitude du `VCA`

Hors scope V1 par défaut :

- matrice complexe de vélocité
- routage libre de vélocité

## 5. Pitch Bend et contrôles de performance

### 5.1 Pitch Bend

Décision V1 :

- plage de `Pitch Bend` fixée à `±2 demi-tons`

### 5.2 Mod Wheel

Décision V1 :

- `CC1` agit comme contrôle musical principal de performance
- en V1, il est recommandé qu'il pilote une quantité de modulation de pitch de type vibrato

### 5.3 Sustain

Décision V1 :

- `CC64` est supporté
- comportement attendu de sustain classique

## 6. Program Change

### 6.1 Objectif

Permettre le rappel rapide des presets depuis un clavier ou séquenceur.

### 6.2 Capacité V1

- `4 presets utilisateur`

### 6.3 Mapping retenu

Fantôme IV V1 traite les `Program Change` ainsi :

- `PC 0` -> Preset 1
- `PC 1` -> Preset 2
- `PC 2` -> Preset 3
- `PC 3` -> Preset 4

### 6.4 Règle hors plage

- les `Program Change` hors `0..3` sont ignorés

### 6.5 Note sur l'affichage utilisateur

L'interface utilisateur peut afficher les presets comme `1..4`, même si le transport MIDI utilise `0..3`.

## 7. Control Change

### 7.1 Philosophie V1

La V1 doit supporter peu de `CC`, mais des `CC` utiles et stables.

Le but n'est pas d'exposer tout le moteur.

Le but est d'offrir un contrôle externe musical et fiable.

### 7.2 Table CC retenue

| CC | Fonction | Comportement V1 |
|---:|---|---|
| 1 | Mod Wheel | modulation de performance principale |
| 7 | Master Volume | volume global |
| 64 | Sustain Pedal | sustain |
| 71 | Filter Resonance | résonance du filtre |
| 74 | Filter Cutoff | cutoff du filtre |
| 91 | Reverb Mix | dosage de reverb |
| 93 | Chorus Depth | profondeur de chorus |
| 12 | Delay Feedback | feedback du delay |
| 13 | Delay Mix | mix du delay |

### 7.3 Règle générale

- les valeurs `CC` sont mappées sur `0..127`
- la transformation vers les paramètres internes est laissée au moteur, mais doit rester musicale et stable

### 7.4 CC non mappés

- les `CC` non documentés sont ignorés en V1

## 8. MIDI Clock

### 8.1 Éléments synchronisables

Le `MIDI Clock` peut piloter :

- `LFO oscillateurs`
- `LFO filtre`
- `Sample & Hold`
- `Delay`

### 8.2 Règle V1

Quand un élément est en mode `Clock` :

- il suit le tempo MIDI reçu
- sa vitesse est définie par une division musicale

### 8.3 Divisions lisibles attendues

Exemples de divisions V1 :

- `1/1`
- `1/2`
- `1/4`
- `1/8`
- `1/16`

La liste pourra être étendue, mais doit rester simple.

### 8.4 Absence de clock

Règle recommandée pour la V1 :

- si un paramètre est en mode `Clock` mais qu'aucune clock n'a encore été reçue depuis le démarrage, utiliser un tempo interne de secours de `120 BPM`
- si une clock a déjà été reçue puis disparaît, conserver le dernier tempo connu jusqu'à changement explicite ou nouvelle clock

Cette règle évite un comportement silencieux ou incompréhensible.

### 8.5 Start / Stop / Continue

Décision V1 :

- `Start`, `Stop` et `Continue` sont reçus
- ils servent au comportement de synchro temporelle
- ils ne doivent pas perturber la tenue des notes ou couper le son de manière inattendue

## 9. All Notes Off et Panic

### 9.1 Exigence

Le synthé doit pouvoir revenir à un état propre si le flux MIDI devient incohérent.

### 9.2 Comportement attendu

- support de `All Notes Off` si reçu
- support d'une action `Panic` interne via UI
- libération propre des voix et des états de sustain

## 10. Messages non requis en V1

Hors scope V1 :

- `Aftertouch`
- `Poly Aftertouch`
- `NRPN`
- `RPN`
- `SysEx`
- `MIDI Out`
- `USB host MIDI`

## 11. Comportement de robustesse

La couche MIDI V1 doit :

- ignorer proprement les messages non supportés
- ne pas planter en cas de flux anormal
- rester jouable même en présence de messages externes inutiles

## 12. Checklist de validation MIDI

Avant de considérer le comportement MIDI V1 comme suffisamment cadré, vérifier :

- le canal MIDI est sélectionnable
- les notes sont jouables depuis un clavier MIDI
- le `Pitch Bend` fonctionne en `±2`
- la `Mod Wheel` a un comportement musical
- le `Sustain` fonctionne
- les `CC` documentés agissent correctement
- les `Program Change` rappellent bien les `4 presets`
- les cibles en mode `Clock` suivent bien la clock MIDI
- le synthé reste jouable sans USB

## 13. Gate documentaire

On considère la couche MIDI suffisamment documentée pour la V1 quand :

- la table de messages supportés est figée
- les `CC` retenus sont figés
- les `Program Change` sont figés
- le comportement de clock est figé
- aucune ambiguïté majeure ne subsiste entre DIN, USB et UI
