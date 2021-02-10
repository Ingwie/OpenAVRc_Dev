# Bluetooth details
Les radios OpenAVRc sont équipées d'un module Bluetooth de type HC-05.
Elles peuvent être configurées soit en 'maître' soit en 'esclave'.
Seule une radio définie en 'maître' peut lancer un scan afin de trouver une autre radio autour d'elle, qui elle, devra être définie en 'esclave'.

L'option Bluetooh permet de connecter deux radios en mode écolage.
Grâce à cette option, on peut commander deux servos via un signal PPM ou via le bluetooth.
Ces servos peuvent alors arienter une caméra en direction et azimute.

Cette interface utilise un Pro Micro 5v/16Mhz et un HC-05.

Le code Arduino est téléchargeable [ici](https://github.com/Ingwie/OpenAVRc_Dev/tree/V3/PCB/Bluetooth/OpenAVRcBT_HeadTracker_GY85).

Deux options sont possibles:
1. L'interface simule un signal **CPPM**.
2. L'interface via le **Bluetooth** génère une trame compatible avec la radio.
 

## Utiliser le module réception en mode PPM
 Configurer dans le code OpenAVRcBT_HeadTracker_GY85.ino, ligne 79,  **#define MODE PPM**

## Utiliser le module réception en mode Bluetooth
 Configurer dans le code OpenAVRcBT_HeadTracker_GY85.ino, ligne 79,  **#define MODE BLUETOOTH**
 
## Réaliser le câblage 
Utiliser le croqui:

![ainsi](https://github.com/Ingwie/OpenAVRc_Dev/blob/V3/PCB/Bluetooth/OpenAVRcBT_HeadTracker_GY85/BTHeadtracker.jpg)

## Configurer le module réception
1. Décommenter à la ligne 35 **#define AT_INIT** .
2. Compiler et Télécharger le code.
3. Au premier lancement, le Uno devrait configurer en **'Slave'** et en **'57600'**, (ou en **115200** pour un board Leonardo)
4. Commenter à nouveau à la ligne 35  **//#define AT_INIT** .
5. Compiler et Télécharger le code.Le module réception est prét.

## Configurer la radio.
1. Aller dans l'écran Bluetooth de la radio.
2. Configurer en mode 'Master'.
3. Lancer un **'Scan'**, vous devriez voir un **BT/SIM**.  Sélectionnez le.

![ainsi](https://github.com/Ingwie/OpenAVRc_Dev/blob/V3/PCB/Bluetooth/OpenAVRcBT_HeadTracker_GY85/BTScanResult.jpg)

4. Validez **'Auto Connect'**.
5. Créer un modèle est choisir autre chose que SIM/BT (par exemple **PPM** ou **Frsky-X**).

![ainsi](https://github.com/Ingwie/OpenAVRc_Dev/blob/V3/PCB/Bluetooth/OpenAVRcBT_HeadTracker_GY85/model.jpg)

6. Eteindre puis redémarrer la radio, celle-ci devrait alors ce connecter au module réception, les leds des deux modules se mettront alors à clignoter toutes les 2 secondes.

Dans tous les cas, démarrer le module réception en premier.




