# Spec Mécanique Fantôme IV

Ce document fixe la direction mécanique de Fantôme IV V1.

## Rôle

Ce document sert à cadrer :

- la logique de façade
- l'implantation des contrôles
- le montage de la `Daisy Seed`
- l'implantation des connecteurs
- les règles de lisibilité et de service

Il ne remplace pas un plan de perçage final.

Il pose la structure mécanique de référence avant CAO détaillée.

## Statut

Statut actuel :

- document de référence actif
- direction mécanique V1 posée
- géométrie finale de façade encore à figer plus tard

## 1. Intention mécanique V1

Fantôme IV V1 doit être :

- un synthé desktop
- lisible avant tout
- proche de la façade finale, même en prototype
- simple à câbler et à maintenir

La V1 ne cherche pas :

- la compacité maximale
- l'optimisation industrielle finale
- une intégration extrême dès le premier prototype

## 2. Architecture mécanique retenue

L'architecture mécanique de référence est la suivante :

- contrôles de façade montés sur panneau
- `Daisy Seed` montée hors panneau, sur support ou carte porteuse
- connecteurs audio, MIDI et USB placés en zone arrière ou latérale arrière
- écran placé dans la zone visuelle haute
- zone de contrôle principale réservée aux `8` potentiomètres

## 3. Référence physique Daisy Seed

D'après la documentation officielle Daisy :

- longueur carte : `51,15 mm`
- largeur carte : `18,00 mm`
- hauteur max de référence : `8,4 mm`
- double rangée de `20` broches
- pas des broches : `2,54 mm`
- entraxe entre rangées : `15,24 mm`

Conséquence pour Fantôme IV :

- la Seed doit être pensée comme un module enfichable ou au moins démontable
- on ne veut pas d'une intégration qui condamne l'accès USB ou le remplacement de la carte

## 4. Montage Daisy retenu

Le montage mécanique recommandé pour la V1 est :

- `Daisy Seed` sur deux barrettes femelles ou supports tulipe
- la carte portée par une plaque proto, une carte porteuse ou un sous-ensemble propre
- pas de câblage volant directement soudé sur toutes les pins de la Seed si on peut l'éviter

Règles :

- laisser l'accès au `micro-USB`
- éviter toute contrainte mécanique sur le connecteur USB
- éviter que la façade ou les câbles tirent directement sur la Seed

## 5. Référence physique de l'écran

Le module OLED retenu est :

- `ER-OLEDM013-1W-I2C`

Dimensions utiles de référence :

- encombrement module : `35,40 mm x 33,50 mm`
- zone visible : `31,42 mm x 16,70 mm`

Conséquences :

- l'écran doit vivre dans une zone dédiée de façade
- il faut prévoir une fenêtre lisible et propre
- il faut laisser une marge mécanique autour du module, du connecteur et des entretoises éventuelles

## 6. Zoning de façade retenu

La façade de référence est organisée en zones.

### 6.1 Zone haute

Zone haute recommandée :

- écran OLED
- LED MIDI
- encodeur
- groupe de `4` boutons

But :

- concentrer navigation et lecture dans la même zone
- éviter d'éparpiller l'interface système

### 6.2 Zone principale de contrôle

Zone principale recommandée :

- `8` potentiomètres

Disposition recommandée :

- `2` rangées de `4` potentiomètres

Pourquoi :

- meilleure lisibilité
- largeur plus raisonnable qu'une ligne unique de `8`
- groupement plus naturel pour `7` contrôles dédiés + `1` pot contextuel

### 6.3 Zone arrière

Zone arrière recommandée :

- `MIDI DIN In`
- sortie audio `TRS 6,35 mm`
- accès `micro-USB`

Pourquoi :

- instrument plus propre en usage normal
- câbles moins gênants sur la face avant

## 7. Règles d'ergonomie retenues

### 7.1 Potentiomètres

Recommandations de façade :

