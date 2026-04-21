# Fantôme IV

Spécifications V1 du projet de synthétiseur numérique polyphonique.

## 1. Vision du projet

Fantôme IV est un synthétiseur numérique de bureau, polyphonique, conçu autour d'une plateforme embarquée DSP.

Objectif produit :

- Construire un instrument jouable, fiable et réalisable par une petite équipe.
- Obtenir un vrai comportement de polysynth moderne avec une interface physique claire.
- Garder une architecture simple côté hardware, tout en laissant de la place pour une V2.
- Se concentrer sur le plaisir de jeu, le son et la stabilité avant d'ajouter trop de fonctions annexes.

Positionnement V1 :

- Synthèse numérique de type analogique
- 4 voix de polyphonie
- Sortie stéréo
- MIDI In DIN 5 broches
- Écran OLED et presets
- Modulations musicales simples et efficaces
- Effets stéréo intégrés

## 2. Nom du projet

Nom retenu :

- Fantôme IV

## 3. Plateforme recommandée

Plateforme cible retenue :

- Daisy Seed 65MB

Pourquoi :

- DSP audio adapté à un synthé embarqué
- Audio stéréo embarqué de bonne qualité
- ADC suffisants pour une interface simple sans multiplexage lourd
- Écosystème audio plus naturel pour ce type de projet qu'un Arduino classique
- Suffisamment puissant pour 4 voix + modulations + effets stéréo

Stack logicielle retenue :

- Langage : C++
- Bibliothèques : libDaisy + DaisySP
- Build : toolchain Daisy officielle
- Architecture logicielle : cœur portable + cible Daisy + cible desktop de test
- Complément de développement retenu : petite cible desktop pour valider vite la logique et le moteur avant l'achat du hardware

## 4. Principes de conception V1

- Garder le CV/Gate hors du scope de la V1
- Favoriser des contrôles par pages + soft takeover plutôt que trop de potentiomètres
- Filtre, VCA et enveloppes par voix
- Effets sur bus global stéréo
- Priorité au MIDI : l'instrument doit d'abord être excellent en MIDI avant toute extension
- Architecture modulaire du code pour pouvoir ajouter une V2 sans réécriture massive

## 4.1 Direction sonore V1

La direction sonore retenue pour la V1 est la suivante :

- oscillateurs : rendu propre et stable
- filtre : caractère musical avant neutralité clinique
- chorus : large et audible
- reverb : discrète et lisible
- presets de départ : `4` presets d'usine

## 5. Non-objectifs V1

Hors scope de la V1 :

- CV/Gate
- Arpégiateur
- Séquenceur interne complet
- Matrice de modulation complexe
- Aftertouch
- USB host MIDI
- Audio In
- Multitimbralité

Ces points restent possibles en V2, mais ne doivent pas ralentir la V1.

## 6. Spécification fonctionnelle V1

### 6.1 Résumé

Fantôme IV V1 doit offrir :

- 4 voix de polyphonie
- MIDI In DIN 5 broches
- Sortie audio stéréo sur jack TRS
- Écran OLED pour l'édition et la gestion des presets
- 1 LFO dédié à la modulation des oscillateurs
- 1 LFO dédié au filtre
- 1 modulation Sample & Hold pour le filtre, basée sur une source de bruit blanc
- 1 filtre low-pass résonant par voix
- 1 VCA par voix
- 1 ADSR VCA par voix
- 1 AR filtre par voix
- Chorus stéréo global
- Delay stéréo global
- Reverb stéréo globale

### 6.2 Architecture audio globale

Architecture recommandée :

1. MIDI In
2. Allocation de voix
3. Moteur de synthèse par voix
4. Mix des 4 voix
5. Chorus stéréo global
6. Delay stéréo global
7. Reverb stéréo globale
8. Sortie stéréo

### 6.3 Architecture par voix

Chaque voix contient :

- 2 oscillateurs numériques
- 1 générateur de bruit blanc partagé ou dérivé d'une source globale
- 1 mixeur de sources
- 1 filtre low-pass résonant
- 1 VCA
- 1 enveloppe ADSR pour le VCA
- 1 enveloppe AR pour le filtre

Flux du signal par voix :

`OSC A + OSC B + Noise -> Mixer -> Low-pass -> VCA -> Somme globale`

### 6.4 Oscillateurs

Choix retenu en V1 :

- 2 oscillateurs par voix

Fonctions minimales :

