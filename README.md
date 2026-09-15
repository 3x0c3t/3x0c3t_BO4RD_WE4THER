# 3x0c3t BO4RD WE4THER

Station météo connectée basée sur **ESP8266 NodeMCU** et écran **TFT ILI9341 2.8" 240×320**.

Le projet affiche les conditions météorologiques actuelles et les prévisions pour plusieurs villes, avec changement automatique ou manuel de la localisation.

## Aperçu de l'écran

L'interface est conçue pour un affichage compact sur écran TFT en orientation paysage.

Elle présente :

* l'identité du projet `-3x0c3t- B04RD`
* le pays et la ville sélectionnée
* la date et l'heure locales
* la température actuelle
* la température ressentie
* la pression atmosphérique
* la vitesse du vent
* les conditions météorologiques
* une représentation graphique de la météo
* les prévisions horaires

### Villes disponibles

* 🇫🇷 **LILLE, FRANCE**
* 🇲🇽 **VERACRUZ, MEXICO**

La ville affichée change automatiquement à intervalle régulier.

Un bouton permet également de changer manuellement de localisation.

## Fonctionnement

L'ESP8266 se connecte au réseau Wi-Fi puis récupère les données météorologiques depuis l'API **Open-Meteo**.

La synchronisation de l'heure est réalisée avec **NTP**.

Les données sont ensuite affichées sur l'écran TFT.

### Actualisation

* Synchronisation horaire par NTP
* Actualisation météo périodique
* Changement automatique de localisation
* Changement manuel avec bouton

## Matériel

### Microcontrôleur

* ESP8266 NodeMCU

### Écran

* TFT 2.8"
* ILI9341
* Résolution physique : `240 × 320`
* Orientation utilisée : paysage
* Résolution logique : `320 × 240`

### Entrées

* Bouton de changement de localisation

## Connexions

### ILI9341

| ILI9341 | ESP8266     |
| ------- | ----------- |
| CS      | D8 / GPIO15 |
| RST     | D0 / GPIO16 |
| DC      | D4 / GPIO2  |
| MOSI    | D7 / GPIO13 |
| MISO    | D6 / GPIO12 |
| SCK     | D5 / GPIO14 |

### Bouton

| Fonction            | ESP8266    |
| ------------------- | ---------- |
| Changement de ville | D3 / GPIO0 |
| Autre borne         | GND        |

Le bouton utilise la résistance `INPUT_PULLUP` interne de l'ESP8266.

## Structure du projet

```text
WE4THER/
├── WE4THER.ino
├── config.h
├── display.cpp
├── display.h
├── locations.h
├── weather.cpp
├── weather.h
└── README.md
```

### Fichiers principaux

**WE4THER.ino**

Programme principal :

* initialisation
* connexion Wi-Fi
* synchronisation NTP
* gestion du bouton
* changement de localisation
* boucle principale
* actualisation météo

**config.h**

Configuration générale du projet :

* Wi-Fi
* broches
* temporisations
* paramètres matériels

**locations.h**

Liste des localisations disponibles :

* nom
* pays
* fuseau horaire
* latitude
* longitude

**weather.h / weather.cpp**

Structures et fonctions liées aux données météorologiques.

**display.h / display.cpp**

Gestion de l'affichage sur le TFT.

## Bibliothèques

Le projet utilise notamment :

* Arduino
* ESP8266WiFi
* ESP8266HTTPClient
* WiFiClientSecure
* ArduinoJson
* TFT_eSPI

## Configuration

Modifier les paramètres Wi-Fi dans `config.h` :

```cpp
#define WIFI_SSID     "VOTRE_WIFI"
#define WIFI_PASSWORD "VOTRE_MOT_DE_PASSE"
```

Les coordonnées et informations des villes sont configurées dans `locations.h`.

## Compilation

Environnement utilisé pour le développement :

* Arduino CLI 1.2.2
* ESP8266 Core 3.1.2
* carte : `esp8266:esp8266:nodemcuv2`

Compilation avec Arduino CLI :

```bash
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 .
```

Téléversement :

```bash
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:nodemcuv2 .
```

Le port série peut varier selon la configuration du système.

## API météo

Les données météorologiques sont récupérées auprès de **Open-Meteo**.

Les informations utilisées comprennent notamment :

* température
* température ressentie
* pression atmosphérique
* vitesse du vent
* code météo
* prévisions horaires

Le projet utilise les coordonnées GPS définies pour chaque localisation.

## Gestion de l'heure

Le système récupère l'heure UTC via NTP.

L'heure affichée est ensuite adaptée à la localisation sélectionnée.

Fuseaux actuellement utilisés :

```text
LILLE     → Europe/Paris
VERACRUZ  → America/Mexico_City
```

## Interface

L'écran utilise une interface pensée pour rester lisible sur une résolution de `320 × 240`.

Organisation générale :

```text
┌──────────────────────────────────────────────────────────────┐
│ -3x0c3t- B04RD                                               │
│ 🇫🇷 FRANCE                         LILLE                      │
│ Date / Heure                                                  │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│                     METEO ACTUELLE                           │
│                                                              │
│      TEMP.       CONDITIONS       PRESSION       VENT        │
│                                                              │
├──────────────────────────────────────────────────────────────┤
│                     METEO A VENIR                            │
│                                                              │
│   HEURE       ICON       TEMP.       CONDITIONS              │
│                                                              │
│   ...                                                        │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

## Évolution prévue

Le projet est destiné à évoluer progressivement.

Fonctionnalités prévues ou en cours d'intégration :

* humidité relative
* prévisions à différentes échéances
* amélioration des icônes météo
* amélioration de l'interface TFT
* gestion plus complète des fuseaux horaires
* ajout éventuel de nouvelles localisations

## Version

**v1.0**

Version initiale du projet `3x0c3t BO4RD WE4THER`.

---

## Auteur

**-3x0c3t-**

Projet personnel de développement embarqué, expérimentation électronique et création numérique.
