# Audit Phase 0 Fantôme IV

Ce document dresse un état d'avancement de la phase 0 documentaire.

Il ne remplace pas les specs.

Il sert à répondre simplement à la question :

- où en est la base documentaire du projet
- ce qui est déjà solide
- ce qui reste à verrouiller avant l'intégration hardware complète

## Verdict global

La phase 0 est bien avancée.

La base documentaire est désormais cohérente, exploitable et déjà très supérieure à un simple brouillon de projet.

En revanche, la phase 0 n'est pas encore complètement close côté hardware.

Le verrou principal n'est plus le produit, l'UX ni le socle logiciel.

Le verrou principal est maintenant le niveau de précision hardware de fin de cadrage :

- stratégie d'alimentation autonome au-delà du prototype USB
- implantation mécanique finale détaillée
- derniers choix mécaniques et électriques à figer

En parallèle, la base documentaire est désormais assez solide pour ouvrir une phase `1A` logicielle `soft-first` sur Mac.

## 1. Ce qui est solide

### 1.1 Gouvernance documentaire

État :

- solide

Acquis :

- la logique `documentation-first` est actée
- la hiérarchie documentaire est écrite
- la phase 1 est explicitement bloquée tant que la doc initiale n'est pas assez solide
- les décisions structurantes sont consignées dans un journal dédié

Documents concernés :

- [DOCUMENTATION.md](./DOCUMENTATION.md)
- [DECISIONS.md](./DECISIONS.md)
- [ROADMAP.md](./ROADMAP.md)

### 1.2 Vision produit V1

État :

- solide

Acquis :

- plateforme retenue : `Daisy Seed 65MB`
- positionnement V1 clair
- fonctions V1 cadrées
- non-objectifs V1 écrits
- priorité musicale et simplicité de la V1 bien affirmées

Document concerné :

- [Fantome-IV-specs.md](./Fantome-IV-specs.md)

### 1.3 Interface utilisateur

État :

- solide

Acquis :

- surface de contrôle figée
- rôles des `8` potentiomètres définis
- logique du `pot 8` contextuel définie
- comportement de l'encodeur verrouillé
- pages OLED identifiées
- logique de navigation claire
- philosophie de lisibilité bien posée

Document concerné :

- [UI-UX-SPEC.md](./UI-UX-SPEC.md)

### 1.4 Contrat MIDI

État :

- solide

Acquis :

- `MIDI In DIN 5 broches` retenu comme interface musicale principale
- séparation USB dev / MIDI DIN usage musical actée
- canal MIDI sélectionnable
- support `Note On/Off`, `Pitch Bend`, `Mod Wheel`, `Sustain`, `CC`, `Program Change`, `MIDI Clock`
- mapping `Program Change` pour `4 presets`
- première table de `CC` documentée

Document concerné :

- [MIDI-SPEC.md](./MIDI-SPEC.md)

### 1.5 Référence hardware

État :

- solide

Acquis :

- architecture matérielle V1 décrite
- blocs principaux identifiés
- distinction claire entre MIDI, audio, UI, USB et alimentation
- budget de signaux établi
- pin map Daisy figée
- bloc `MIDI In` détaillé au niveau composants et valeurs
- topologie de sortie audio figée
- références exactes de composants critiques fixées
- contraintes de prototype réalistes

Documents concernés :

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [ALIMENTATION-POWER-SPEC.md](./ALIMENTATION-POWER-SPEC.md)
- [MECHANICAL-SPEC.md](./MECHANICAL-SPEC.md)
- [PINMAP-DAISY.md](./PINMAP-DAISY.md)
- [MIDI-IN-HARDWARE-SPEC.md](./MIDI-IN-HARDWARE-SPEC.md)
- [AUDIO-OUT-SPEC.md](./AUDIO-OUT-SPEC.md)
- [COMPONENT-REFERENCES.md](./COMPONENT-REFERENCES.md)
- [SCHEMA-SPEC.md](./SCHEMA-SPEC.md)

### 1.6 BOM et achats

État :

- solide

Acquis :

- BOM prototype rédigée
- séparation entre achats immédiats, utiles et plus tard
- quantités d'interface cohérentes avec les specs
- références d'achat critiques désormais explicites
- liste d'achats simple maintenue en parallèle

Documents concernés :

- [BOM.md](./BOM.md)
- [LISTE-COURSES.md](./LISTE-COURSES.md)
- [COMPONENT-REFERENCES.md](./COMPONENT-REFERENCES.md)

### 1.7 Open source et dépôt

État :

- propre

Acquis :

- README structuré
- licence code en `0BSD`
- licence contenus en `CC0 1.0`
- page web publique simplifiée

Documents concernés :

- [README.md](../README.md)
- [LICENSE](../LICENSE)
- [LICENSE-CONTENT.md](../LICENSE-CONTENT.md)
- [index.html](../index.html)

### 1.8 Base logicielle soft-first

État :

- ouverte

Acquis :

- architecture logicielle de référence écrite
- arborescence `firmware/` posée
- cœur portable en `C++` posé
- cible desktop simple en place
- premiers tests locaux en place

Documents concernés :

- [SOFTWARE-ARCHITECTURE.md](./SOFTWARE-ARCHITECTURE.md)
- [ROADMAP.md](./ROADMAP.md)
- [README.md](../README.md)

## 2. Ce qui reste à verrouiller

### 2.1 Hardware détaillé

À verrouiller avant clôture sereine de la phase 0 :

- stratégie d'alimentation autonome finale
- support mécanique de la Daisy
- derniers détails de façade et d'implantation

### 2.2 Choix mécaniques

À verrouiller :

- implantation mécanique de l'écran
- support mécanique de la Daisy

### 2.3 Niveau de détail UI final

La logique UX est bonne, mais quelques détails restent encore à figer :

- intitulés OLED finaux
- conventions d'abréviation
- détail visuel du soft takeover
- détail exact des écrans système et presets

## 3. Risques restants

Les risques majeurs ne sont plus des risques de vision produit.

Les risques restants sont surtout des risques de translation vers le vrai hardware :

- écart entre la pin map figée et son respect réel au schéma
- choix de module OLED ou d'encodeur peu adaptés mécaniquement
- stratégie d'alimentation autonome encore trop vague
- détails mécaniques encore sous-documentés

## 4. Recommandation

La bonne suite n'est plus d'attendre le hardware avant tout travail.

La bonne suite est maintenant double :

1. continuer le cœur logiciel portable
2. terminer le verrouillage documentaire hardware restant
3. alimenter les tests avec les règles MIDI et UI déjà figées
4. préparer ensuite l'intégration Daisy sur une base saine

La fermeture complète de la phase 0 dépend encore de quelques points hardware, mais elle ne bloque plus le démarrage du socle logiciel.

## 5. Conclusion

Fantôme IV n'est plus au stade de l'idée floue.

Le projet a déjà :

- une vision V1 claire
- une architecture matérielle choisie
- une interface utilisateur cohérente
- un contrat MIDI crédible
- une gouvernance documentaire saine

La phase 0 est donc avancée, mais pas encore soldée côté hardware.

Le passage en phase 1A logicielle ne dépend plus d'un manque de cadrage MIDI, audio ou brochage.

L'ouverture de la future intégration hardware dépend surtout de la fermeture des derniers points mécaniques et d'alimentation.