- Accord global
- Sélection de forme d'onde indépendante pour OSC A et OSC B
- Réglage de niveau OSC A / OSC B
- Réglage d'octave ou transposition pour OSC A et OSC B
- Fine tune pour OSC A et OSC B
- Synchronisation d'oscillateur
- PWM sur forme carrée / pulse

Formes d'onde minimales recommandées :

- Sine
- Triangle
- Square / Pulse
- Saw
- Noise

### 6.5 Polyphonie et allocation de voix

Règles V1 :

- 4 voix maximum
- Voice stealing propre et prévisible
- Mode prioritaire : oldest voice steal
- Modes de jeu : Poly, Mono, Unison
- Comportement legato en mode Mono et Unison
- Gestion correcte des notes relâchées et du sustain
- Réponse stable en accords et en jeu legato

Fonctions MIDI de base liées au jeu :

- Note On / Note Off
- Pitch Bend
- Mod Wheel
- Sustain Pedal
- All Notes Off / Panic

Fonctions recommandées si le temps le permet :

- Velocity vers VCA
- Velocity vers filtre en quantité optionnelle

## 7. Modulations

### 7.1 LFO oscillateurs

Rôle :

- Moduler le pitch des oscillateurs

Paramètres minimum :

- Rate
- Amount
- Waveform
- Sync on/off
- Mode Free / Clock
- Divisions de tempo en mode Clock

Formes d'onde recommandées :

- Sine
- Triangle
- Square
- Random si le coût et l'interface restent simples

Destination V1 :

- Pitch des oscillateurs

### 7.2 LFO filtre

Rôle :

- Moduler le cutoff du filtre

Paramètres minimum :

- Rate
- Amount
- Waveform
- Sync on/off
- Mode Free / Clock
- Divisions de tempo en mode Clock

Formes d'onde recommandées :

- Sine
- Triangle
- Square

Destination V1 :

- Cutoff du filtre

### 7.3 Sample & Hold filtre

Rôle :

- Créer une modulation aléatoire par pas pour le cutoff

Source :

- Bruit blanc

Paramètres minimum :

- Rate
- Amount
- Sync on/off
- Mode Free / Clock
- Divisions de tempo en mode Clock

Paramètre recommandé :

- Slew / Smooth pour adoucir les marches

Destination V1 :

- Cutoff du filtre

Comportement attendu :

- Free-run possible
- Synchronisation MIDI Clock souhaitée
- Résultat musical sans clics

## 8. Filtre

Type retenu pour la V1 :

- Low-pass résonant par voix

Implémentation DSP recommandée :

- State variable filter ou autre low-pass stable et musical

Paramètres minimum :

- Cutoff
- Resonance
- Filter Env Amount
- Filter LFO Amount
- Sample & Hold Amount

Objectif sonore :

- Filtre "classique" musical
- Stable à résonance élevée
- Sans auto-oscillation obligatoire en V1

## 9. Enveloppes

### 9.1 ADSR VCA

Par voix :

- Attack
- Decay
- Sustain
- Release

Rôle :

- Contrôle du volume de la voix

### 9.2 AR filtre

Par voix :

- Attack
- Release

Rôle :

- Contrôle dynamique du cutoff via une quantité d'enveloppe

Contraintes de qualité :

- Pas de clics
- Temps courts crédibles pour les attaques rapides
- Temps longs utilisables musicalement

## 10. VCA

VCA par voix :

- Contrôle par ADSR VCA
- Possibilité de modulation par velocity en option

Exigences :

- Réponse propre
- Pas de zipper noise
- Bonne gestion du niveau global quand 4 voix jouent simultanément

## 11. Effets

### 11.1 Chorus

Type :

- Chorus stéréo global
- Effet prioritaire de la V1

Paramètres minimum :

- Rate
- Depth
- Mix

Objectif :

- Élargir l'image stéréo
- Donner de l'ampleur sans rendre le son flou par défaut

### 11.2 Delay

Type :

- Delay stéréo global

Paramètres minimum :

- Time en mode Free
- Division en mode Clock
- Feedback
- Mix
- Mode Free / Sync MIDI

Objectif :

- Ajouter de l'espace rythmique sans compliquer l'interface
- Rester musical et facile à doser dans un contexte polysynth

### 11.3 Reverb

Type :

- Reverb stéréo globale
- Effet le moins prioritaire des trois pour la V1

Paramètres minimum :

- Size ou Time
- Tone ou Damping
- Mix

Objectif :

