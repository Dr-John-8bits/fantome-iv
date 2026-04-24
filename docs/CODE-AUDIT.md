# Audit Code Fantôme IV

Date : `24 avril 2026`

Ce document audite l'état réel du code et des fonctions implémentées avant achat du matériel.

## Verdict court

Le cœur logiciel portable est sain et validé localement.

Sont validés :

- compilation CMake standard
- compilation avec warnings traités en erreurs
- compilation avec sanitizers `AddressSanitizer` et `UndefinedBehaviorSanitizer`
- tests fonctionnels
- smoke test desktop
- cible Daisy en mode stub

Ne sont pas encore certifiés :

- firmware réel flashé sur `Daisy Seed`
- callback audio `libDaisy` réel
- lecture réelle des `8` ADC
- encodeur, boutons et LED sur GPIO réels
- OLED réel `SH1106`
- `MIDI DIN` réel via UART
- persistance réelle sur la cible
- charge CPU réelle et bruit audio réel

Conclusion :

- le code portable est suffisamment solide pour justifier le passage au matériel
- on ne peut pas affirmer que le synthé complet fonctionnera du premier coup sans validation sur la vraie `Daisy Seed`
- l'achat du matériel devient maintenant utile, car la prochaine couche de validation dépend du réel

## Commandes exécutées

Validation standard :

```bash
cmake -S firmware -B firmware/build
cmake --build firmware/build
./firmware/build/fantome_tests
./firmware/build/fantome_desktop_smoke
./firmware/build/fantome_daisy_stub
```

Validation sanitizers :

```bash
cmake -S firmware -B firmware/build-asan -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-fsanitize=address,undefined
cmake --build firmware/build-asan
./firmware/build-asan/fantome_tests
./firmware/build-asan/fantome_desktop_smoke
./firmware/build-asan/fantome_daisy_stub
```

Validation warnings stricts :

```bash
cmake -S firmware -B firmware/build-werror -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-Werror
cmake --build firmware/build-werror
./firmware/build-werror/fantome_tests
./firmware/build-werror/fantome_desktop_smoke
./firmware/build-werror/fantome_daisy_stub
```

Résultat :

- toutes les commandes passent
- aucun crash détecté
- aucune erreur mémoire détectée
- aucun comportement indéfini détecté par la passe sanitizer
- aucun warning bloquant détecté par la passe `-Werror`

## Fonctions implémentées

### Moteur musical

- moteur `FantomeEngine`
- banque de `4` slots utilisateur
- patch courant éditable
- dirty state du slot actif
- rendu audio stéréo portable
- allocation de voix
- voice stealing
- modes `Poly`, `Mono`, `Unison`
- legato
- pitch bend
- mod wheel
- sustain
- Program Change
- CC principaux
- MIDI Clock reçu et compté

### DSP

- `4` voix
- `2` oscillateurs par voix
- formes `Sine`, `Triangle`, `Square`, `Saw`, `Noise`
- `PWM`
- hard sync
- fine tune
- VCA ADSR
- filtre AR
- filtre low-pass résonant
- LFO pitch
- LFO filtre
- Sample & Hold filtre
- chorus global stéréo
- delay global stéréo
- reverb globale stéréo
- headroom et soft clipping de sortie
- DC blocker de sortie

### Interface portable

- état UI par pages
- encodeur
- `4` boutons
- `8` potentiomètres
- potard contextuel `K8`
- soft takeover
- actions de slot utilisateur
- sauvegarde session
- restauration session
- écran OLED texte portable
- splash screen `FANTOME IV / by Dr. John.`
- navigateur preset/session
- messages transitoires de confirmation

### Persistance

- sauvegarde de banque de presets
- chargement de banque de presets
- sauvegarde de session complète
- restauration de session complète
- fallback propre si session absente ou corrompue

### Runtime et cible Daisy

- runtime firmware portable
- scan `raw -> cooked` des contrôles
- métriques de rendu audio
- abstractions `ADC`, `OLED`, `MIDI UART`
- parser MIDI UART en stub
- conversion ADC brute en valeurs normalisées
- cible `daisy` compilable en stub
- pinmap Daisy figée en code
- adaptation audio interleavée en stub
- store de session côté cible en stub

## Corrections faites pendant l'audit

Un défaut de logique session a été corrigé :

- avant : tous les `Control Change` MIDI marquaient la session comme modifiée
- maintenant : seuls les `CC` qui modifient réellement un paramètre persistant marquent la session comme modifiée

Les messages de performance suivants ne salissent plus la session :

- `Mod Wheel`
- `Sustain`
- `Pitch Bend`

Les messages suivants continuent de marquer la session comme modifiée :

- `Program Change`
- `CC7` volume
- `CC12` delay feedback
- `CC13` delay mix
- `CC71` resonance
- `CC74` cutoff
- `CC91` reverb mix
- `CC93` chorus depth

Un test dédié couvre maintenant ce comportement.

## Risques restants

### 1. Cible `libDaisy` réelle

Le dossier `targets/daisy` prépare la cible, mais l'implémentation réelle `libDaisy` n'est pas encore un firmware complet.

Le code actuel sait simuler :

- boot cible
- session store
- scan ADC
- MIDI UART
- OLED
- callback audio interleavé

Mais la vraie version matérielle doit encore raccorder :

- `DaisySeed::StartAudio`
- ADC réels
- GPIO réels
- encodeur réel
- OLED I2C réel
- MIDI UART réel
- LED MIDI réelle
- stockage cible réel

### 2. MIDI Clock

Le moteur reçoit et compte les pulses MIDI Clock.

Le tempo de travail est déjà utilisé par les LFO et le delay, mais l'estimation automatique du BPM à partir du timing réel des pulses devra être validée dans le runtime matériel.

### 3. Audio réel

Le moteur est borné et stable dans les tests.

Il reste à valider sur la Daisy :

- niveau de sortie
- bruit
- clipping réel
- latence
- charge CPU
- comportement avec les effets actifs

### 4. Persistance cible

La persistance portable fonctionne sur fichier.

Sur Daisy, le backend réel de persistance doit encore être choisi et validé.

## Go / No-Go matériel

Go pour acheter le matériel si l'objectif est :

- commencer le bring-up réel
- valider la Daisy seule
- valider audio, contrôles, OLED et MIDI par blocs
- raccorder progressivement le runtime portable au vrai hardware

No-Go si l'objectif est :

- acheter seulement quand le firmware complet peut déjà être garanti sur la vraie Daisy
- éviter toute phase de bring-up hardware

Le bon seuil actuel est donc :

- acheter le matériel pour passer à l'intégration réelle
- garder en tête que le premier allumage sera une validation par blocs, pas encore une certitude de synthé complet immédiat

## Prochaine étape technique

Après achat et réception :

1. valider la Daisy seule
2. compiler/flasher un exemple `libDaisy`
3. brancher le callback audio réel
4. valider une sortie audio simple
5. brancher un seul ADC
6. brancher OLED
7. brancher MIDI UART
8. brancher les contrôles restants
9. mesurer CPU et niveaux

La route exacte est décrite dans [INSTALLATION-MATERIEL.md](./INSTALLATION-MATERIEL.md).