- entraxe recommandé : `25 mm` minimum entre centres
- `22 mm` est considéré comme la limite basse à ne pas franchir sans raison forte
- diamètre de knob cible recommandé : `14 mm` à `16 mm`

Conséquence :

- si des knobs plus gros sont retenus, il faudra revoir l'entraxe

### 7.2 Encodeur

Recommandations :

- l'encodeur doit rester proche de l'écran
- il doit rester clairement distinct du groupe de boutons
- laisser un dégagement suffisant pour l'usage au doigt sans toucher l'écran ou un bouton voisin

### 7.3 Boutons

Recommandations :

- groupement cohérent autour de l'écran/encodeur
- espacement centre à centre recommandé : `16 mm` ou plus
- lecture visuelle évidente des rôles `Page -`, `Page +`, `Shift / Back`, `Preset / Save / Action`

### 7.4 Écran

Recommandations :

- écran dans l'axe visuel
- pas caché par les doigts pendant l'usage de l'encodeur
- marge visuelle autour de la fenêtre de façade : `2 mm` minimum recommandés

## 8. Références mécaniques des composants retenus

### 8.1 Potentiomètres

Référence retenue :

- `Alpha RD901F-40-15F-B10K-00DL1`

Direction retenue :

- axe plat `6 mm`
- longueur d'axe `15 mm`
- vrai montage panneau

### 8.2 Encodeur

Référence retenue :

- `Bourns PEC11R-4225F-S0024`

Direction retenue :

- encodeur cranté
- axe plat `6 mm`
- bouton intégré
- montage panneau

### 8.3 Boutons

Référence retenue :

- `APEM IPR3FAD7/1`

Direction retenue :

- boutons de panneau
- momentané `NO`
- usage confortable en façade

### 8.4 Connecteurs

Références retenues :

- `Switchcraft 57GB5FX` pour le `MIDI DIN In`
- `Switchcraft 155` pour la sortie audio

Direction retenue :

- montage panneau
- câblage arrière

## 9. Règles de câblage mécanique

Le prototype doit rester maintenable.

Donc :

- éviter les faisceaux trop courts qui empêchent l'ouverture ou la maintenance
- prévoir un peu de mou de service
- regrouper les câbles par famille : audio, analogique, numérique
- éviter qu'un câble exerce un effort sur un module ou un connecteur de façade

## 10. Support de façade recommandé

Pour le prototype proche façade finale, la direction recommandée est :

- panneau plat rigide
- matériau de `2 mm` à `3 mm` recommandé

Matériaux acceptables en prototype :

- aluminium
- acrylique épais
- FR4 de façade
- panneau imprimé 3D renforcé si les zones jack/MIDI sont correctement reprises

## 11. Ce qui n'est pas retenu en V1

- pas de mini-format extrême
- pas de façade saturée en contrôles
- pas de connecteurs audio ou MIDI placés en pleine zone de jeu si on peut l'éviter
- pas de Daisy soudée de façon non remplaçable dans un empilement fragile

## 12. Points encore à figer

Les points encore ouverts ne remettent pas en cause la direction mécanique, mais restent à décider :

- dimensions finales exactes du panneau
- position exacte de chaque trou
- matériau final du boîtier
- stratégie de fixation du module OLED
- support mécanique exact de la `Daisy Seed`

## 13. Checklist de validation

La direction mécanique sera considérée comme respectée si :

- la façade reste lisible
- la Daisy reste accessible et remplaçable
- l'USB reste accessible
- l'écran est dans une zone visuelle haute
- les `8` potentiomètres sont groupés de manière claire
- les connecteurs de jeu normal sont à l'arrière ou en zone arrière
- le câblage n'impose pas de contrainte mécanique sur les modules

## 14. Documents liés

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [COMPONENT-REFERENCES.md](./COMPONENT-REFERENCES.md)
- [UI-UX-SPEC.md](./UI-UX-SPEC.md)