- Reverb musicale, légère à moyenne
- Utilisable en contexte synthé sans noyer le signal

### 11.4 Ordre des effets

Ordre recommandé :

- Mix des voix -> Chorus -> Delay -> Reverb -> Sortie

## 12. Sortie stéréo

Exigence utilisateur :

- Sortie audio jack TRS

Spécification V1 :

- 1 sortie stéréo sur jack TRS 6,35 mm

Recommandation actuelle :

- Prévoir une sortie ligne stéréo simple, propre et avec niveau maîtrisé

## 13. MIDI

### 13.1 Connectique

Minimum V1 :

- MIDI In physique
- interface principale en DIN 5 broches
- entrée opto-isolée

Implémentation hardware recommandée :

- DIN 5 broches avec optocoupleur

Décision V1 :

- le MIDI musical de la V1 passe par l'entrée DIN
- l'USB sert au développement, au flash et au debug
- l'USB n'est pas une exigence d'usage musical de la V1

Extension acceptable plus tard :

- Compatibilité TRS-MIDI via adaptateur ou révision hardware

### 13.2 Messages MIDI supportés

Obligatoires :

- Note On
- Note Off
- Control Change
- Program Change
- Pitch Bend
- Mod Wheel
- Sustain Pedal
- MIDI Clock
- Start / Stop / Continue si la synchro est implémentée

### 13.3 Fonctions MIDI utilisateur

- Jouer les 4 voix
- Choisir le canal MIDI dans l'interface
- Charger les presets par Program Change
- Synchroniser les LFO, le S&H et le delay à l'horloge MIDI
- Utiliser la Mod Wheel comme modulation musicale utile
- Pitch bend avec plage de 2 demi-tons

### 13.4 Mapping MIDI de base recommandé

Proposition initiale :

- CC 1 : Mod Wheel
- CC 7 : Volume master
- CC 10 : Pan / width global si implémenté
- CC 71 : Resonance
- CC 74 : Filter Cutoff

Mapping additionnel à définir pendant l'implémentation :

- Attack / Decay / Sustain / Release
- LFO rates
- FX mix
- Preset-safe parameters

### 13.5 Exigence d'intégration

- le synthé doit être pleinement jouable via son entrée MIDI DIN sans dépendre de l'USB
- le comportement MIDI doit rester identique que l'appareil soit branché ou non à un ordinateur

## 14. Presets

### 14.1 Objectif

Permettre la sauvegarde et le rappel rapide des sons.

### 14.2 Spécification V1

- Sauvegarde locale sur mémoire non volatile
- Écran de sélection des presets
- Save / Load depuis l'interface
- Chargement via Program Change

Capacité recommandée :

- 4 presets utilisateur

### 14.3 Exigences

- Chargement sans comportement destructif
- Soft takeover des potentiomètres après rappel d'un preset
- Init patch disponible à tout moment
- Rappel du dernier preset au démarrage recommandé
- Versionnement simple de la structure preset pour éviter les incompatibilités futures

## 15. Interface utilisateur

### 15.1 Philosophie UI

Objectif :

- Peu de contrôles, mais bien choisis
- Navigation rapide
- Utilisable sans devoir mémoriser un menu trop profond
- Écran lisible et comportement évident en jeu
- Lisibilité de façade prioritaire sur la compacité extrême

### 15.2 Interface physique recommandée

V1 recommandée :

- 1 écran OLED `128x64` `1,3"` en `I2C`
- 1 encodeur rotatif cliquable cranté
- 8 potentiomètres de panneau
- 4 boutons poussoirs de panneau
- 1 LED MIDI de signalisation

Boutons recommandés :

- Page -
- Page +
- Shift / Back
- Preset / Save

Potentiomètres :

- 7 contrôles dédiés
- 1 contrôle contextuel selon la page
- soft takeover obligatoire après chargement de preset

### 15.3 Pages UI recommandées

Page 1 :

- Oscillateurs

Page 2 :

- Filtre

Page 3 :

- ADSR VCA

Page 4 :

- AR filtre

Page 5 :

- LFO / S&H

Page 6 :

- Effets

Page 7 :

- Système / MIDI / Presets

### 15.4 Informations affichées

Minimum :

- Nom ou numéro du preset
- Nom du paramètre édité
- Valeur du paramètre
- Page courante
- Canal MIDI courant
- État de synchro si utile

### 15.5 Répartition de contrôle retenue

Proposition retenue pour la V1 :

