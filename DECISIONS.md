# Décisions Fantôme IV

Ce fichier consigne les décisions structurantes du projet.

## D-0001 - Documentation first

Statut :

- actée

Décision :

- la documentation est la source de vérité du projet
- la phase 1 ne démarre pas tant que la documentation initiale n'est pas assez solide

Documents de référence :

- [DOCUMENTATION.md](./DOCUMENTATION.md)
- [Fantome-IV-specs.md](./Fantome-IV-specs.md)
- [ROADMAP.md](./ROADMAP.md)

## D-0002 - Plateforme matérielle retenue

Statut :

- actée

Décision :

- la plateforme retenue pour Fantôme IV V1 est `Daisy Seed 65MB`

Impact :

- firmware en `C++`
- usage de `libDaisy` et `DaisySP`
- développement et flash via USB depuis le Mac

## D-0003 - Interface musicale MIDI retenue

Statut :

- actée

Décision :

- l'interface MIDI principale de la V1 est `MIDI In DIN 5 broches`
- l'entrée MIDI doit être `opto-isolée`
- l'USB n'est pas l'interface musicale principale

Impact :

- nécessité d'un connecteur DIN 5
- nécessité d'un optocoupleur et du circuit MIDI associé
- besoin de documenter clairement la séparation entre USB de développement et MIDI DIN d'usage

## D-0004 - Surface de contrôle V1 retenue

Statut :

- actée

Décision :

- `8 potentiomètres`
- `1 encodeur rotatif cliquable`
- `4 boutons`
- `1 écran OLED 128x64`

Impact :

- interface pensée par pages
- soft takeover obligatoire après rappel de preset
- hiérarchie claire entre contrôles dédiés et paramètres de menu

## D-0005 - Priorités d'effets V1

Statut :

- actée

Décision :

- priorité 1 : chorus
- priorité 2 : delay
- priorité 3 : reverb

Impact :

- si une simplification DSP est nécessaire, la reverb est la première candidate à être allégée

## D-0006 - Référence hardware obligatoire

Statut :

- actée

Décision :

- un document hardware de référence fait partie de la source de vérité du projet

Documents de référence :

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [BOM.md](./BOM.md)

Impact :

- le schéma devra rester cohérent avec ce document
- les changements matériels devront être répercutés dans la documentation

## D-0007 - Spec de schéma et spec UI/UX obligatoires

Statut :

- actée

Décision :

- la documentation initiale ne sera pas considérée comme suffisante sans une spec de schéma et une spec UI/UX

Documents de référence :

- [SCHEMA-SPEC.md](./SCHEMA-SPEC.md)
- [UI-UX-SPEC.md](./UI-UX-SPEC.md)

Impact :

- le schéma ne devra pas être dessiné sans cadre documentaire
- l'implémentation firmware UI ne devra pas partir sans cadre UX explicite

## D-0008 - Spec MIDI obligatoire

Statut :

- actée

Décision :

- la documentation initiale ne sera pas considérée comme suffisante sans une spec MIDI dédiée

Documents de référence :

- [MIDI-SPEC.md](./MIDI-SPEC.md)

Impact :

- le comportement MIDI reçu doit être figé avant la phase 1
- la table de CC et les Program Change doivent être documentés avant implémentation

## D-0009 - Interaction encodeur retenue

Statut :

- actée

Décision :

- l'encodeur sert à sélectionner un élément puis à entrer en édition
- une fois en édition, la valeur change en direct pendant la rotation
- un nouvel appui sort du mode édition
- les valeurs ordinaires ne demandent pas de validation supplémentaire
- la confirmation explicite est réservée aux actions sensibles comme `Save preset` ou `Init patch`

Documents de référence :

- [UI-UX-SPEC.md](./UI-UX-SPEC.md)

Impact :

- l'encodeur ne doit pas se comporter comme un formulaire à valider à chaque étape
- l'édition des effets, du MIDI et des presets doit rester fluide

## D-0010 - Écran OLED prototype retenu

Statut :

- actée

Décision :

- l'écran de travail retenu pour la V1 est un `OLED 128x64` de type `SSD1306`
- le bus retenu par défaut pour le prototype est `I2C`
- le format retenu est `1,3"` pour privilégier la lisibilité

Documents de référence :

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [SCHEMA-SPEC.md](./SCHEMA-SPEC.md)
- [BOM.md](./BOM.md)

Impact :

- la BOM peut être resserrée sur un module `SSD1306 I2C`
- le schéma peut partir sur une liaison écran simple sans ouvrir une branche `SPI`

## D-0011 - Boutons physiques V1 retenus

Statut :

- actée

Décision :

- les `4` boutons de face avant sont des boutons poussoirs momentanés `NO`
- ils sont distincts du bouton poussoir intégré à l'encodeur
- leurs rôles de base sont `Page -`, `Page +`, `Shift / Back`, `Preset / Save / Action`
- le prototype vise des boutons de panneau plutôt qu'une interface de bench minimale

Documents de référence :

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [SCHEMA-SPEC.md](./SCHEMA-SPEC.md)
- [UI-UX-SPEC.md](./UI-UX-SPEC.md)

Impact :

- le budget GPIO reste clair
- la mécanique de façade peut être pensée autour de `4` poussoirs simples

## D-0012 - Base de travail du bloc MIDI In prototype

Statut :

- actée

Décision :

- le bloc `MIDI In` du prototype part sur une topologie standard opto-isolée
- la référence de départ retenue est un optocoupleur de type `H11L1` ou compatible
- le bloc prévoit un découplage local `100nF`

Documents de référence :

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [SCHEMA-SPEC.md](./SCHEMA-SPEC.md)
- [BOM.md](./BOM.md)

Impact :

- le champ des achats et du schéma se resserre
- la référence exacte pourra encore changer si disponibilité ou validation électrique contraires

## D-0013 - Prototype mécanique proche de la façade finale

Statut :

- actée

Décision :

- le prototype matériel doit être pensé proche de la façade finale
- les potentiomètres sont visés en format panneau
- la façade doit privilégier la lisibilité avant la compacité extrême

Documents de référence :

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [BOM.md](./BOM.md)
- [UI-UX-SPEC.md](./UI-UX-SPEC.md)

Impact :

- la BOM doit viser des composants mécaniquement crédibles pour le produit
- les choix d'implantation deviennent un vrai sujet de phase 0

## D-0014 - Encodeur cranté retenu

Statut :

- actée

Décision :

- l'encodeur de la V1 doit être cranté

Documents de référence :

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [UI-UX-SPEC.md](./UI-UX-SPEC.md)
- [BOM.md](./BOM.md)

Impact :

- navigation plus lisible dans les menus
- la sensation mécanique doit rester cohérente avec un usage en façade

## D-0015 - LED MIDI retenue

Statut :

- actée

Décision :

- la V1 inclut une `LED MIDI` de signalisation

Documents de référence :

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [SCHEMA-SPEC.md](./SCHEMA-SPEC.md)
- [BOM.md](./BOM.md)

Impact :

- un GPIO supplémentaire doit être prévu
- le schéma doit intégrer la LED et sa résistance série

## D-0016 - Alimentation prototype via USB

Statut :

- actée

Décision :

- le prototype initial est alimenté par `USB`

Documents de référence :

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [BOM.md](./BOM.md)

Impact :

- le démarrage matériel est simplifié
- la stratégie d'alimentation finale reste un sujet distinct du prototype
