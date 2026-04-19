# Spec UI/UX Fantôme IV

Ce document décrit l'interface utilisateur de Fantôme IV V1.

Il couvre :

- la logique de navigation
- la répartition des contrôles
- les pages de l'OLED
- les comportements attendus en jeu
- les règles de lisibilité et de cohérence

## Rôle

Ce document sert à transformer les intentions produit en comportement d'interface exploitable.

Il complète :

- [Fantome-IV-specs.md](./Fantome-IV-specs.md)
- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [DECISIONS.md](./DECISIONS.md)

## Statut

Statut actuel :

- document de référence actif
- suffisamment détaillé pour cadrer la V1
- encore ouvert sur quelques détails visuels fins

## 1. Objectif de l'UI V1

L'interface de Fantôme IV doit être :

- lisible
- rapide à prendre en main
- jouable sans réfléchir à l'arborescence
- suffisamment souple pour exposer des paramètres avancés

L'UI V1 ne cherche pas à être spectaculaire.

Elle cherche à être claire et musicale.

## 2. Principes UX

### 2.1 Priorités

- les paramètres essentiels doivent être immédiats
- les paramètres secondaires peuvent vivre dans le menu
- le joueur ne doit pas se perdre
- la logique des pages doit rester stable

### 2.2 Règles

- les contrôles physiques importants gardent toujours le même rôle
- l'écran doit toujours dire ce qui est en train d'être édité
- les noms doivent être courts et compréhensibles
- un même geste doit produire un comportement constant

## 3. Surface de contrôle

### 3.1 Contrôles physiques retenus

- `8 potentiomètres`
- `1 encodeur rotatif cliquable cranté`
- `4 boutons poussoirs momentanés`
- `1 OLED 128x64` `SSD1306` `1,3"`
- `1 LED MIDI` de signalisation

### 3.2 Contrôles dédiés

- Pot 1 : Master volume
- Pot 2 : Filter cutoff
- Pot 3 : Filter resonance
- Pot 4 : Amp Attack
- Pot 5 : Amp Decay
- Pot 6 : Amp Sustain
- Pot 7 : Amp Release

### 3.3 Contrôle contextuel

- Pot 8 : paramètre principal de la page courante

Ce potard sert à modifier rapidement le paramètre le plus pertinent de la page en cours.

### 3.4 Encodeur

Rotation :

- déplacer la sélection
- ajuster les paramètres discrets ou fins

Appui :

- entrer dans l'édition
- valider une action
- confirmer un choix

### 3.5 Boutons

- `Page -` : page précédente
- `Page +` : page suivante
- `Shift / Back` : action secondaire, retour, annulation
- `Preset / Save / Action` : actions liées aux presets ou à la validation selon contexte

Nature des contrôles :

- ce sont `4` boutons poussoirs momentanés
- ils sont distincts du bouton intégré à l'encodeur

Priorité d'implantation :

- la face avant doit rester très lisible
- l'espacement et le confort de lecture priment sur la compacité extrême

### 3.6 Règle de base de l'encodeur

Le comportement retenu pour la V1 est le suivant :

1. rotation encodeur = sélectionner un paramètre ou une ligne
2. appui encodeur = entrer dans l'édition de cet élément
3. rotation encodeur = modifier la valeur en direct
4. appui encodeur = sortir du mode édition
5. rotation encodeur = passer au paramètre suivant

Règle importante :

- les changements de valeur ordinaires sont appliqués en direct
- il n'y a pas de validation supplémentaire pour chaque quantité
- l'appui ne sert pas à confirmer chaque incrément ou chaque pourcentage
- la validation explicite supplémentaire est réservée aux actions destructives ou sensibles

Exemple simple :

- je tourne pour sélectionner `Delay Feedback`
- j'appuie pour éditer
- je tourne jusqu'à la bonne valeur
- j'appuie pour sortir de l'édition
- je tourne pour aller au paramètre suivant

## 4. Modèle de navigation

### 4.1 Niveau principal

L'interface est organisée en pages.

Le changement de page se fait par les boutons `Page -` et `Page +`.