- Pot 1 : Master volume
- Pot 2 : Filter cutoff
- Pot 3 : Filter resonance
- Pot 4 : Amp Attack
- Pot 5 : Amp Decay
- Pot 6 : Amp Sustain
- Pot 7 : Amp Release
- Pot 8 : paramètre contextuel principal de la page courante

Rôle de l'encodeur :

- naviguer dans la page
- sélectionner le paramètre à éditer
- ajuster les valeurs de menu : waveform, tuning, sync, type d'effet, canal MIDI, preset

Rôle des boutons :

- Page -
- Page +
- Shift / Back
- Preset / Save / Action

## 16. Paramètres exposés à l'utilisateur

Liste minimale recommandée :

- Osc A waveform
- Osc A octave ou transpose
- Osc A fine tune
- Osc A level
- Osc B waveform
- Osc B octave ou transpose
- Osc B fine tune
- Osc B level
- Osc sync on/off
- PWM amount
- Noise level
- Filter cutoff
- Filter resonance
- Filter env amount
- Amp ADSR A
- Amp ADSR D
- Amp ADSR S
- Amp ADSR R
- Filter AR A
- Filter AR R
- LFO osc rate
- LFO osc amount
- LFO osc waveform
- LFO osc mode free/clock
- LFO osc division
- LFO filter rate
- LFO filter amount
- LFO filter waveform
- LFO filter mode free/clock
- LFO filter division
- S&H rate
- S&H amount
- S&H smooth
- S&H mode free/clock
- S&H division
- Chorus rate
- Chorus depth
- Chorus mix
- Delay time
- Delay sync division
- Delay feedback
- Delay mix
- Reverb size
- Reverb tone
- Reverb mix
- Master volume

## 17. Hardware V1 recommandé

### 17.1 Carte principale

- Daisy Seed

### 17.2 Affichage

- OLED `128x64` `1,3"` en `I2C`

### 17.3 Contrôles

- 8 potentiomètres de panneau
- 1 encodeur rotatif cranté avec bouton
- 4 boutons poussoirs de panneau
- priorité mécanique à une façade très lisible

### 17.4 MIDI

- 1 entrée MIDI In DIN 5 broches
- 1 optocoupleur
- 1 LED MIDI de signalisation

### 17.5 Audio

- 1 sortie stéréo jack TRS 6,35 mm
- étage de sortie adapté à l'usage retenu

### 17.6 Alimentation

V1 prototype :

- Alimentation par USB
- Câblage simple et stable pour le prototype

Recommandation :

- Dissocier autant que possible l'alimentation audio et le bruit numérique
- Soigner la masse, le routage audio et le découplage

### 17.7 Simplifications hardware retenues

Pour la V1 :

- pas de CV/Gate
- pas d'audio input
- pas de multiplexeur obligatoire si le nombre de contrôles reste raisonnable

## 18. Architecture logicielle recommandée

Modules logiciels à prévoir :

- `AudioEngine`
- `Voice`
- `Oscillator`
- `Filter`
- `Envelope`
- `Lfo`
- `SampleHold`
- `FxChorus`
- `FxDelay`
- `FxReverb`
- `PresetManager`
- `MidiManager`
- `UiManager`
- `ParameterStore`

Principes :

- Séparer clairement DSP, UI et hardware
- Paramètres centralisés
- Sauvegarde preset basée sur une structure stable
- Pas de logique écran dans le code audio
- Pas d'allocation dynamique dans la callback audio

## 19. Performance et contraintes techniques

Le système doit tenir :

- 4 voix
- 2 oscillateurs par voix
- 2 enveloppes par voix
- filtre par voix
- chorus stéréo
- delay stéréo
- reverb stéréo
- UI réactive
- MIDI fiable

Contraintes :

- aucune coupure audible en jeu normal
- aucune latence anormale des contrôles
- pas de clics majeurs à l'édition
- charge CPU maîtrisable avec marge de sécurité

## 20. Qualité attendue

Définition d'une "V1 réussie" :

- Instrument stable
- Son musical
- 4 notes jouables proprement
- Presets fiables
- Interface claire
- Sortie stéréo exploitable
- Modulations expressives

## 21. Roadmap de développement

### Phase 0 - Cadrage

- Valider ces spécifications
- Consolider la documentation comme source de vérité
- Ouvrir le journal de décisions du projet
- Ouvrir le nouveau dépôt
- Poser l'arborescence du projet
- Définir les conventions de nommage et de build

### Phase 1A - Socle logiciel portable

