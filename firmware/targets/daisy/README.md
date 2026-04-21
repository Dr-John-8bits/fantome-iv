# Daisy Target Fantôme IV

Ce dossier est réservé à la future intégration `libDaisy`.

La règle de travail est la suivante :

- le code `core/` doit rester portable
- cette couche ne doit contenir que l'adaptation hardware
- l'audio callback, l'ADC, l'OLED, les boutons, l'encodeur et le MIDI UART vivront ici
- la cible `Daisy` devra réutiliser le splash screen portable de démarrage puis basculer vers l'écran principal

Quand le hardware sera acheté, ce dossier deviendra le point d'entrée firmware réel de la V1.
