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