### 4.2 Niveau secondaire

À l'intérieur d'une page :

- l'encodeur déplace la sélection
- l'appui encodeur entre ou sort du mode édition
- le Pot 8 agit comme raccourci du paramètre principal de la page

### 4.3 Rôle de Shift

Le bouton `Shift / Back` sert à :

- revenir en arrière
- accéder aux actions secondaires
- éviter de surcharger les autres boutons

### 4.4 États d'interaction retenus

L'UI V1 fonctionne avec trois états simples :

1. `Navigation`
   L'utilisateur se déplace entre pages, lignes et éléments.

2. `Édition`
   L'utilisateur modifie la valeur de l'élément sélectionné.

3. `Confirmation`
   L'utilisateur confirme une action sensible.

Actions concernées par `Confirmation` :

- `Save preset`
- `Load preset` si risque de perte de contexte jugé important
- `Init patch`

Les paramètres ordinaires n'utilisent pas cet état supplémentaire.

## 5. Philosophie d'affichage OLED

L'écran doit toujours montrer au minimum :

- le preset courant
- la page courante
- le paramètre sélectionné
- la valeur courante
- le canal MIDI si on est sur une page système
- l'état de synchro si pertinent

### 5.1 Priorité visuelle

Ordre de lecture privilégié :

1. nom de page
2. paramètre sélectionné
3. valeur
4. informations annexes

### 5.2 Style

- texte simple
- pas de surcharge graphique
- abréviations cohérentes
- hiérarchie visuelle stable

## 6. Pages retenues

### 6.1 Page 1 - Oscillateurs

But :

- configurer le caractère de base du son

Paramètres typiques :

- Osc A waveform
- Osc A octave / transpose
- Osc A fine tune
- Osc A level
- Osc B waveform
- Osc B octave / transpose
- Osc B fine tune
- Osc B level
- Osc sync
- PWM amount
- Noise level

Pot 8 recommandé :

- `Osc Mix` ou `Noise level` selon le choix final d'implémentation

### 6.2 Page 2 - Filtre

But :

- régler immédiatement la couleur du son

Paramètres typiques :

- cutoff
- resonance
- filter env amount
- filter LFO amount
- S&H amount

Pot 8 recommandé :

- `Filter Env Amount`

### 6.3 Page 3 - ADSR VCA

But :

- contrôler l'enveloppe de volume

Paramètres typiques :

- Attack
- Decay
- Sustain
- Release

Potards dédiés :

- Pot 4 à Pot 7

Pot 8 recommandé :

- doublon temporaire du paramètre sélectionné ou inutilisé selon l'implémentation finale

### 6.4 Page 4 - AR filtre

But :

- régler la dynamique d'ouverture du filtre

Paramètres typiques :

- Filter Attack
- Filter Release
- Filter Env Amount

Pot 8 recommandé :

- `Filter Attack` ou `Filter Env Amount`

### 6.5 Page 5 - LFO / S&H

But :

- régler les modulations temporelles

Sous-ensemble à afficher clairement :

- LFO osc : waveform, mode, division ou rate, amount
- LFO filtre : waveform, mode, division ou rate, amount
- S&H : mode, division ou rate, amount, smooth

Pot 8 recommandé :

- paramètre de modulation actuellement sélectionné

### 6.6 Page 6 - Effets

But :

- doser les effets globaux

Ordre de priorité d'affichage :

1. chorus
2. delay
3. reverb

Paramètres typiques :

- Chorus rate
- Chorus depth
- Chorus mix
- Delay mode
- Delay time ou division
- Delay feedback
- Delay mix
- Reverb size
- Reverb tone
- Reverb mix

Pot 8 recommandé :

- `Chorus mix` ou `Delay feedback` selon la sous-section affichée

Comportement recommandé de l'encodeur sur cette page :

- rotation en mode navigation : choisir `Chorus`, `Delay`, `Reverb` ou un paramètre à l'intérieur
- appui : entrer dans l'élément choisi
- rotation en mode édition : modifier la valeur en direct
- appui : revenir au niveau précédent

Exemple pour le delay :

