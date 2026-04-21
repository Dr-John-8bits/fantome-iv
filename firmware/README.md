# Firmware Fantôme IV

Cette arborescence sert à avancer le plus loin possible sur le firmware et le logiciel avant l'achat du hardware final.

## Principe

On sépare volontairement le code en trois couches :

- `core/` : logique portable du synthé, sans dépendance `libDaisy`
- `targets/desktop/` : petit exécutable local pour faire des tests rapides sur Mac
- `targets/daisy/` : futur point d'entrée hardware pour la `Daisy Seed`
- `tests/` : tests locaux des comportements critiques

## Ce que cette base couvre déjà

- modèle de patch V1
- banque de `4` presets
- contrat MIDI principal
- allocation de voix
- modes `Poly`, `Mono`, `Unison`
- gestion de `Pitch Bend`, `Mod Wheel`, `Sustain`, `CC`, `Program Change`
- état de transport pour la synchro `MIDI Clock`
- première voix DSP portable
- rendu audio stéréo de base sur Mac
- oscillateurs de départ, enveloppes et filtre low-pass
- `LFO` pitch et filtre dans le cœur portable
- `Sample & Hold` filtre dans le cœur portable
- modes `Free / Clock` de base pour les modulations
- `delay` stéréo global portable
- `chorus` stéréo global portable
- `reverb` stéréo globale portable
- état UI portable
- paramètres éditables par pages
- `soft takeover` portable pour les `8` potentiomètres
- persistance portable de la banque de `4` presets

## Ce que cette base ne fait pas encore

- effets globaux complets
- oscillateurs et filtre DSP finalisés
- intégration `libDaisy`
- UI OLED réelle
- persistance réelle sur hardware

## Build local

Depuis la racine du dépôt :

```bash
cmake -S firmware -B firmware/build
cmake --build firmware/build
./firmware/build/fantome_desktop_smoke
./firmware/build/fantome_tests
```

## Intention

Le but n'est pas de simuler toute la machine dans le vide.

Le but est de sécuriser très tôt :

- l'architecture logicielle
- les structures de données
- le comportement MIDI
- les règles d'allocation de voix
- les points de friction probables avant l'arrivée du hardware
