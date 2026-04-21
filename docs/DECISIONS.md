# Décisions Fantôme IV

Ce fichier consigne les décisions structurantes du projet.

## D-0001 - Documentation first

Statut :

- actée

Décision :

- la documentation est la source de vérité du projet
- la phase 1A logicielle ne démarre pas tant que la documentation initiale n'est pas assez solide

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

- l'écran de travail retenu pour la V1 est un `OLED 128x64` `1,3"` en `I2C`
- le bus retenu par défaut pour le prototype est `I2C`
- le format retenu est `1,3"` pour privilégier la lisibilité

Documents de référence :

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [SCHEMA-SPEC.md](./SCHEMA-SPEC.md)
- [BOM.md](./BOM.md)

Impact :

- la BOM peut être resserrée sur un module `OLED I2C 1,3"`
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
- cette décision préparatoire a ensuite été figée plus précisément par `D-0019`

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

## D-0017 - Module OLED exact retenu

Statut :

- actée

Décision :

- la référence exacte retenue pour l'écran prototype est `BuyDisplay ER-OLEDM013-1W-I2C`
- ce module repose sur un contrôleur `SH1106`
- cette décision remplace l'ancienne hypothèse trop vague d'un simple module `SSD1306 1,3"`

Documents de référence :

- [COMPONENT-REFERENCES.md](./COMPONENT-REFERENCES.md)
- [BOM.md](./BOM.md)
- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)

Impact :

- la doc doit parler d'un `OLED 128x64 1,3" I2C` au niveau générique
- la référence exacte d'achat est désormais figée
- le futur firmware devra viser la référence `SH1106`

## D-0018 - Pin map Daisy retenue

Statut :

- actée

Décision :

- la pin map V1 de la `Daisy Seed` est désormais figée
- `A0` à `A7` sont dédiées aux `8` potentiomètres
- `D11/D12` sont dédiées à l'écran `I2C`
- `D14` est dédiée au `MIDI_RX`
- `D0` à `D7` couvrent encodeur, boutons et LED MIDI

Documents de référence :

- [PINMAP-DAISY.md](./PINMAP-DAISY.md)
- [SCHEMA-SPEC.md](./SCHEMA-SPEC.md)
- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)

Impact :

- le schéma peut être dessiné sans ambiguïté de brochage
- la structure firmware future peut reprendre les mêmes noms de signaux

## D-0019 - Bloc MIDI In exact retenu

Statut :

- actée

Décision :

- le bloc `MIDI In` V1 retient `H11L1M`
- la résistance de boucle retenue est `220R`
- la diode inverse retenue est `1N4148`
- le découplage local retenu est `100nF`
- le pull-up de sortie optocoupleur retenu est `1k` vers `3V3_D`
- le signal va vers `D14 / USART1_RX`

Documents de référence :

- [MIDI-IN-HARDWARE-SPEC.md](./MIDI-IN-HARDWARE-SPEC.md)
- [PINMAP-DAISY.md](./PINMAP-DAISY.md)
- [BOM.md](./BOM.md)

Impact :

- le bloc MIDI In n'est plus une simple intention
- la BOM et le schéma peuvent converger sur une implémentation unique

## D-0020 - Topologie de sortie audio V1 retenue

Statut :

- actée

Décision :

- la sortie audio V1 utilise directement les sorties ligne de la `Daisy Seed`
- `pin 18` va au `tip`
- `pin 19` va au `ring`
- `pin 20 AGND` va au `sleeve`
- aucun étage actif externe n'est ajouté en V1

Documents de référence :

- [AUDIO-OUT-SPEC.md](./AUDIO-OUT-SPEC.md)
- [PINMAP-DAISY.md](./PINMAP-DAISY.md)
- [SCHEMA-SPEC.md](./SCHEMA-SPEC.md)

Impact :

- le prototype reste simple et fidèle à la plateforme Daisy
- la sortie V1 est explicitement ligne, pas casque

## D-0021 - Références exactes de composants de façade retenues

Statut :

- actée

Décision :

- potentiomètre retenu : `Alpha RD901F-40-15F-B10K-00DL1`
- encodeur retenu : `Bourns PEC11R-4225F-S0024`
- bouton retenu : `APEM IPR3FAD7/1`
- jack audio retenu : `Switchcraft 155`
- embase MIDI retenue : `Switchcraft 57GB5FX`

Documents de référence :

- [COMPONENT-REFERENCES.md](./COMPONENT-REFERENCES.md)
- [BOM.md](./BOM.md)

Impact :

- les achats peuvent se faire sur des références explicites
- le schéma et la mécanique peuvent viser des pièces réelles et non des placeholders

## D-0022 - Architecture d'alimentation prototype retenue

Statut :

- actée

Décision :

- le prototype V1 est alimenté uniquement par le `micro-USB` de la `Daisy Seed`
- la `Daisy Seed` reste la seule source de `3,3 V` du prototype
- `+3V3A` est réservé aux potentiomètres
- `+3V3D` alimente OLED, `MIDI In`, encodeur, boutons et `LED MIDI`
- `AGND` et `DGND` doivent être reliées explicitement en un seul point près de la Seed

Documents de référence :

- [ALIMENTATION-POWER-SPEC.md](./ALIMENTATION-POWER-SPEC.md)
- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [PINMAP-DAISY.md](./PINMAP-DAISY.md)

Impact :

- le schéma prototype évite les ambiguïtés de rails
- la discipline de masse est cadrée avant dessin détaillé

## D-0023 - Architecture mécanique de référence retenue

Statut :

- actée

Décision :

- le prototype V1 reste un instrument `desktop`
- les contrôles sont montés sur panneau
- la `Daisy Seed` est montée hors panneau sur support ou carte porteuse
- l'écran reste dans la zone visuelle haute
- les connecteurs d'usage normal sont placés en zone arrière

Documents de référence :

- [MECHANICAL-SPEC.md](./MECHANICAL-SPEC.md)
- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [UI-UX-SPEC.md](./UI-UX-SPEC.md)

Impact :

- la façade reste lisible et maintenable
- le schéma et le câblage peuvent viser une intégration réaliste

## D-0024 - Architecture logicielle soft-first retenue

Statut :

- actée

Décision :

- le développement firmware démarre par un cœur logiciel portable
- ce cœur doit être compilable et testable sur Mac sans `libDaisy`
- l'intégration `Daisy Seed` doit vivre dans une cible séparée
- une cible desktop simple fait partie du socle de travail

Documents de référence :

- [SOFTWARE-ARCHITECTURE.md](./SOFTWARE-ARCHITECTURE.md)
- [ROADMAP.md](./ROADMAP.md)
- [Fantome-IV-specs.md](./Fantome-IV-specs.md)

Impact :

- le projet peut avancer fortement avant achat du hardware
- la logique MIDI, presets et allocation de voix doit vivre dans le cœur portable
- la future couche `Daisy` doit rester fine

## D-0025 - Direction sonore V1 retenue

Statut :

- actée

Décision :

- les oscillateurs V1 doivent viser un rendu propre
- le filtre V1 doit viser un caractère musical
- le chorus V1 doit être large et audible
- la reverb V1 doit rester discrète
- la base logicielle doit partir sur `4` presets d'usine

Documents de référence :

- [Fantome-IV-specs.md](./Fantome-IV-specs.md)
- [SOFTWARE-ARCHITECTURE.md](./SOFTWARE-ARCHITECTURE.md)
- [ROADMAP.md](./ROADMAP.md)

Impact :

- les choix DSP ne devront pas dériver vers un rendu trop clinique ou trop chargé
- le travail logiciel initial doit inclure une banque de presets de départ
