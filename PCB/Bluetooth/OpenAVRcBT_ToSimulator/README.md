# Bluetooth details
Les radios OpenAVRc sont équipées d'un module Bluetooth de type HC-05.
Elles peuvent être configurées soit en 'maître' soit en 'esclave'.
Seule une radio définie en 'maître' peut lancer un scan afin de trouver une autre radio autour d'elle, qui elle, devra être définie en 'esclave'.

L'option Bluetooh permet de connecter deux radios en mode écolage.
Mais il est maintenant aussi possible d'utiliser un simulateur de vol (Realflight ou autre) ou de conduire (Vrc Pro) via ce module Bluetooth.
Deux options sont possibles:
1. Le module Bluetooth côté réception, associé à un Arduino Pro Micro, simule un signal **CPPM**.
2. Le module Bluetooth côté réception, associé à un Arduino Pro Micro, simule un **Joystick**.
 
 Le code Arduino est téléchargeable [ici](https://github.com/Ingwie/OpenAVRc_Dev/blob/V3/PCB/Bluetooth/OpenAVRcBT_ToSimulator/OpenAVRcBT_ToSimulator.ino).

## Utiliser le module réception en mode PPM
 Configurer dans le code OpenAVRcBT_ToSimulator.ino, ligne 79,  **#define MODE PPM**

## Utiliser le module réception en mode Joystick
 Configurer dans le code OpenAVRcBT_ToSimulator.ino, ligne 79,  **#define MODE JOYSTICK**
 
## Réaliser le câblage 
Utiliser le croqui ![suivant](https://github.com/Ingwie/OpenAVRc_Dev/blob/V3/PCB/Bluetooth/OpenAVRcBT_ToSimulator/BTSIM.jpg) .

## Configurer le module réception
1. Décommenter à la ligne 35 **#define AT_INIT** .
2. Compiler et Télécharger le code.
3. Au premier lancement, le Pro Micro devrait configurer en **'Salve'** et en **'115200'**.
4. Commenter à nouveau à la ligne 35  **//#define AT_INIT** .
5. Compiler et Télécharger le code.Le module réception est prét.

## Configurer la radio.
1. Aller dans l'écran Bluetooth de la radio.
2. Configurer en mode 'Master'.
3. Lancer un **'Scan'**, vous devriez voir un **BT/SIM**. Sélectionnez le.
4. Validez **'Auto Connect'**.
5. Créer un modèle est choisir BT/SIM.
6. Eteindre puis redémarrer la radio, celle-ci devrait alors ce connecter au module réception, les leds des deux modules se mettront alors à clignoter toutes les 2 secondes.

Dans tous les cas, démarrer le module réception en premier.


