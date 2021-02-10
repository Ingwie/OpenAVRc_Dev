# Bluetooth details
Les radios OpenAVRc sont équipées d'un module Bluetooth de type HC-05.
Elles peuvent être configurées soit en 'maître' soit en 'esclave'.
Seule une radio définie en 'maître' peut lancer un scan afin de trouver une autre radio autour d'elle, qui elle, devra être définie en 'esclave'.

L'option Bluetooh permet de connecter deux radios en mode écolage.
Grâce à cette option, on peut commander la radio à partir d'un joystick USB de type Logiteck 3D Pro.
Cette interface utilise un shiel Arduino de type *USB Host Shield v2.0*.

![ici](https://github.com/Ingwie/OpenAVRc_Dev/blob/V3/PCB/Bluetooth/OpenAVRcBT_JoystickReader/UsbHostShieldv2.0.jpg)     ![ici](https://github.com/Ingwie/OpenAVRc_Dev/blob/V3/PCB/Bluetooth/OpenAVRcBT_JoystickReader/UsbHostShield&Uno.jpg)

 Le code Arduino est téléchargeable [ici](https://github.com/Ingwie/OpenAVRc_Dev/blob/V3/PCB/Bluetooth/OpenAVRcBT_JoystickReader/OpenAVRcBT_JoystickReader.ino).

Deux options sont possibles:
1. L'interface simule un signal **CPPM**.
2. L'interface via le **Bluetooth** génère une trame compatible avec la radio.
 

## Utiliser le module réception en mode PPM
 Configurer dans le code OpenAVRcBT_ToSimulator.ino, ligne 79,  **#define MODE PPM**

## Utiliser le module réception en mode Bluetooth
 Configurer dans le code OpenAVRcBT_ToSimulator.ino, ligne 79,  **#define MODE BLUETOOTH**
 
## Réaliser le câblage 
Utiliser le croqui .

![suivant](https://github.com/Ingwie/OpenAVRc_Dev/blob/V3/PCB/Bluetooth/OpenAVRcBT_ToSimulator/BTSIM.jpg) .

## Configurer le module réception
1. Décommenter à la ligne 35 **#define AT_INIT** .
2. Compiler et Télécharger le code.
3. Au premier lancement, le Pro Micro devrait configurer en **'Slave'** et en **'115200'**.
4. Commenter à nouveau à la ligne 35  **//#define AT_INIT** .
5. Compiler et Télécharger le code.Le module réception est prét.

## Configurer la radio.
1. Aller dans l'écran Bluetooth de la radio.
2. Configurer en mode 'Master'.
3. Lancer un **'Scan'**, vous devriez voir un **BT/SIM**.  Sélectionnez le.

![ainsi](https://github.com/Ingwie/OpenAVRc_Dev/blob/V3/PCB/Bluetooth/OpenAVRcBT_ToSimulator/BTScanResult.jpg)

4. Validez **'Auto Connect'**.
5. Créer un modèle est choisir autre chose que SIM/BT (par exemple **PPM** ou **Frsky-X**).

![ainsi](https://github.com/Ingwie/OpenAVRc_Dev/blob/V3/PCB/Bluetooth/OpenAVRcBT_ToSimulator/model.jpg)

6. Eteindre puis redémarrer la radio, celle-ci devrait alors ce connecter au module réception, les leds des deux modules se mettront alors à clignoter toutes les 2 secondes.

Dans tous les cas, démarrer le module réception en premier.


