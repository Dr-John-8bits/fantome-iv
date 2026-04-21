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
- la phase 1A logicielle ne démarre pas tant que la documentation initiale n'est pas jugée assez solide
- l'intégration Daisy ne doit pas aspirer la logique métier hors du cœur portable

## Hiérarchie des documents

Ordre de référence du plus stratégique au plus opérationnel :

1. [Fantome-IV-specs.md](./Fantome-IV-specs.md)
   Rôle : source de vérité fonctionnelle et technique de la V1.

2. [DECISIONS.md](./DECISIONS.md)
   Rôle : journal des décisions structurantes prises pendant le projet.

3. [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
   Rôle : référence hardware de la V1, blocs physiques et interfaces.

4. [ALIMENTATION-POWER-SPEC.md](./ALIMENTATION-POWER-SPEC.md)
   Rôle : stratégie d'alimentation, de rails et de masses.

5. [MECHANICAL-SPEC.md](./MECHANICAL-SPEC.md)
   Rôle : direction mécanique de façade, montage et accessibilité.

6. [PINMAP-DAISY.md](./PINMAP-DAISY.md)
   Rôle : brochage de référence de la Daisy Seed pour Fantôme IV.

7. [MIDI-IN-HARDWARE-SPEC.md](./MIDI-IN-HARDWARE-SPEC.md)
   Rôle : bloc MIDI In détaillé et valeurs retenues.

8. [AUDIO-OUT-SPEC.md](./AUDIO-OUT-SPEC.md)
   Rôle : topologie détaillée de sortie audio.

9. [COMPONENT-REFERENCES.md](./COMPONENT-REFERENCES.md)
   Rôle : références exactes de composants retenues pour le prototype.

10. [SCHEMA-SPEC.md](./SCHEMA-SPEC.md)
   Rôle : traduction de la V1 en schéma à dessiner.

11. [UI-UX-SPEC.md](./UI-UX-SPEC.md)
   Rôle : référence d'interface utilisateur et de comportement de la face avant.

12. [MIDI-SPEC.md](./MIDI-SPEC.md)
   Rôle : contrat MIDI de la V1.

13. [SOFTWARE-ARCHITECTURE.md](./SOFTWARE-ARCHITECTURE.md)
   Rôle : stratégie logicielle `soft-first`, séparation `core / desktop / Daisy`.

14. [BOM.md](./BOM.md)
   Rôle : BOM finale d'achat et référence de commande du prototype.

15. [INSTALLATION-MATERIEL.md](./INSTALLATION-MATERIEL.md)
   Rôle : ordre de lecture et ordre d'assemblage physique.

16. [ROADMAP.md](./ROADMAP.md)
   Rôle : ordre de travail et jalons du projet.

17. [README.md](../README.md)
   Rôle : porte d'entrée du dépôt.

18. [LISTE-COURSES.md](./LISTE-COURSES.md)
   Rôle : version simple de la BOM pour les achats.

## Règles de maintenance

- si une décision contredit la spec, la spec doit être mise à jour
- si un achat contredit la BOM, la BOM doit être mise à jour
- si une priorité change, la roadmap doit être mise à jour
- si une décision structurante est prise, elle doit aussi être ajoutée à `DECISIONS.md`
- si un point reste incertain, il doit être marqué explicitement comme `à figer`

## Contenu minimal à documenter avant la phase 1A logicielle

La phase 1A logicielle ne doit pas démarrer tant que les points suivants ne sont pas suffisamment stabilisés :

- architecture matérielle retenue
- surface de contrôle retenue
- comportement fonctionnel V1
- blocs hardware principaux décrits
- pin map hardware figée
- stratégie d'alimentation suffisamment décrite
- direction mécanique suffisamment décrite
- circuit MIDI In suffisamment spécifié
- sortie audio suffisamment spécifiée
- références exactes des composants critiques
- structure du schéma suffisamment cadrée
- logique UI/UX suffisamment cadrée
- contrat MIDI suffisamment cadré
- priorités d'effets et de modulations
- logique MIDI
- stratégie de presets
- architecture logicielle de référence
- nomenclature matérielle initiale
- gouvernance de la documentation

## Définition de "documentation initiale assez solide"

On considère la base documentaire comme suffisamment solide quand :

- les fonctions V1 sont figées à un niveau exploitable
- les ambiguïtés majeures ont disparu
- le hardware principal est décidé
- les blocs hardware critiques sont décrits
- la stratégie d'alimentation prototype est écrite
- la direction mécanique de référence est écrite
- le brochage Daisy utile est figé
- le MIDI In est décrit à un niveau composant et valeurs
- la sortie audio est décrite à un niveau exploitable
- les références exactes des composants critiques sont connues
- la structure du schéma est décrite
- les interfaces utilisateur et MIDI sont décrites
- la navigation UI/UX est décrite
- le comportement MIDI reçu est décrit
- l'architecture logicielle est cadrée
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

Une fois ce seuil atteint, le bon travail devient aussi :

- coder le cœur portable
- tester localement sur Mac
- préparer l'intégration Daisy sans la précipiter
