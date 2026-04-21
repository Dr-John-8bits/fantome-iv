# Daisy Target Fantôme IV

Ce dossier porte maintenant une première cible `Daisy` volontairement très mince.

## Intention

- garder tout le comportement produit dans `core/`
- limiter `targets/daisy/` à l'adaptation `libDaisy`
- préparer dès maintenant une couture propre entre le runtime portable et le hardware réel

## État actuel

- [main.cpp](./main.cpp) existe
- [DaisyPlatform.h](./DaisyPlatform.h) définit l'interface minimale de plateforme
- [DaisyApp.h](./DaisyApp.h) recolle cette plateforme au runtime portable
- [DaisyPeripherals.h](./DaisyPeripherals.h) sépare les interfaces `ADC`, `OLED` et `MIDI UART`
- [DaisyPlatformStub.h](./DaisyPlatformStub.h) fournit une implémentation de test sans hardware
- `DaisyApp` sépare maintenant explicitement :
  - le tick de contrôle
  - le rendu audio bloc par bloc
- un scanner portable transforme les lectures brutes de contrôles en événements firmware stables
- le stub `MIDI UART` sait parser un flux d'octets MIDI en messages firmware
- le stub `ADC` sait accepter des échantillons de potentiomètres bruts
- si `libDaisy` n'est pas disponible, la cible compile en mode `stub`
- si `libDaisy` est disponible plus tard, ce point d'entrée servira de base à l'intégration réelle

## Ce qui vivra ici ensuite

- l'audio callback temps réel
- le scan `ADC` des `8` potentiomètres
- l'encodeur et les `4` boutons
- l'affichage OLED réel
- le `MIDI UART DIN`
- la persistance finale sur la cible
- le branchement effectif des callbacks audio temps réel

## Règle

- le code `core/` reste portable
- la logique de synthé, de preset, de session et d'UI ne doit pas être redéplacée ici
- cette couche doit seulement traduire le hardware réel vers les abstractions d'entrée/sortie du runtime portable
