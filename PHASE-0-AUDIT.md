# Audit Phase 0 Fantôme IV

Ce document dresse un état d'avancement de la phase 0 documentaire.

Il ne remplace pas les specs.

Il sert à répondre simplement à la question :

- où en est la base documentaire du projet
- ce qui est déjà solide
- ce qui reste à verrouiller avant la phase 1 firmware

## Verdict global

La phase 0 est bien avancée.

La base documentaire est désormais cohérente, exploitable et déjà très supérieure à un simple brouillon de projet.

En revanche, la phase 0 n'est pas encore complètement close.

Le verrou principal n'est plus le produit ou l'UX.

Le verrou principal est maintenant le niveau de précision hardware de fin de cadrage :

- brochage exact de la Daisy
- références exactes de quelques composants clés
- topologie exacte de la sortie audio
- derniers choix mécaniques et électriques à figer

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

- bonne base

Acquis :

- architecture matérielle V1 décrite
- blocs principaux identifiés
- distinction claire entre MIDI, audio, UI, USB et alimentation
- budget de signaux établi
- contraintes de prototype réalistes

Documents concernés :

- [HARDWARE-REFERENCE.md](./HARDWARE-REFERENCE.md)
- [SCHEMA-SPEC.md](./SCHEMA-SPEC.md)

### 1.6 BOM et achats

État :

- exploitable

Acquis :

- BOM prototype rédigée
- séparation entre achats immédiats, utiles et plus tard
- quantités d'interface cohérentes avec les specs
- liste d'achats simple maintenue en parallèle

Documents concernés :

- [BOM.md](./BOM.md)
- [LISTE-COURSES.md](./LISTE-COURSES.md)

### 1.7 Open source et dépôt

État :

- propre

Acquis :

- README structuré
- licence code en `MIT`
- licence contenus en `CC BY 4.0`
- page web publique simplifiée

Documents concernés :

- [README.md](./README.md)
- [LICENSE](./LICENSE)
- [LICENSE-CONTENT.md](./LICENSE-CONTENT.md)
- [index.html](./index.html)

## 2. Ce qui reste à verrouiller

### 2.1 Hardware détaillé

À verrouiller avant clôture sereine de la phase 0 :

- brochage détaillé de la `Daisy Seed`
- valeurs exactes du circuit `MIDI In`
- topologie exacte de la sortie audio
- stratégie d'alimentation finale et règles de masse associées
- référence finale de l'optocoupleur si différente du `H11L1`
- référence exacte du module `OLED 1,3"`

### 2.2 Choix mécaniques

À verrouiller :

- références exactes des potentiomètres de panneau
- référence exacte de l'encodeur cranté
- référence exacte des boutons de panneau
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

- conflit de broches si le pin mapping est mal figé
- choix de module OLED ou d'encodeur peu adaptés mécaniquement
- étage de sortie audio sous-documenté
- temps perdu si le circuit MIDI In n'est pas normalisé dès maintenant

## 4. Recommandation

La bonne suite n'est pas encore de lancer la phase 1 firmware complète.

La bonne suite est de terminer le verrouillage documentaire sur quatre points :

1. pin map Daisy détaillée
2. bloc MIDI In détaillé
3. bloc sortie audio détaillé
4. conventions OLED et implantation mécanique minimale

Une fois ces points consolidés, la phase 0 pourra être considérée comme suffisamment solide pour ouvrir la phase 1 dans de bonnes conditions.

## 5. Conclusion

Fantôme IV n'est plus au stade de l'idée floue.

Le projet a déjà :

- une vision V1 claire
- une architecture matérielle choisie
- une interface utilisateur cohérente
- un contrat MIDI crédible
- une gouvernance documentaire saine

La phase 0 est donc avancée, mais pas encore soldée.

Le passage en phase 1 doit attendre la fermeture des derniers points hardware concrets.
