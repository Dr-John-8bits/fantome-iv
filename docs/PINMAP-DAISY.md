# Pin Map Daisy Fantôme IV

Ce document fige le brochage de travail de la `Daisy Seed 65MB` pour Fantôme IV V1.

Il sert de référence commune entre :

- la documentation hardware
- le futur schéma
- la BOM
- le firmware

## Rôle

Ce document répond à une question simple :

- quelle broche Daisy fait quoi dans Fantôme IV

La pin map retenue privilégie :

- `8 ADC` dédiées, sans multiplexeur
- `I2C` simple pour l'écran
- `UART` dédié pour le `MIDI In`
- des `GPIO` numériques clairs pour l'encodeur, les boutons et la LED MIDI
- quelques broches laissées libres pour la suite

## Sources techniques de base

Cette pin map est construite à partir :

- du pinout officiel `Daisy Seed`
- des fonctions alternatives documentées dans la datasheet officielle
- des besoins V1 de Fantôme IV

## Principes retenus

### 1. ADC

Les `8` potentiomètres utilisent `A0` à `A7`.

Cela permet :

- un câblage simple
- une lecture analogique directe
- aucune dépendance à un multiplexeur en V1

### 2. OLED

L'écran utilise `I2C1` sur `D11` et `D12`.

Ce choix libère `D13` / `D14` pour l'UART `USART1`, utile côté MIDI.

### 3. MIDI

Le `MIDI In DIN` entre sur `D14`, utilisé en `USART1_RX`.

`D13` reste réservé en `USART1_TX` pour un éventuel `MIDI Thru`, debug série ou extension ultérieure.

### 4. Contrôles numériques

L'encodeur, son bouton, les `4` boutons de façade et la `LED MIDI` utilisent `D0` à `D7`.

Ce bloc est lisible, groupé, et évite de disperser inutilement les signaux.

## Pin map retenue

| Daisy pin | Nom Daisy | STM32 | Fonction Fantôme IV | Type | Note |
|---|---|---|---|---|---|
| 1 | `D0` | `PB12` | `ENC_A` | GPIO input | encodeur quadrature A |
| 2 | `D1` | `PC11` | `ENC_B` | GPIO input | encodeur quadrature B |
| 3 | `D2` | `PC10` | `ENC_SW` | GPIO input | bouton intégré à l'encodeur |
| 4 | `D3` | `PC9` | `BTN_PAGE_PREV` | GPIO input | bouton `Page -` |
| 5 | `D4` | `PC8` | `BTN_PAGE_NEXT` | GPIO input | bouton `Page +` |
| 6 | `D5` | `PD2` | `BTN_SHIFT` | GPIO input | bouton `Shift / Back` |
| 7 | `D6` | `PC12` | `BTN_ACTION` | GPIO input | bouton `Preset / Save / Action` |
| 8 | `D7` | `PG10` | `LED_MIDI` | GPIO output | activité MIDI pilotée par firmware |
| 12 | `D11` | `PB8` | `OLED_SCL` | I2C | `I2C1_SCL` |
| 13 | `D12` | `PB9` | `OLED_SDA` | I2C | `I2C1_SDA` |
| 14 | `D13` | `PB6` | `UART1_TX_RESERVED` | reserved | extension future |
| 15 | `D14` | `PB7` | `MIDI_RX` | UART RX | `USART1_RX` |
| 18 | `AUDIO OUT L` | n/a | `LINE_OUT_L` | analog out | sortie ligne gauche Daisy |
| 19 | `AUDIO OUT R` | n/a | `LINE_OUT_R` | analog out | sortie ligne droite Daisy |
| 20 | `AGND` | n/a | `AUDIO_GND` | ground | retour audio analogique |
| 21 | `+3V3A` | n/a | `POT_3V3A` | power | alimentation des potentiomètres |
| 22 | `A0 / D15` | `PC0` | `POT_1_MASTER` | ADC | Master volume |
| 23 | `A1 / D16` | `PA3` | `POT_2_CUTOFF` | ADC | Filter cutoff |
| 24 | `A2 / D17` | `PB1` | `POT_3_RESO` | ADC | Filter resonance |
| 25 | `A3 / D18` | `PA7` | `POT_4_AMP_A` | ADC | Amp attack |
| 26 | `A4 / D19` | `PA6` | `POT_5_AMP_D` | ADC | Amp decay |
| 27 | `A5 / D20` | `PC1` | `POT_6_AMP_S` | ADC | Amp sustain |
| 28 | `A6 / D21` | `PC4` | `POT_7_AMP_R` | ADC | Amp release |
| 29 | `A7 / D22` | `PA5` | `POT_8_CONTEXT` | ADC | potard contextuel |
| 38 | `+3V3D` | n/a | `DIGITAL_3V3` | power | OLED, MIDI In, LED, entrées numériques |
| 40 | `GND` | n/a | `DIGITAL_GND` | ground | masse numérique |

## Broches explicitement laissées libres

Broches non utilisées en V1, mais réservées :

- `16` et `17` : `AUDIO IN L/R`
- `30` à `35` : futures extensions analogiques ou numériques
- `39` : `VIN`, non utilisé dans le prototype USB

Ces broches ne doivent pas être allouées légèrement tant que le schéma V1 n'est pas dessiné.

## Conventions de câblage

### Potentiomètres

- haut de piste : `+3V3A`
- bas de piste : `AGND`
- curseur : ADC Daisy correspondant

### Boutons et encodeur

- logique active bas recommandée
- bouton vers `GND`
- pull-up internes Daisy autorisés pour le prototype

### OLED

- alimentation depuis `+3V3D`
- bus `I2C1` sur `D11/D12`

### MIDI In

- optocoupleur alimenté depuis `+3V3D`
- sortie optocoupleur vers `D14`

### Audio

- `pin 18` vers `tip`
- `pin 19` vers `ring`
- `pin 20 AGND` vers `sleeve`

## Pourquoi cette pin map

Cette pin map a été retenue parce qu'elle :

- respecte tous les besoins V1
- évite les conflits entre `ADC`, `I2C` et `UART`
- garde une lecture simple pour le schéma et le firmware
- laisse un peu de marge pour une extension future

## Points encore ouverts malgré cette pin map

La pin map est figée pour la V1 documentaire.

Les sujets encore ouverts ne sont plus le choix des broches, mais :

- la forme exacte du support mécanique de la Daisy
- le niveau de finition du câblage prototype
- la stratégie de routage si un PCB custom est dessiné plus tard
