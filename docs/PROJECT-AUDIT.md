# Audit Global Fantôme IV

Date d'audit :

- `21 avril 2026`

Ce document dresse un état global du projet au-delà de la seule phase 0 documentaire.

Il couvre :

- l'état du dépôt
- l'état de la documentation
- l'état de la base firmware portable
- les vérifications techniques réellement exécutées
- les points encore ouverts

## Verdict

Le projet est dans un bon état.

La documentation est solide, la structure du dépôt est lisible, et la base logicielle portable est déjà exploitable sans hardware.

À l'issue de cet audit :

- aucun blocage critique n'a été trouvé dans le périmètre logiciel portable
- le build standard du projet refonctionne
- les tests passent
- le smoke test desktop passe
- l'UI portable, la persistance, le session manager et l'affichage OLED simulé fonctionnent ensemble

## 1. Périmètre audité

### 1.1 Dépôt

Audit de :

- l'arborescence du repo
- la séparation entre le vrai projet et les inspirations externes
- l'hygiène des dossiers de build
- la cohérence du dépôt Git

### 1.2 Documentation

Audit de :

- la cohérence du `README`
- la roadmap
- les specs hardware et UI
- l'alignement entre documentation et implémentation actuelle

### 1.3 Code portable

Audit de :

- moteur synthé portable
- MIDI
- allocation de voix
- DSP de base
- modulations
- effets globaux
- état UI
- soft takeover
- persistance preset/session
- OLED portable
- splash screen
- mini navigateur `preset/session`
- session manager
- couche d'input portable

## 2. Vérifications exécutées

Les vérifications suivantes ont été réellement lancées pendant l'audit :

```bash
cmake --fresh -S firmware -B firmware/build
cmake --build firmware/build
./firmware/build/fantome_tests
./firmware/build/fantome_desktop_smoke
```

Résultat :

- `configure` : OK
- `build` : OK
- `tests` : OK
- `desktop smoke test` : OK

## 3. Corrections appliquées pendant l'audit

Les corrections suivantes ont été appliquées au passage :

- réinitialisation propre du dossier `firmware/build` après la réorganisation du repo, afin de restaurer le workflow CMake standard
- maintien du splash screen portable avec retour automatique vers l'écran principal
- finalisation du mini navigateur `preset/session` sur la page `System`
- affichage du statut de session dans l'OLED portable, y compris sur la vue système
- réalignement de la documentation publique avec l'état réel du code

## 4. État actuel par zone

### 4.1 Documentation

État :

- bon

Acquis :

- specs V1 cohérentes
- documentation hardware de référence en place
- documentation UI/UX exploitable
- stratégie logicielle `soft-first` claire
- roadmap lisible

### 4.2 Dépôt

État :

- bon

Acquis :

- repo principal `fantome-iv/` clairement séparé
- dossier `pour-inspiration/` isolé
- arborescence lisible : `docs/`, `firmware/`, `assets/`

Point d'attention :

- les dossiers de build existent logiquement en local mais restent des artefacts de travail, pas une source de vérité

### 4.3 Firmware portable

État :

- avancé

Acquis :

- moteur portable en `C++`
- modèle de patch V1
- banque de `4` presets
- allocation de voix et modes de jeu principaux
- MIDI de base et transport clock
- première voix DSP portable
- chorus, delay, reverb portables
- UI state portable
- soft takeover portable
- persistance preset portable
- persistance de session portable
- session manager portable
- couche d'input portable
- OLED textuel portable
- splash screen
- mini navigateur `preset/session`

### 4.4 Site public

État :

- propre

Acquis :

- page simplifiée
- design clair
- dark mode système
- contenu globalement cohérent avec le projet

## 5. Ce qui fonctionne aujourd'hui

Dans le périmètre audité, on peut considérer comme fonctionnels :

- le build local du cœur portable
- le lancement des tests
- le smoke test desktop
- le rendu audio portable de base
- la logique MIDI documentée dans la V1 portable
- la navigation UI portable
- le splash screen de démarrage
- le navigateur `preset/session`
- la sauvegarde/restauration de session portable

## 6. Ce qui n'est pas encore validé

Les points suivants ne sont pas invalides, mais ils ne sont pas encore validés sur vrai hardware :

- charge CPU réelle sur `Daisy Seed`
- callback audio temps réel réel
- lecture des potentiomètres, boutons et encodeur réels
- affichage OLED réel
- MIDI DIN réel sur la machine
- stockage réel sur la cible finale
- comportement électrique et analogique final

## 7. Risques restants

Les risques principaux ne sont plus des risques de cadrage.

Ils sont désormais surtout :

- qualité sonore finale encore à raffiner
- validation hardware encore absente
- cible `targets/daisy/` encore à démarrer
- stockage/persistance finale sur hardware encore non branchés
- ergonomie finale à confirmer avec la vraie face avant

## 8. Position du projet

Le projet n'est plus au stade de la préparation vague.

Il se situe maintenant ici :

- phase documentaire largement solide
- phase logicielle portable déjà concrète et testée
- hardware final encore non acheté/intégré

On peut donc dire :

- la base open source est crédible
- la base firmware existe réellement
- la prochaine valeur vient surtout de l'intégration `Daisy` et du raffinement DSP/UI

## 9. Feuille de route recommandée

Ordre recommandé à partir de maintenant :

1. finaliser la maturité du cœur portable
2. améliorer la qualité DSP des oscillateurs et du filtre
3. stabiliser les niveaux, la dynamique et les effets
4. préparer une première cible `targets/daisy/`
5. brancher ensuite l'OLED, les entrées physiques et le MIDI DIN sur le vrai hardware
6. valider enfin la machine complète sur prototype

## 10. Conclusion

Fantôme IV est déjà un vrai projet en cours, pas seulement une intention.

La documentation tient, le code portable tient, le build tient, les tests tiennent.

La suite ne demande plus de reposer les fondations.

La suite demande surtout :

- de raffiner
- de brancher au vrai runtime Daisy
- puis de confronter le tout au hardware réel
