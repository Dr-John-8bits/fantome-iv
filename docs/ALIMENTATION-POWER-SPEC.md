# Spec Alimentation Fantôme IV

Ce document fige la stratégie d'alimentation et de masses de Fantôme IV V1.

## Rôle

Ce document répond à quatre questions :

- comment le prototype est alimenté
- quels rails alimentent quels sous-blocs
- comment gérer `AGND` et `DGND`
- quelles règles de câblage limiteront le bruit et les erreurs

Il ne fige pas encore l'alimentation du produit final autonome.

Il fige la stratégie prototype V1 et la discipline de câblage.

## Statut

Statut actuel :

- document de référence actif
- stratégie prototype V1 figée
- alimentation finale autonome encore ouverte

## 1. Données techniques Daisy à respecter

D'après la documentation officielle `Daisy Seed` :

- `VIN` accepte `+5 V` à `+17 V`
- `+3V3A` et `+3V3D` sont des sorties de la Seed
- `AGND` et `DGND` doivent être connectées dans l'application

Conséquence pour Fantôme IV :

- on n'injecte rien dans `+3V3A`
- on n'injecte rien dans `+3V3D`
- la Seed reste la source de `3,3 V` du prototype
- la gestion des masses doit être explicite

## 2. Stratégie d'alimentation retenue pour le prototype V1

La stratégie retenue est volontairement simple :

- alimentation unique par `micro-USB` sur la `Daisy Seed`
- aucun rail externe `5 V` séparé dans le prototype V1
- aucun convertisseur DC/DC additionnel dans le prototype V1
- aucun bloc d'alimentation externe imposé au démarrage

Ce choix est retenu parce qu'il :

- simplifie fortement le premier prototype
- réduit les risques d'erreur de câblage
- s'aligne avec la manière la plus simple de développer et flasher la Daisy

## 3. Architecture de puissance retenue

```text
USB 5V
  |
  +--> Daisy Seed
         |
         +--> +3V3A --> potentiomètres
         |
         +--> +3V3D --> OLED + MIDI In + encodeur + boutons + LED MIDI
         |
         +--> AGND ---> audio + bas des potentiomètres
         |
         +--> DGND ---> contrôles numériques + OLED + MIDI In
```

## 4. Répartition des rails

| Rail / référence | Usage Fantôme IV | Règle retenue |
|---|---|---|
| `USB 5V` | alimentation unique du prototype | entre par le `micro-USB` de la Daisy |
| `VIN` | non utilisé dans le prototype V1 | réservé pour une future alimentation autonome |
| `+3V3A` | `8` potentiomètres | rail analogique réservé aux lectures ADC |
| `+3V3D` | OLED, `H11L1M`, pull-up MIDI, LED MIDI, logique de façade | rail numérique du prototype |
| `AGND` | sortie audio, référence bas de potentiomètres | rail de retour analogique |
| `DGND` | OLED, boutons, encodeur, MIDI In logique | rail de retour numérique |

## 5. Règle critique AGND / DGND

La règle retenue pour Fantôme IV est :

- `AGND` et `DGND` doivent être reliées une seule fois, de manière explicite, au plus près de la Daisy

Cela implique :

- ne pas multiplier les ponts `AGND <-> DGND` à plusieurs endroits du proto
- faire revenir l'audio vers `AGND`
- faire revenir les sous-blocs numériques vers `DGND`
- créer un point de jonction unique sur la carte porteuse, la plaque à pastilles ou le proto

Cette règle est importante.

Le but n'est pas de "séparer magiquement" l'analogique et le numérique.

Le but est :

- d'éviter des boucles de retour sales
- de garder un chemin de masse lisible
- de maîtriser le bruit ramené vers la sortie audio

## 6. Règles de câblage par sous-bloc

### 6.1 Potentiomètres

Pour chaque potentiomètre :

- borne haute -> `+3V3A`
- borne basse -> `AGND`
- curseur -> ADC Daisy correspondant

Règles :

