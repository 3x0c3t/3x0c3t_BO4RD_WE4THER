## Interface

L'écran utilise une interface dédiée au TFT **ILI9341 2.8"**, avec une résolution logique de **320 × 240 pixels** en orientation paysage.

L'interface est conçue pour afficher les informations essentielles en permanence, tout en conservant une lecture claire sur une petite surface.

### Aperçu de l'écran

![Aperçu de l'écran](docs/interface.png)

> **Aperçu de l'interface TFT 320 × 240**
>
> La capture ci-dessus présente le rendu réel de l'interface sur l'écran du projet.

### Rendu général

```text
┌──────────────────────────────────────────────────────────────────────┐
│ -3x0c3t- B04RD                                      [🇫🇷] FRANCE     │
│                                                          LILLE       │
│ Date : 15/09/2026                              Heure : 05:30:00      │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│                         METEO ACTUELLE                               │
│                                                                      │
│   TEMPÉRATURE        CONDITIONS       PRESSION        VENT           │
│                                                                      │
│      18.4 °C         CIEL CLAIR       1018 hPa      8.4 km/h        │
│                                                                      │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│                         METEO A VENIR                                │
│                                                                      │
│    HEURE        ICON       TEMP.       CONDITIONS                    │
│                                                                      │
│     06           ☀        18.9 °C      CIEL CLAIR                    │
│     10           ☁        21.4 °C      NUAGEUX                      │
│     15           🌧       20.2 °C      PLUIE                        │
│     04           ☁        17.3 °C      NUAGEUX                      │
│     04           🌧       16.8 °C      PLUIE                        │
│                                                                      │
└──────────────────────────────────────────────────────────────────────┘
```

> Le contenu et les valeurs affichées varient naturellement selon la localisation, l'heure et les données météorologiques reçues.

### Organisation de l'affichage

L'écran est organisé en plusieurs zones horizontales.

#### 1. En-tête

La partie supérieure identifie le système et la localisation active.

Elle contient :

* `-3x0c3t- B04RD`
* le pays
* la ville actuellement sélectionnée
* la date locale
* l'heure locale

Exemple :

```text
-3x0c3t- B04RD

🇫🇷 FRANCE
LILLE

15/09/2026
05:30:00
```

La localisation affichée est automatiquement adaptée à la ville sélectionnée.

---

#### 2. Météo actuelle

La zone centrale présente les conditions météorologiques actuelles.

Elle regroupe notamment :

| Information | Description                             |
| ----------- | --------------------------------------- |
| Température | Température actuelle de l'air           |
| Ressenti    | Température ressentie                   |
| Conditions  | Description correspondant au code météo |
| Pression    | Pression atmosphérique                  |
| Vent        | Vitesse du vent                         |
| Icône       | Représentation graphique des conditions |

Exemple :

```text
METEO ACTUELLE

18.4 °C

☀

CIEL CLAIR

1018 hPa
VENT 8.4 km/h
```

Cette zone constitue le résumé principal de la situation météorologique de la localisation sélectionnée.

---

#### 3. Prévisions

La partie inférieure de l'écran est consacrée aux prévisions météorologiques.

Elle est regroupée sous le titre :

```text
METEO A VENIR
```

Chaque ligne présente une échéance, une représentation graphique, une température et une description des conditions.

Format général :

```text
HEURE    ICON    TEMP.    CONDITIONS
```

Exemple :

```text
06       ☀       18.9 °C  CIEL CLAIR
10       ☁       21.4 °C  NUAGEUX
15       🌧      20.2 °C  PLUIE
04       ☁       17.3 °C  NUAGEUX
04       🌧      16.8 °C  PLUIE
```

### Échéances météorologiques

Les prévisions sont organisées autour de plusieurs échéances horaires.

Le principe utilisé par le projet est :

```text
+1 heure
+5 heures
+10 heures
+24 heures
+48 heures
```

L'heure affichée est volontairement compacte :

```text
06
```

et non :

```text
06:00
```

Cette présentation permet de conserver davantage d'espace horizontal pour les autres informations.

### Localisation

Deux localisations sont actuellement configurées :

```text
🇫🇷 LILLE
    FRANCE
    Europe/Paris
    50.6292, 3.0573

🇲🇽 VERACRUZ
    MEXICO
    America/Mexico_City
    19.1738, -96.1342
```

La localisation active peut changer de deux manières.

#### Changement automatique

L'écran passe automatiquement d'une localisation à l'autre après l'intervalle défini dans la configuration du projet.

```text
LILLE
  ↓
VERACRUZ
  ↓
LILLE
  ↓
...
```

#### Changement manuel

Le bouton connecté sur **D3 / GPIO0** permet de passer immédiatement à la localisation suivante.

```text
Appui bouton

LILLE
  ↓
VERACRUZ
```

### Horloge

L'heure est synchronisée par **NTP**.

Le système récupère une référence UTC puis applique la logique de localisation utilisée par le projet pour afficher l'heure correspondante.

Exemple :

```text
LILLE
Europe/Paris

VERACRUZ
America/Mexico_City
```

L'heure affichée correspond donc à la localisation actuellement sélectionnée.

### Mise à jour météo

Les données météorologiques sont récupérées périodiquement depuis l'API Open-Meteo.

Le cycle général est :

```text
ESP8266
   │
   ├── Connexion Wi-Fi
   │
   ├── Synchronisation NTP
   │
   ├── Requête météo
   │
   ├── Réception des données
   │
   ├── Traitement des prévisions
   │
   └── Affichage TFT
```

Les données sont conservées séparément pour chaque localisation afin que le changement de ville n'efface pas les informations déjà récupérées pour l'autre localisation.

### Résolution et orientation

Le contrôleur ILI9341 possède une résolution physique de :

```text
240 × 320 pixels
```

Le projet utilise l'écran en orientation paysage :

```text
320 × 240 pixels
```

La totalité de l'interface est donc pensée autour de cette surface disponible.

```text
320 px
┌──────────────────────────────────────────────────────────────┐
│                                                              │
│                                                              │
│                         240 px                               │
│                                                              │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

### Philosophie de l'interface

L'interface privilégie :

* la lisibilité
* une hiérarchie visuelle simple
* l'utilisation maximale de la surface TFT
* des informations immédiatement accessibles
* des textes courts
* une actualisation sans reconstruction inutile de l'ensemble de l'écran
* une présentation adaptée à un affichage embarqué

L'objectif n'est pas de reproduire une application météo mobile, mais de construire une **interface météo dédiée à un petit écran embarqué**.

### Évolution de l'interface

L'interface est destinée à évoluer avec le projet.

Les évolutions envisagées comprennent notamment :

* affichage de l'humidité relative
* affichage de l'humidité dans les prévisions
* amélioration des icônes météo
* amélioration de la hiérarchie des informations
* gestion plus complète des fuseaux horaires
* ajout de nouvelles localisations
* optimisation de l'utilisation des 320 × 240 pixels

---
