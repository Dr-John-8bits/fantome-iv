# Documentation Fantôme IV

Ce document définit comment la documentation du projet doit vivre.

Objectif :

- faire de la documentation la source de vérité du projet
- permettre à d'autres personnes de comprendre, reprendre et fabriquer Fantôme IV
- éviter que les décisions restent dans les messages, la mémoire ou des fichiers annexes non maintenus

## Principe directeur

Fantôme IV suit une logique `documentation-first`.

Cela veut dire :

- une décision produit importante doit être écrite dans la documentation
- une décision hardware importante doit être écrite dans la documentation
- une décision firmware importante doit être écrite dans la documentation
- la phase 1 ne démarre pas tant que la documentation initiale n'est pas jugée assez solide

## Hiérarchie des documents

Ordre de référence du plus stratégique au plus opérationnel :

1. [Fantome-IV-specs.md](./Fantome-IV-specs.md)
   Rôle : source de vérité fonctionnelle et technique de la V1.

2. [DECISIONS.md](./DECISIONS.md)
   Rôle : journal des décisions structurantes prises pendant le projet.

3. [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
   Rôle : référence hardware de la V1, blocs physiques et interfaces.

4. [BOM.md](./BOM.md)
   Rôle : nomenclature matérielle de référence pour le prototype.

5. [ROADMAP.md](./ROADMAP.md)
   Rôle : ordre de travail et jalons du projet.

6. [README.md](./README.md)
   Rôle : porte d'entrée du dépôt.

7. [LISTE-COURSES.md](./LISTE-COURSES.md)
   Rôle : version simple de la BOM pour les achats.

## Règles de maintenance

- si une décision contredit la spec, la spec doit être mise à jour
- si un achat contredit la BOM, la BOM doit être mise à jour
- si une priorité change, la roadmap doit être mise à jour
- si une décision structurante est prise, elle doit aussi être ajoutée à `DECISIONS.md`
- si un point reste incertain, il doit être marqué explicitement comme `à figer`

## Contenu minimal à documenter avant la phase 1

La phase 1 firmware ne doit pas démarrer tant que les points suivants ne sont pas suffisamment stabilisés :

- architecture matérielle retenue
- surface de contrôle retenue
- comportement fonctionnel V1
- blocs hardware principaux décrits
- priorités d'effets et de modulations
- logique MIDI
- stratégie de presets
- nomenclature matérielle initiale
- gouvernance de la documentation

## Définition de "documentation initiale assez solide"

On considère la base documentaire comme suffisamment solide quand :

- les fonctions V1 sont figées à un niveau exploitable
- les ambiguïtés majeures ont disparu
- le hardware principal est décidé
- les blocs hardware critiques sont décrits
- les interfaces utilisateur et MIDI sont décrites
- la BOM couvre le prototype réel
- les décisions structurantes sont consignées

## Cas particulier du MIDI

Pour Fantôme IV V1 :

- l'interface musicale principale est `MIDI In DIN 5 broches`
- l'entrée MIDI doit être `opto-isolée`
- l'USB sert au développement, au flash et au debug
- l'USB n'est pas une exigence d'usage musical de la V1

## Discipline projet

Tant que la documentation n'est pas au niveau attendu, le bon travail n'est pas :

- coder plus vite
- acheter plus de matériel
- multiplier les prototypes

Le bon travail est :

- clarifier
- structurer
- figer
- écrire