- Poser le cœur logiciel portable
- Implémenter le modèle de patch
- Implémenter la banque de `4` presets d'usine
- Implémenter le moteur MIDI
- Implémenter l'allocation de voix
- Ajouter une cible desktop de test

### Phase 1B - Moteur de synthèse nu

- Implémenter 1 voix complète
- Ajouter 2 oscillateurs par voix
- Viser des oscillateurs propres dès la première base
- Ajouter filtre, VCA, enveloppes
- Implémenter 4 voix et l'allocation complète

### Phase 2 - Modulations

- Ajouter LFO oscillateurs
- Ajouter LFO filtre
- Ajouter Sample & Hold filtre
- Ajouter modes Free / Clock et divisions rythmiques
- Ajouter Pitch Bend / Mod Wheel / Sustain

### Phase 3 - Effets

- Intégrer chorus stéréo
- Intégrer delay stéréo
- Intégrer reverb stéréo
- Stabiliser le gain staging et le niveau de sortie

### Phase 4 - Interface et presets

- Écran OLED
- Encodeur et boutons
- Édition des paramètres par pages
- Sauvegarde / rappel des presets
- Soft takeover

### Phase 5 - MIDI complet

- Program Change
- sélection du canal MIDI dans l'interface
- CC mapping
- MIDI Clock sync pour les LFO, le S&H et le delay

### Phase 6 - Intégration hardware

- Tests avec la vraie face avant
- Vérification des contrôles
- Validation de la sortie audio
- Nettoyage UX final

## 22. Risques à surveiller

- Charge CPU trop haute avec chorus + delay + reverb + 4 voix
- Reverb trop coûteuse pour la V1
- Delay mal dosé ou trop envahissant dans le mix global
- Mauvais gain staging provoquant saturation ou souffle
- Sortie ligne stéréo insuffisamment dimensionnée
- Répartition des contrôles physiques peu naturelle en jeu
- Synchro MIDI Clock plus délicate que prévu
- UI trop chargée pour un petit OLED

Mitigations :

- Commencer sans reverb lourde si nécessaire
- Garder un delay global simple en V1
- Garder la reverb discrète
- Privilégier un chorus ample plutôt qu'une accumulation d'effets
- Mesurer la charge DSP très tôt
- Garder les effets sur bus global uniquement
- Fixer l'interface physique avant la PCB

## 23. Critères d'acceptation V1

Fantôme IV V1 est considérée comme terminée si :

- 4 notes simultanées fonctionnent proprement
- Les 2 oscillateurs par voix sont stables et accordables
- Le filtre low-pass est musical et pilotable
- L'ADSR VCA et l'AR filtre par voix fonctionnent correctement
- Les 2 LFO fonctionnent correctement
- Le Sample & Hold filtre fonctionne avec bruit blanc
- Chorus, delay et reverb stéréo sont utilisables
- Les presets peuvent être sauvegardés et rappelés
- L'écran permet l'édition sans confusion
- MIDI In fonctionne de manière fiable
- Le canal MIDI est sélectionnable depuis l'interface
- Program Change et au moins un set de CC utiles fonctionnent
- La sortie stéréo sur jack TRS est validée

## 24. Évolutions V2 envisagées

- CV In pour cutoff ou pitch mono
- Gate In
- Arpégiateur
- Matrice de modulation
- Plus de formes d'onde
- Sortie casque dédiée si la V1 part en simple ligne stéréo

## 25. Décisions gelées pour le démarrage

Décisions à considérer comme figées au début du projet :

- Nom : Fantôme IV
- Plateforme : Daisy Seed
- Langage : C++
- 4 voix
- MIDI In
- Sortie stéréo sur jack TRS
- OLED + presets
- 2 oscillateurs par voix
- 1 filtre low-pass résonant par voix
- 1 VCA par voix
- 1 ADSR VCA par voix
- 1 AR filtre par voix
- 1 LFO pitch global
- 1 LFO filtre global
- 1 Sample & Hold filtre global à partir de bruit blanc
- Chorus stéréo global
- Delay stéréo global
- Reverb stéréo globale
- 4 presets utilisateur
- Canal MIDI sélectionnable
- Pas de CV/Gate en V1

## 26. Prochaine étape

Quand le nouveau répertoire sera créé, il faudra :

1. Y copier ou y recréer ce document.
2. Initialiser la structure du projet firmware.
3. Commencer par le moteur audio minimal sans interface.
4. Ajouter l'UI et les presets ensuite.
