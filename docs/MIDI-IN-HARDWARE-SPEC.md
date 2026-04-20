# Spec Hardware MIDI In Fantôme IV

Ce document fige le bloc `MIDI In DIN 5 broches` de Fantôme IV V1.

## Rôle

Ce bloc doit permettre :

- de jouer Fantôme IV depuis un clavier MIDI
- de recevoir `Note On/Off`, `Pitch Bend`, `CC`, `Program Change` et `MIDI Clock`
- de rester pleinement fonctionnel sans dépendre de l'USB

## Référence normative

Le point de départ est la mise à jour `CA-033` de la spécification électrique MIDI.

Points normatifs utiles :

- le récepteur garde une résistance série `220 ohms`
- une diode de protection inverse de type `1N4148` est recommandée
- la broche `2` du `DIN In` ne doit pas être reliée directement à la masse signal
- un petit couplage capacitif de la broche `2` vers le châssis est optionnel pour l'EMI

## Décision retenue pour Fantôme IV V1

Pour Fantôme IV, le bloc retenu est :

- `DIN 5` femelle de panneau
- optocoupleur `H11L1M`
- alimentation logique du bloc en `3V3_D`
- résistance MIDI côté boucle : `220 ohms`
- diode inverse : `1N4148`
- condensateur de découplage local : `100 nF`
- pull-up de sortie optocoupleur : `1 kohm` vers `3V3_D`
- sortie vers `USART1_RX` de la Daisy sur `D14`
- `LED MIDI` pilotée séparément par firmware, avec une résistance série `1 kohm`
- la `LED MIDI` n'est pas directement placée sur la boucle MIDI

## Topologie retenue

```text
DIN pin 4 ---------------------------> anode LED H11L1
DIN pin 5 ---- 220R -----------------> cathode LED H11L1

1N4148 en antiparallèle sur la LED d'entrée du H11L1

DIN pin 2 -> non connecté en prototype
           -> option future : 100nF vers châssis si boîtier métallique et stratégie EMI définie

H11L1M VCC -> +3V3_D
H11L1M GND -> DGND
100nF entre VCC et GND au plus près du H11L1M

H11L1M OUT -> MIDI_RX -> Daisy D14 / USART1_RX
MIDI_RX -> 1k pull-up -> +3V3_D
```

## Valeurs finales retenues

| Élément | Valeur / référence | Statut |
|---|---|---|
| Résistance série boucle MIDI | `220 ohms`, `5 %`, `0,25 W` | figé |
| Diode inverse | `1N4148` | figé |
| Optocoupleur | `H11L1M` | figé |
| Pull-up sortie opto | `1 kohm` vers `3V3_D` | figé |
| Découplage local | `100 nF` céramique `X7R` | figé |
| Broche Daisy | `D14` / `USART1_RX` | figé |

## Pourquoi `H11L1M`

Le `H11L1M` est retenu parce que :

- c'est un optocoupleur à sortie logique avec hystérésis
- il est compatible avec une alimentation logique `3,3 V`
- il est couramment utilisé dans des récepteurs MIDI modernes à faible tension
- il évite la dépendance à un rail `5 V` côté réception

Le choix `H11L1M` est donc volontaire.

On n'utilise pas `6N138` comme référence V1, car ce composant pousse souvent vers un environnement `5 V`, moins cohérent avec la Daisy seule.

## Pourquoi un pull-up `1 kohm`

La valeur `1 kohm` n'est pas une exigence directement imposée par `CA-033`.

C'est un choix d'implémentation retenu pour Fantôme IV :

- suffisamment fort pour donner des fronts nets à `31,25 kbaud`
- suffisamment modéré pour rester simple et robuste à `3,3 V`
- cohérent avec une sortie `open collector` logique comme celle du `H11L1M`

Cette valeur doit donc être lue comme :

- un choix d'ingénierie figé pour Fantôme IV
- et non comme une valeur universelle imposée par la norme

## Brochage logique retenu

Le net de sortie du bloc s'appelle :

- `MIDI_RX`

La pin Daisy associée est :

- `D14`
- fonction `USART1_RX`

Le bloc est pensé pour fournir directement un signal exploitable par l'UART, sans inversion logicielle additionnelle.

## Broche 2 du connecteur DIN

Pour la V1 prototype :

- `DIN pin 2` reste non connectée à la masse signal

Pour une version plus avancée avec boîtier métallique :

- un condensateur `100 nF` vers châssis peut être envisagé
- cette option reste hors du prototype V1 actuel tant que la stratégie châssis n'est pas figée

## Ce qui n'est pas retenu en V1

- pas de `MIDI Out`
- pas de `MIDI Thru`
- pas de ferrites obligatoires dans le prototype
- pas d'alimentation `5 V` dédiée juste pour le MIDI In
- pas de LED directement dans la boucle MIDI

## Références de composants du bloc

- connecteur MIDI In : `Switchcraft 57GB5FX`
- optocoupleur : `onsemi H11L1M`
- diode : `1N4148`

Les références détaillées vivent aussi dans [COMPONENT-REFERENCES.md](./COMPONENT-REFERENCES.md).

## Validation attendue au schéma

Le schéma final V1 sera considéré conforme si :

- la boucle d'entrée suit bien la topologie retenue
- la résistance `220R` est bien côté entrée MIDI
- la diode inverse `1N4148` est bien présente
- le `H11L1M` est bien alimenté en `3V3_D`
- le `100 nF` est bien au plus près de l'opto
- la sortie opto va bien sur `D14`
- la `LED MIDI` reste un bloc séparé piloté par firmware
