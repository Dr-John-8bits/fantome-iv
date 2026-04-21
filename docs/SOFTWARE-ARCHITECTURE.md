# Architecture logicielle Fantôme IV

Ce document définit la stratégie logicielle du projet.

Son rôle est simple :

- permettre d'avancer fortement sur le firmware avant l'achat du hardware
- séparer le code portable du code spécifique à la `Daisy Seed`
- faire du Mac un vrai environnement de travail utile pour la V1

## 1. Principe directeur

Fantôme IV suit désormais une logique `soft-first`.

Cela veut dire :

- on code d'abord le cœur du synthé en `C++` portable
- on valide tôt la logique de patch, de MIDI, de presets et d'allocation de voix sur Mac
- on garde l'intégration `Daisy` comme une couche d'adaptation séparée
- on n'attend pas l'achat du hardware pour commencer à écrire sérieusement le firmware

## 2. Couches logicielles

### 2.1 `firmware/core/`

Rôle :

- porter le cœur du synthé
- rester indépendant de `libDaisy`
- pouvoir être compilé localement sur Mac

Contenu attendu :

- structures de patch
- banque de presets
- moteur MIDI
- allocation de voix
- transport et état de clock
- DSP portable au fur et à mesure

### 2.2 `firmware/targets/desktop/`

Rôle :

- exécuter localement des tests rapides
- vérifier les comportements sans hardware
- permettre des démonstrations simples de l'état interne

Cette cible n'a pas vocation à devenir un plugin ou une grosse application.

Elle sert de `smoke test` logiciel.

### 2.3 `firmware/targets/daisy/`

Rôle :

- intégrer le cœur portable au runtime `Daisy`
- héberger l'audio callback
- brancher `MIDI UART`, `OLED`, `ADC`, encodeur, boutons et stockage

Règle :

- cette couche doit rester la plus fine possible
- elle ne doit pas aspirer la logique métier du synthé

### 2.4 `firmware/tests/`

Rôle :

- valider les comportements critiques
- sécuriser les régressions
- documenter par l'exemple ce que le firmware doit faire

Exemples de sujets à tester :

- voice stealing
- sustain
- rappel de presets
- mapping `CC`
- comportement `Program Change`
- modes `Poly`, `Mono`, `Unison`

## 2.5 Direction sonore de travail

Le socle logiciel V1 doit partir avec ces intentions :

- oscillateurs propres et stables
- filtre à caractère musical
- chorus large et audible
- reverb discrète
- `4` presets d'usine dès la base logicielle

## 3. Ce qu'on peut avancer sans hardware

On peut avancer immédiatement sur :

- modèle de données du synthé
- paramètres V1
- banque de presets
- logique MIDI
- allocation de voix
- comportement des modes de jeu
- structure UI d'état
- représentation portable de l'écran OLED
- splash screen portable avec durée d'affichage contrôlée
- persistance portable de session
- cycle de session portable explicite `Boot / Checkpoint / Shutdown`
- simulation logicielle des contrôles physiques
- première implémentation DSP portable
- tests de cohérence

## 4. Ce qui attendra le hardware

Les points suivants ont besoin de la vraie `Daisy Seed` ou au moins de sa toolchain :

- validation CPU réelle
- validation temps réel de l'audio callback
- branchement des potentiomètres, boutons et encodeur
- affichage OLED réel
- test du `MIDI DIN`
- validation de la sortie audio physique
- persistance réelle sur la plateforme cible

## 5. Contrat d'architecture

Le cœur logiciel doit respecter ces règles :

- pas d'include `libDaisy` dans `firmware/core/`
- pas de dépendance hardware dans les structures de patch
- pas de logique produit cachée dans la cible `Daisy`
- les comportements MIDI documentés doivent vivre dans le cœur portable
- les tests doivent couvrir en priorité la logique musicale, pas l'I/O

## 6. Point de départ retenu

Le premier jalon logiciel utile est :

1. créer l'arborescence firmware
2. poser le modèle de patch
3. poser la banque de `4` presets
4. implémenter le moteur MIDI de base
5. implémenter l'allocation de voix
6. exposer une cible desktop simple
7. écrire les premiers tests

Ce jalon permet déjà de faire du vrai travail de firmware, sans matériel.

## 7. Impact sur le projet

Cette architecture permet de :

- retarder l'achat hardware sans bloquer le développement
- réduire le risque de se retrouver avec une carte branchée mais pas de socle logiciel
- garder le projet open source lisible et maintenable
- faciliter la reprise du projet par d'autres personnes

## 8. Jalons déjà posés

La base logicielle porte déjà :

- le modèle de patch V1
- la banque de `4` presets d'usine
- le moteur MIDI de base
- l'allocation de voix
- une cible desktop de smoke test
- des tests locaux
- une première voix DSP portable avec oscillateurs, enveloppes et filtre
- un premier rendu stéréo de travail
- les premières modulations portables `LFO + S&H`
- une base `Free / Clock` pour les modulations
- un `delay` stéréo global portable
- un `chorus` stéréo global portable
- une `reverb` stéréo globale portable
- un état UI portable par pages
- une logique de `soft takeover` portable
- une persistance portable de la banque de presets
- une représentation portable de l'écran OLED page par page
- un contrôleur portable de splash screen au démarrage
- une persistance portable de session
- un session manager portable explicite pour le cycle de démarrage et d'arrêt
- une façade d'input portable pour simuler encodeur, boutons et potentiomètres

Ce n'est pas encore la qualité sonore finale.

C'est en revanche une vraie base de firmware exploitable sans hardware.
