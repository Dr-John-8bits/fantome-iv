# Roadmap Fantôme IV

Cette roadmap reste volontairement simple. Elle sert à garder une vue claire du projet sans recopier toutes les specs.

## État actuel

Au `21 avril 2026` :

- la phase 0 documentaire est largement solide
- la phase 1A logicielle est concrètement ouverte et déjà très avancée
- le cœur portable, l'UI portable, la persistance et le smoke test existent
- la BOM finale et le guide d'installation matériel sont prêts
- la prochaine marche logique est maintenant l'achat puis le bring-up matériel réel

## Phase 0 - Base du projet

- finaliser les specs V1
- figer les besoins fonctionnels V1
- définir la documentation comme source de vérité du projet
- ouvrir et maintenir un journal de décisions
- fixer la licence du dépôt
- poser le README, la roadmap et la page projet
- figer la base matérielle sur Daisy Seed 65MB
- rédiger la référence hardware du projet
- rédiger la spec d'alimentation
- rédiger la spec mécanique
- figer la pin map Daisy
- figer le bloc `MIDI In` au niveau composants et valeurs
- figer la topologie de sortie audio
- figer les références exactes des composants critiques
- rédiger la spec de schéma
- rédiger la spec UI/UX
- rédiger la spec MIDI
- valider l'interface V1 : 8 potentiomètres de panneau, 1 encodeur cranté, 4 boutons poussoirs, 1 OLED 1,3"
- figer la répartition des contrôles physiques et du menu encodeur
- figer l'entrée `MIDI DIN 5 broches` comme interface musicale principale
- préparer l'arborescence firmware

## Gate avant phase 1A logicielle

La phase 1A logicielle ne démarre pas tant que :

- les specs V1 ne sont pas suffisamment stables
- la référence hardware n'est pas assez précise
- la spec de schéma n'est pas assez précise
- la spec UI/UX n'est pas assez précise
- la spec MIDI n'est pas assez précise
- la BOM n'est pas exploitable
- les décisions structurantes ne sont pas écrites
- le MIDI, l'interface et les presets ne sont pas suffisamment décrits

## Gate avant phase 6 - Intégration Daisy et hardware

L'intégration Daisy et hardware ne démarre pas tant que :

- la stratégie d'alimentation autonome future n'est pas encore cadrée
- l'implantation mécanique finale n'est pas encore figée
- le support mécanique de la Daisy n'est pas encore cadré
- le cœur logiciel portable n'est pas assez mature

État :

- gate levée

## Phase 1A - Socle logiciel portable

- poser l'arborescence `firmware/`
- implémenter le modèle de patch V1
- implémenter la banque de `4` slots utilisateur
- implémenter le moteur MIDI de base
- implémenter l'allocation de voix
- créer une cible desktop simple
- écrire les premiers tests locaux

État :

- largement réalisé

## Phase 1B - DSP de base

- implémenter une première voix complète dans le cœur portable
- valider oscillateurs, filtre, VCA et enveloppes
- viser des oscillateurs propres dès le départ
- viser un filtre musical plutôt que clinique
- préparer des mesures de coût DSP le plus tôt possible

## Phase 2 - Polyphonie et modulation

- passer à 4 voix
- ajouter allocation de voix et voice stealing
- ajouter les modes mono, unison et legato
- intégrer LFO pitch, LFO filtre et Sample & Hold
- ajouter modes Free / Clock et divisions rythmiques
- ajouter Pitch Bend, Mod Wheel et sustain

## Phase 3 - Effets

- intégrer chorus global
- intégrer delay global
- intégrer reverb globale
- viser un chorus large et clairement audible
- garder une reverb discrète et utile
- stabiliser niveaux, headroom et mix

## Phase 4 - Interface et presets

- brancher l'écran OLED
- brancher encodeur, boutons et potentiomètres
- éditer les paramètres par pages
- ajouter sauvegarde et rappel des presets
- implémenter le soft takeover

État :

- très avancé côté portable
- pas encore branché au vrai hardware

## Phase 5 - MIDI complet

- Program Change
- sélection du canal MIDI dans l'interface
- mapping CC principal
- synchro MIDI Clock pour les modulations et le delay

## Phase 6 - Intégration Daisy et hardware

- acheter le matériel selon la BOM finale
- réceptionner et contrôler les références
- valider la `Daisy Seed` seule au bootloader
- brancher la cible `libDaisy`
- brancher le runtime portable abstrait au vrai scan hardware
- brancher l'audio stéréo
- brancher d'abord un seul potard, puis les `8`
- brancher encodeur, boutons et LED MIDI
- brancher l'OLED
- brancher enfin le MIDI In DIN
- tester avec la vraie face avant
- valider les contrôles
- valider la sortie stéréo
- nettoyer l'expérience utilisateur finale

## Priorité recommandée

À court terme :

- acheter le matériel selon la BOM finale
- suivre l'ordre d'installation matériel documenté
- valider la `Daisy Seed` seule
- commencer le bring-up réel par blocs

## Cap du projet

La V1 reste saine tant que l'on garde :

- une architecture simple
- une charge DSP sous contrôle
- une interface claire
- un instrument musical avant tout
