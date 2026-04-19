# Roadmap Fantôme IV

Cette roadmap reste volontairement simple. Elle sert à garder une vue claire du projet sans recopier toutes les specs.

## Phase 0 - Base du projet

- finaliser les specs V1
- figer les besoins fonctionnels V1
- définir la documentation comme source de vérité du projet
- ouvrir et maintenir un journal de décisions
- fixer la licence du dépôt
- poser le README, la roadmap et la page projet
- figer la base matérielle sur Daisy Seed 65MB
- valider l'interface V1 : 8 potentiomètres, 1 encodeur, 4 boutons, 1 OLED
- figer la répartition des contrôles physiques et du menu encodeur
- figer l'entrée `MIDI DIN 5 broches` comme interface musicale principale
- préparer l'arborescence firmware

## Gate avant phase 1

La phase 1 ne démarre pas tant que :

- les specs V1 ne sont pas suffisamment stables
- la BOM n'est pas exploitable
- les décisions structurantes ne sont pas écrites
- le MIDI, l'interface et les presets ne sont pas suffisamment décrits

## Phase 1 - Moteur audio

- initialiser le projet Daisy
- faire tourner une voix complète
- valider oscillateurs, filtre, VCA et enveloppes
- mesurer tôt la charge DSP

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
- stabiliser niveaux, headroom et mix

## Phase 4 - Interface et presets

- brancher l'écran OLED
- brancher encodeur, boutons et potentiomètres
- éditer les paramètres par pages
- ajouter sauvegarde et rappel des presets
- implémenter le soft takeover

## Phase 5 - MIDI complet

- Program Change
- sélection du canal MIDI dans l'interface
- mapping CC principal
- synchro MIDI Clock pour les modulations et le delay

## Phase 6 - Intégration hardware

- tester avec la vraie face avant
- valider les contrôles
- valider la sortie stéréo
- nettoyer l'expérience utilisateur finale

## Cap du projet

La V1 reste saine tant que l'on garde :

- une architecture simple
- une charge DSP sous contrôle
- une interface claire
- un instrument musical avant tout