- l'utilisateur sélectionne `Delay`
- appuie pour entrer dans le bloc
- tourne pour choisir `Mode`, `Time/Division`, `Feedback`, `Mix`
- appuie pour éditer le paramètre choisi
- tourne pour modifier la valeur
- appuie pour sortir de l'édition
- tourne pour aller au paramètre suivant

### 6.7 Page 7 - Système / MIDI / Presets

But :

- gérer la configuration globale

Paramètres typiques :

- MIDI channel
- play mode : poly / mono / unison
- legato si distinct
- preset load
- preset save
- init patch

Pot 8 recommandé :

- paramètre système principal sélectionné

## 7. Presets

### 7.1 Capacité retenue

- `4 presets`

### 7.2 Exigences UX

- rappel simple
- sauvegarde simple
- aucune confusion entre charger et écraser
- soft takeover après chargement
- `init patch` disponible
- rappel du dernier preset au démarrage recommandé

### 7.3 Comportement attendu

- l'écran doit afficher clairement le numéro ou le nom du preset
- le mode `save` doit être distinct du mode `load`
- une action destructive doit nécessiter une validation

## 8. Soft takeover

### 8.1 Objectif

Éviter les sauts de valeur après chargement d'un preset.

### 8.2 Règle UX

- un potentiomètre physique ne doit pas provoquer un saut brutal tant que sa position n'a pas rejoint la valeur logique

### 8.3 Affichage souhaité

- l'écran doit permettre de comprendre qu'un paramètre attend d'être repris

Le détail visuel exact reste `à figer`, mais le comportement lui-même est obligatoire.

## 9. MIDI côté UI

### 9.1 Réglages exposés

- canal MIDI
- éventuellement état de clock
- preset via Program Change

### 9.2 Exigences UX

- le canal MIDI doit être simple à trouver
- la machine doit rester jouable sans ordinateur
- les informations MIDI ne doivent pas envahir l'écran hors pages système

## 10. Modes Free / Clock

Les éléments concernés :

- LFO oscillateurs
- LFO filtre
- Sample & Hold
- Delay

### 10.1 Règle UX

Chaque élément synchronisable doit exposer :

- un mode `Free`
- un mode `Clock`

En mode `Clock`, la valeur affichée doit être une division lisible :

- `1/1`
- `1/2`
- `1/4`
- `1/8`
- `1/16`

La liste exacte pourra évoluer, mais la logique doit rester uniforme.

## 11. Édition en jeu

L'interface doit permettre :

- de jouer sans quitter l'écran principal de travail
- de modifier cutoff, resonance et ADSR ampli immédiatement
- de changer de page rapidement
- de retrouver facilement les paramètres avancés

Règle ergonomique :

- on ne doit pas avoir à valider chaque mouvement de valeur comme dans un formulaire
- l'encodeur sert à entrer/sortir de l'édition, pas à confirmer chaque incrément

## 12. Gestion des erreurs et cas limites

L'UI doit gérer proprement :

- `All Notes Off / Panic`
- valeurs extrêmes de paramètres
- presets inexistants ou corrompus
- absence de clock MIDI
- changement de page en cours de jeu

## 13. Ce que l'UI V1 ne doit pas devenir

- une interface profonde et opaque
- une hiérarchie de menus difficile à mémoriser
- une page système envahissante
- un écran surchargé de graphismes inutiles

## 14. Checklist de validation UI/UX

Avant de considérer l'UI V1 comme bien cadrée, vérifier :

- les rôles des 8 potards sont clairs
- l'encodeur a un comportement constant
- les boutons ont chacun une logique simple
- les pages couvrent tous les besoins V1
- les paramètres essentiels sont accessibles rapidement
- le système de preset est compréhensible
- le canal MIDI est simple à modifier
- les modes `Free / Clock` sont lisibles

## 15. Gate documentaire

On considérera la base UI/UX suffisamment solide quand :

- aucune ambiguïté majeure ne subsiste sur la navigation
- le rôle des contrôles physiques est figé
- les pages de l'écran sont décrites
- les comportements critiques en jeu sont explicités