- éviter les liaisons inutilement longues
- garder les faisceaux analogiques séparés des faisceaux USB et OLED autant que possible

### 6.2 OLED

L'écran utilise :

- `+3V3D`
- `DGND`
- `I2C`

Règles :

- ne pas l'alimenter depuis `+3V3A`
- garder le faisceau I2C raisonnablement court
- si le câblage devient long ou instable, ajouter un petit réservoir local `10 uF` côté module sur la carte porteuse

### 6.3 MIDI In

Le bloc `MIDI In` utilise :

- `+3V3D`
- `DGND`
- `100 nF` local obligatoire au plus près du `H11L1M`

Règles :

- ne pas alimenter le `H11L1M` depuis `+3V3A`
- garder le retour logique du bloc sur `DGND`
- la `LED MIDI` reste un bloc de signalisation séparé

### 6.4 Boutons, encodeur, LED MIDI

Ces éléments restent dans le domaine numérique :

- `DGND`
- `+3V3D` si nécessaire

Règles :

- ne pas les référencer à `AGND`
- regrouper ces liaisons dans un faisceau logique simple

### 6.5 Audio

La sortie audio doit utiliser :

- `AUDIO OUT L`
- `AUDIO OUT R`
- `AGND`

Règles :

- ne pas faire revenir la sortie jack sur `DGND`
- garder ces liaisons aussi courtes que possible
- éloigner le trajet audio des liaisons USB, OLED et MIDI

## 7. Découplage retenu

Découplage explicitement retenu dans la V1 :

- `100 nF` obligatoire sur le `H11L1M`
- le découplage déjà intégré à la `Daisy Seed` est conservé comme base principale
- si la carte porteuse regroupe plusieurs fils numériques longs, un `10 uF` local sur le rail `+3V3D` de distribution est recommandé

Ce qui n'est pas exigé à ce stade :

- pas de réseau complexe de filtrage en amont
- pas de LDO externe additionnel juste par principe
- pas de DC/DC dédié pour le prototype initial

## 8. Discipline anti-bruit retenue

Pour le prototype V1 :

- l'USB reste acceptable comme alimentation de départ
- le `MIDI DIN` reste l'interface musicale normale
- l'USB ne doit pas devenir un chemin audio ou MIDI obligatoire

Règles pratiques :

- séparer autant que possible les faisceaux audio et numériques
- éviter de faire courir le câble audio le long du câble USB
- éviter de créer plusieurs points de masse entre audio et numérique
- si un bruit de masse apparaît pendant les écoutes, commencer par vérifier le routage, les retours de masse et la longueur des câbles avant d'ajouter des filtres au hasard

## 9. Ce qui n'est pas retenu en V1 prototype

- pas d'alimentation externe obligatoire
- pas de jack d'alimentation décidé à ce stade
- pas de batterie
- pas de bloc casque dédié
- pas de distribution `5 V` séparée pour des périphériques

## 10. Direction recommandée pour une future version autonome

Ce point n'est pas encore figé, mais la direction saine est :

- entrée d'alimentation externe dédiée
- décision explicite entre `VIN` Daisy et une architecture régulée dédiée
- stratégie de châssis et de masse revue à ce moment-là

Ce futur choix devra être documenté avant toute version non-USB.

## 11. Checklist de validation

La stratégie d'alimentation prototype sera considérée comme respectée si :

- la Seed est la seule source `3,3 V` du proto
- `AGND` et `DGND` sont reliées une seule fois et explicitement
- les potentiomètres utilisent `+3V3A` et `AGND`
- OLED, MIDI In et logique de façade utilisent `+3V3D` et `DGND`
- le `H11L1M` a bien son `100 nF` local
- la sortie audio utilise bien `AGND`

## 12. Documents liés

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [PINMAP-DAISY.md](./PINMAP-DAISY.md)
- [MIDI-IN-HARDWARE-SPEC.md](./MIDI-IN-HARDWARE-SPEC.md)
- [AUDIO-OUT-SPEC.md](./AUDIO-OUT-SPEC.md)
