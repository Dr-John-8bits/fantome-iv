# BOM Fantôme IV

Cette BOM est la nomenclature de travail de Fantôme IV V1.

Elle est orientée prototype réel autour de la route retenue :

- `Daisy Seed 65MB`
- `8 potentiomètres`
- `1 encodeur rotatif cliquable`
- `4 boutons poussoirs momentané NO`
- `1 écran OLED 128x64` de type `SSD1306`
- `MIDI In` physique
- sortie audio stéréo

## Hypothèses

- prototype câblé à la main sur breadboard ou plaque de prototypage
- développement firmware sur Mac via USB
- usage musical normal via MIDI DIN
- pas de PCB custom dans l'immédiat
- pas de façade définitive dans l'immédiat

## BOM Prototype - À acheter maintenant

| Catégorie | Élément | Spécification cible | Qté projet | Qté achat conseillée | Priorité | Note |
|---|---|---|---:|---:|---|---|
| Calcul | Daisy Seed | `Daisy Seed 65MB` | 1 | 1 | Indispensable | Cœur du projet |
| Connexion | Câble USB | `micro-USB` données | 1 | 1 | Indispensable | Sert au développement, au flash et au debug |
| Proto | Breadboard | format moyen ou grand | 1 | 1 à 2 | Indispensable | Plus confortable si deux plaques |
| Proto | Fils Dupont | mâle/mâle + mâle/femelle | 1 lot | 1 lot | Indispensable | Pour tout le câblage de bench |
| Interface | Potentiomètres | `10k` linéaires | 8 | 10 | Indispensable | 8 pour le projet, 2 de marge |
| Interface | Encodeur rotatif | incrémental + bouton poussoir | 1 | 2 | Indispensable | 1 pour le projet, 1 de secours |
| Interface | Boutons poussoirs | momentané NO | 4 | 6 | Indispensable | 4 boutons de face avant, hors bouton intégré à l'encodeur |
| Interface | Écran OLED | `128x64`, `SSD1306`, `I2C` | 1 | 1 à 2 | Indispensable | Module simple 4 broches recommandé pour démarrer |
| Audio | Sortie audio | jack TRS stéréo `6,35 mm` | 1 | 1 | Indispensable | Cible produit V1 |
| MIDI | Connecteur MIDI In | DIN 5 broches femelle | 1 | 1 | Indispensable | Interface musicale principale de la V1 |
| MIDI | Optocoupleur | `H11L1` ou compatible MIDI In | 1 | 2 | Indispensable | Référence de départ du prototype |
| MIDI | Résistances MIDI In | `220R`, `270R` | 2 | 2 à 4 | Indispensable | Valeurs de départ à valider au schéma final |
| MIDI | Condensateur MIDI In | `100nF` | 1 | 2 à 4 | Indispensable | Découplage local de l'optocoupleur |
| Composants | Résistances | assortiment de base | 1 lot | 1 lot | Indispensable | Pour MIDI, LED, pull-up, essais |
| Composants | Condensateurs | assortiment de base | 1 lot | 1 lot | Indispensable | Découplage et essais |
| Test | Casque ou monitoring | sortie ligne ou casque | 1 | 1 | Indispensable | Pour écouter le proto |
| Test | Clavier ou contrôleur MIDI | n'importe quel modèle fonctionnel | 1 | 1 | Indispensable | Pour valider le jeu |

## BOM Prototype - Recommandé mais non obligatoire

| Catégorie | Élément | Spécification cible | Qté projet | Qté achat conseillée | Priorité | Note |
|---|---|---|---:|---:|---|---|
| Debug | ST-Link | `ST-Link V3 Mini` | 1 | 1 | Utile | Pas obligatoire pour flasher, utile pour debug |
| Proto | Plaque à pastilles | proto soudé plus robuste | 1 | 1 | Utile | Pour éviter une breadboard trop fragile |
| Interface | Knobs | compatibles avec les axes choisis | 8 | 8 | Utile | Pas indispensable pour démarrer |
| Signalisation | LED | 3 mm ou 5 mm | 1 | 2 à 4 | Utile | Par exemple pour l'activité MIDI |

## BOM À figer au schéma final

Ces éléments existeront probablement, mais la référence exacte dépendra du schéma que l'on validera ensemble.

| Élément | Direction actuelle | Statut |
|---|---|---|
| Circuit MIDI In | DIN 5 + `H11L1` ou compatible + résistances + `100nF` | À figer |
| Étage de sortie audio | sortie stéréo propre, niveau maîtrisé | À figer |
| Alimentation finale | hors USB uniquement si nécessaire | À figer |
| Variante mécanique de l'OLED | `SSD1306 128x64 I2C` | À figer |
| Type exact de potentiomètres | encombrement, axe, fixation | À figer |
| Type exact d'encodeur | encombrement, filetage, sensation | À figer |
| Support mécanique de la Daisy | headers, entretoises, support | À figer |

## À acheter plus tard

| Élément | Quand | Note |
|---|---|---|
| PCB custom | plus tard | Une fois l'architecture validée |
| Façade définitive | plus tard | Une fois l'interface figée |
| Boîtier | plus tard | À choisir après validation mécanique |
| Knobs définitifs | plus tard | À prendre quand les axes sont figés |
| Alimentation définitive | plus tard | Selon le besoin réel du montage final |
| Étage casque dédié | plus tard | Seulement si on le retient en version finale |

## Quantités d'interface retenues

Pour Fantôme IV V1, la face avant cible reste :

- `8 potentiomètres`
- `1 encodeur rotatif cliquable`
- `4 boutons`

## Références à choisir avec soin

Les éléments sur lesquels il vaut mieux éviter l'achat "au hasard" :

- l'encodeur
- les potentiomètres
- l'écran OLED
- le connecteur MIDI In
- le jack audio

Leur format mécanique compte presque autant que leur fonction électrique.

## Liens utiles

- [LISTE-COURSES.md](./LISTE-COURSES.md)
- [Fantome-IV-specs.md](./Fantome-IV-specs.md)
