# BOM Finale Fantôme IV

Cette BOM est désormais la référence d'achat du prototype V1.

Elle correspond au point où le projet est arrivé :

- le socle logiciel/firmware portable est suffisamment mûr
- la route matérielle est figée
- l'achat du matériel peut démarrer sans ouvrir de piste parallèle

Date de vérification des liens de référence :

- `21 avril 2026`

Important :

- les stocks et prix peuvent bouger
- les références critiques ci-dessous sont figées
- les consommables standards peuvent rester génériques

## Ce qu'on achète maintenant

Le prototype réel retenu repose sur :

- `1 Daisy Seed 65MB`
- `8 potentiomètres`
- `1 encodeur rotatif cliquable cranté`
- `4 boutons poussoirs de panneau`
- `1 écran OLED 128x64 1,3" I2C`
- `1 LED MIDI`
- `1 entrée MIDI DIN`
- `1 sortie audio TRS stéréo`

## BOM Référencée - Achat principal

| Catégorie | Élément | Référence retenue | Qté projet | Qté achat | Lien de référence | Statut | Note |
|---|---|---|---:|---:|---|---|---|
| Carte principale | DSP audio | `Daisy Seed 65MB` | 1 | 1 | [Electrosmith Seed](https://electro-smith.com/products/daisy-seed) | Critique | carte principale |
| Carte principale | Option achat UE | `Daisy Seed 65MB` | 1 | 1 | [Signal Sounds EU](https://signalsounds.eu/electrosmith-daisy-seed-development-board-65mb/) | Référence achat | utile si la boutique officielle est en rupture |
| Câble | USB données | `micro-USB` | 1 | 1 | [Electrosmith Micro USB cable](https://electro-smith.com/products/micro-usb-cable) | Indispensable | câble de données, pas charge seule |
| Proto | Breadboard | `BusBoard BB830` | 1 | 2 | [BusBoard BB830](https://www.digikey.com/en/products/detail/busboard-prototype-systems/BB830/19200392) | Indispensable | 2 plaques rendent le proto plus confortable |
| Proto | Plaque à pastilles | `BusBoard SB830` | 0 | 1 | [BusBoard SB830](https://www.digikey.com/en/products/detail/busboard-prototype-systems/SB830/19200393) | Recommandé | pour fiabiliser après la breadboard |
| Proto | Fils Dupont | `26 AWG`, M/M et M/F | 1 lot | 1 lot | type générique | Indispensable | consommable standard, pas de référence figée |
| Interface | Potentiomètres | `Alpha RD901F-40-15F-B10K-00DL1` | 8 | 10 | [Mouser Alpha 10k](https://www.mouser.es/ProductDetail/Alpha-Taiwan/RD901F-40-15F-B10K-00DL1?qs=1mbolxNpo8eEwO1hUJUOSQ%3D%3D) | Critique | `10k` linéaire, axe plat `6 mm`, longueur `15 mm` |
| Interface | Encodeur | `Bourns PEC11R-4225F-S0024` | 1 | 2 | [DigiKey PEC11R](https://www.digikey.fr/fr/products/detail/bourns-inc/PEC11R-4225F-S0024/4499659) | Critique | encodeur cranté avec switch |
| Interface | Boutons de façade | `APEM IPR3FAD7/1` | 4 | 6 | [DigiKey IPR3FAD7/1](https://www.digikey.com/en/products/detail/apem-inc/IPR3FAD7-1/1280196) | Critique | poussoirs momentané `NO`, montage panneau |
| Interface | Écran OLED | `ER-OLEDM013-1W-I2C` | 1 | 2 | [BuyDisplay OLED 1,3 I2C](https://www.buydisplay.com/i2c-white-1-3-inch-oled-display-module-128x64-arduino-raspberry-pi) | Critique | `SH1106`, `3,3 V`, `128x64` |
| Audio | Jack sortie | `Switchcraft 155` | 1 | 1 | [DigiKey Switchcraft 155](https://www.digikey.com/en/products/detail/switchcraft-inc/155/1290109) | Critique | jack `TRS 6,35 mm` stéréo de panneau |
| MIDI | Embase DIN 5 | `Switchcraft 57GB5FX` | 1 | 1 | [DigiKey 57GB5FX](https://www.digikey.com/en/products/detail/switchcraft-inc/57GB5FX/1832316) | Critique | entrée MIDI DIN de panneau |
| MIDI | Optocoupleur | `onsemi H11L1M` | 1 | 2 | [DigiKey H11L1M](https://www.digikey.com/en/products/detail/onsemi/H11L1M/284866) | Critique | opto logique pour le bloc MIDI In |
| MIDI | Diode | `onsemi 1N4148-T50R` | 1 | 10 | [DigiKey 1N4148-T50R](https://www.digikey.com/en/products/detail/onsemi/1N4148-T50R/978510) | Indispensable | protection inverse côté LED opto |
| Signalisation | LED MIDI | `Kingbright WP710A10ID` | 1 | 4 | [DigiKey WP710A10ID](https://www.digikey.com/en/products/detail/kingbright/WP710A10ID/2769809) | Critique | LED rouge diffuse `3 mm` |
| Debug | Sonde debug | `STLINK-V3MINIE` | 0 | 1 | [ST STLINK-V3MINIE](https://www.st.com/en/development-tools/stlink-v3minie.html) | Recommandé | pas obligatoire, très utile pour debug |

## Composants standards à prendre en même temps

Ces lignes ne demandent pas une référence unique stricte. Le plus important est le type électrique et mécanique.

| Catégorie | Élément | Spécification à acheter | Qté conseillée | Statut | Note |
|---|---|---|---:|---|---|
| Résistances | MIDI In | `220R`, traversant, `1/4W`, `5%` | 10 | Indispensable | boucle MIDI |
| Résistances | Pull-up MIDI | `1k`, traversant, `1/4W`, `5%` | 10 | Indispensable | sortie `H11L1M` |
| Résistances | LED MIDI | `1k`, traversant, `1/4W`, `5%` | 10 | Indispensable | limitation courant LED |
| Condensateurs | Découplage MIDI | `100nF`, céramique, traversant ou pas `2,54 mm` | 10 | Indispensable | près de l'opto |
| Consommables | Dupont | lot M/M + M/F | 1 lot | Indispensable | bench |
| Consommables | Fil souple | `24-26 AWG` | 1 lot | Recommandé | version proto soudée |
| Consommables | Entretoises / headers | `2,54 mm` | 1 lot | Recommandé | support Daisy |

## Hors achat immédiat

À ne pas acheter tout de suite :

- PCB custom
- boîtier final
- façade finale
- knobs définitifs
- alimentation autonome finale
- éventuel étage casque dédié

## Règle d'achat

Les références réellement figées pour Fantôme IV V1 sont :

- `Daisy Seed 65MB`
- `ER-OLEDM013-1W-I2C`
- `RD901F-40-15F-B10K-00DL1`
- `PEC11R-4225F-S0024`
- `IPR3FAD7/1`
- `Switchcraft 155`
- `Switchcraft 57GB5FX`
- `H11L1M`
- `WP710A10ID`

Si l'une de ces références change :

- [COMPONENT-REFERENCES.md](./COMPONENT-REFERENCES.md) doit être mise à jour
- [DECISIONS.md](./DECISIONS.md) doit consigner le changement si l'impact est structurant

## Ordre d'achat recommandé

1. `Daisy Seed 65MB`
2. `OLED`
3. `8 pots + 1 encodeur + 4 boutons + 1 LED`
4. `MIDI In` complet : DIN + opto + diode + résistances + condensateur
5. `jack audio`
6. `breadboard`, fils, headers, consommables
7. `ST-Link V3MINIE` si tu veux debugger proprement

## Documents liés

- [INSTALLATION-MATERIEL.md](./INSTALLATION-MATERIEL.md)
- [COMPONENT-REFERENCES.md](./COMPONENT-REFERENCES.md)
- [PINMAP-DAISY.md](./PINMAP-DAISY.md)
- [MIDI-IN-HARDWARE-SPEC.md](./MIDI-IN-HARDWARE-SPEC.md)
- [AUDIO-OUT-SPEC.md](./AUDIO-OUT-SPEC.md)
