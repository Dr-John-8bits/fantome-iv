# Références Exactes de Composants Fantôme IV

Ce document fige les références exactes recommandées pour le prototype V1.

## Rôle

Ce document sert à éviter les achats flous du type :

- "un encodeur quelconque"
- "un OLED 1,3 quelconque"
- "des boutons à peu près compatibles"

Il transforme les intentions mécaniques et électriques du projet en pièces réellement recherchables.

## Principe

Les références ci-dessous sont les références de travail retenues pour Fantôme IV V1.

Elles ne prétendent pas être les seules pièces possibles.

Elles sont retenues parce qu'elles sont :

- cohérentes avec la V1
- crédibles mécaniquement
- documentées
- adaptées à un prototype proche de la façade finale

## Références retenues

| Fonction | Référence retenue | Type | Pourquoi ce choix |
|---|---|---|---|
| Carte principale | `Electro-Smith Daisy Seed 65MB` | module DSP | plateforme cœur du projet |
| Écran OLED | `BuyDisplay / EastRising ER-OLEDM013-1W-I2C` | OLED `128x64`, `1,3"`, `I2C`, `SH1106`, `3,3 V` | module exact, lisible, documenté, facile à piloter |
| Potentiomètres | `Alpha (Taiwan) RD901F-40-15F-B10K-00DL1` | pot `10k` linéaire, `9 mm`, axe plat `6 mm`, longueur `15 mm` | cohérent avec une façade de synthé et compatible boutons de panneau |
| Encodeur | `Bourns PEC11R-4225F-S0024` | encodeur incrémental, cranté, push switch, `24 PPR`, axe plat `6 mm` | sensation nette, référence très standard, bonne doc |
| Boutons de façade | `APEM IPR3FAD7/1` | bouton poussoir momentané `NO`, montage panneau, actionneur rond | vrai composant de panneau, lisible et robuste |
| MIDI In DIN | `Switchcraft 57GB5FX` | DIN `5` femelle, panneau, œillets à souder | vraie embase de panneau MIDI crédible pour proto/final |
| Optocoupleur MIDI | `onsemi H11L1M` | optocoupleur logique à hystérésis | compatible `3,3 V`, bon choix pour MIDI In moderne |
| Jack audio | `Switchcraft 155` | jack `TRS 6,35 mm` stéréo de panneau | solide, standard, adapté à une sortie ligne stéréo |
| LED MIDI | `Kingbright WP710A10ID` | LED rouge diffuse `3 mm` traversante | simple, lisible, standard |

## Point important sur l'écran OLED

Le choix exact retenu pour la V1 est :

- `ER-OLEDM013-1W-I2C`

Ce module est basé sur :

- `SH1106`

Ce choix remplace donc l'ancienne hypothèse trop générique :

- `SSD1306 1,3" I2C`

Conclusion pratique :

- le projet doit désormais documenter l'écran comme un `OLED 128x64 1,3" I2C`
- et l'implémentation de référence doit viser `SH1106`

## Point important sur les potentiomètres

La référence retenue est en axe plat :

- plus simple pour des knobs compatibles `D-shaft`
- plus stable mécaniquement qu'un axe lisse au hasard

Spécification cible retenue pour les `8` potards :

- `10 kohms`
- linéaire
- axe `6 mm`
- longueur d'axe `15 mm`
- format panneau

## Point important sur l'encodeur

La référence retenue impose déjà :

- un encodeur cranté
- un bouton intégré
- une vraie géométrie de panneau

Cela évite d'acheter un encodeur "fluide" ou sans switch, qui serait contraire à l'UX retenue.

## Quantités recommandées

Pour achat prototype :

- `10 x` potentiomètres `Alpha RD901F-40-15F-B10K-00DL1`
- `2 x` encodeurs `Bourns PEC11R-4225F-S0024`
- `6 x` boutons `APEM IPR3FAD7/1`
- `1 à 2 x` écrans `ER-OLEDM013-1W-I2C`
- `1 x` jack `Switchcraft 155`
- `1 x` embase MIDI `Switchcraft 57GB5FX`
- `2 x` optocoupleurs `H11L1M`

## Alternatives acceptables si rupture

Les alternatives sont autorisées uniquement si elles respectent la même intention.

### OLED

Alternative acceptable :

- autre module `OLED 128x64 1,3"` en `I2C`, `3,3 V`, documenté, avec encombrement compatible

Attention :

- ne pas acheter un module aléatoire sans vérifier contrôleur, tension et empreinte

### Potentiomètres

Alternative acceptable :

- autre potentiomètre de panneau `10k linéaire`, axe `6 mm`, longueur voisine, format `9 mm` ou `16 mm` si la façade est revue

### Encodeur

Alternative acceptable :

- encodeur incrémental cranté, push switch, axe `6 mm`, vraie fixation panneau

### Boutons

Alternative acceptable :

- bouton poussoir momentané `NO`, montage panneau, diamètre et course cohérents avec une face avant lisible

## Règle projet

Si l'une de ces références change réellement :

- `BOM.md` doit être mise à jour
- `LISTE-COURSES.md` doit être mise à jour
- `DECISIONS.md` doit consigner le changement si l'impact est structurant
