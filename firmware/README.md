# Firmware Fantôme IV

Cette arborescence sert à avancer le plus loin possible sur le firmware et le logiciel avant l'achat du hardware final.

## Principe

On sépare volontairement le code en trois couches :

- `core/` : logique portable du synthé, sans dépendance `libDaisy`
- `targets/desktop/` : petit exécutable local pour faire des tests rapides sur Mac
- `targets/daisy/` : futur point d'entrée hardware pour la `Daisy Seed`
- `tests/` : tests locaux des comportements critiques

## Ce que cette base couvre déjà

- modèle de patch V1
- banque de `4` slots utilisateur
- contrat MIDI principal
- allocation de voix
- modes `Poly`, `Mono`, `Unison`
- gestion de `Pitch Bend`, `Mod Wheel`, `Sustain`, `CC`, `Program Change`
- état de transport pour la synchro `MIDI Clock`
- première voix DSP portable
- rendu audio stéréo de base sur Mac
- oscillateurs portables avec `hard sync` et `PWM`
- enveloppes plus musicales et filtre low-pass plus stable
- comportement `Mono / Unison` raffiné avec centrage, largeur et détune
- `LFO` pitch et filtre dans le cœur portable
- `Sample & Hold` filtre dans le cœur portable
- modes `Free / Clock` de base pour les modulations
- `delay` stéréo global portable avec feedback plus sûr
- `chorus` stéréo global portable avec mix plus cohérent
- `reverb` stéréo globale portable avec headroom plus propre
- gain staging global de travail et soft clipping de sortie
- calibration de rendu vérifiée sur plusieurs tailles de blocs audio
- état UI portable
- paramètres éditables par pages
- `soft takeover` portable pour les `8` potentiomètres
- persistance portable de la banque de `4` slots utilisateur
- représentation portable de l'écran OLED page par page
- mini navigateur `preset/session` sur la page système OLED portable
- persistance portable de session de travail
- session manager portable explicite pour le démarrage et l'arrêt
- messages transitoires de confirmation pour `save`, `reload` et `write slot`
- contrôleur portable de splash screen au démarrage
- couche d'input portable pour simuler boutons, encodeur et potards
- runtime firmware portable unifié
- logique d'entrées/sorties hardware abstraites
- scanner matériel portable `raw -> cooked` pour préparer le scan réel des contrôles
- rendu audio portable bloc par bloc avec métriques de sortie
- slots utilisateur de départ plus différenciés musicalement
- parser `MIDI UART` portable de flux d'octets côté stub Daisy
- stub `ADC` capable d'accepter des échantillons de potentiomètres bruts
- adaptateur audio interleavé pour préparer le callback cible
- store de session côté cible avec backend stub fichier
- première cible `targets/daisy/` très mince avec stub compilable
- config cible Daisy figée en code avec pinmap, OLED, MIDI, audio et session
- boot cible via store de session possédé par la plateforme

## Ce que cette base ne fait pas encore

- qualité sonore DSP finale
- calibration fine des slots utilisateur par défaut
- intégration `libDaisy`
- branchement effectif du callback audio `libDaisy`
- UI OLED réelle sur `Daisy`
- persistance réelle sur hardware
- lecture des contrôles physiques réels
- callback audio réel et drivers `OLED / MIDI UART / ADC`

## Build local

Depuis la racine du dépôt :

```bash
cmake -S firmware -B firmware/build
cmake --build firmware/build
./firmware/build/fantome_desktop_smoke
./firmware/build/fantome_tests
./firmware/build/fantome_daisy_stub
```

## Intention

Le but n'est pas de simuler toute la machine dans le vide.

Le but est de sécuriser très tôt :

- l'architecture logicielle
- les structures de données
- le comportement MIDI
- les règles d'allocation de voix
- les points de friction probables avant l'arrivée du hardware
