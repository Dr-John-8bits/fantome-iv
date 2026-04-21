# Daisy Target Fantôme IV

Ce dossier porte maintenant une première cible `Daisy` volontairement très mince.

## Intention

- garder tout le comportement produit dans `core/`
- limiter `targets/daisy/` à l'adaptation `libDaisy`
- préparer dès maintenant une couture propre entre le runtime portable et le hardware réel

## État actuel

- [main.cpp](./main.cpp) existe
- si `libDaisy` n'est pas disponible, la cible compile en mode `stub`
- si `libDaisy` est disponible plus tard, ce point d'entrée servira de base à l'intégration réelle

## Ce qui vivra ici ensuite

- l'audio callback temps réel
- le scan `ADC` des `8` potentiomètres
- l'encodeur et les `4` boutons
- l'affichage OLED réel
- le `MIDI UART DIN`
- la persistance finale sur la cible

## Règle

- le code `core/` reste portable
- la logique de synthé, de preset, de session et d'UI ne doit pas être redéplacée ici
- cette couche doit seulement traduire le hardware réel vers les abstractions d'entrée/sortie du runtime portable
