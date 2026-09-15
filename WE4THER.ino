#include "weather.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <time.h>

#include "config.h"
#include "locations.h"

WeatherData weather[LOCATION_COUNT];

const uint8_t FORECAST_OFFSETS[FORECAST_COUNT] = {
  1,
  5,
  10,
  24,
  48
};


// ============================================================
// INITIALISATION DES DONNEES
// ============================================================

void clearWeatherData(uint8_t locationIndex) {

  if (locationIndex >= LOCATION_COUNT) {
    return;
  }

  weather[locationIndex].temperature = 0.0;
  weather[locationIndex].feelsLike = 0.0;
  weather[locationIndex].pressure = 0.0;
  weather[locationIndex].windSpeed = 0.0;
  weather[locationIndex].humidity = 0;
  weather[locationIndex].weatherCode = -1;
  weather[locationIndex].valid = false;

  for (uint8_t j = 0; j < FORECAST_COUNT; j++) {

    weather[locationIndex].forecast[j].time[0] = '\0';
    weather[locationIndex].forecast[j].temperature = 0.0;
    weather[locationIndex].forecast[j].humidity = 0;
    weather[locationIndex].forecast[j].weatherCode = -1;
    weather[locationIndex].forecast[j].valid = false;
  }
}


// ============================================================
// WIFI
// ============================================================

void connectWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.println();
  Serial.println("Connexion WiFi...");

  WiFi.mode(WIFI_STA);

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  unsigned long start = millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - start < WIFI_TIMEOUT
  ) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("WiFi connecte");

    Serial.print("IP : ");
    Serial.println(WiFi.localIP());

  } else {

    Serial.println("ERREUR : WiFi non connecte");
  }
}


// ============================================================
// NTP
// ============================================================

void initTime() {

  Serial.println("Synchronisation NTP...");

  configTime(
    0,
    0,
    "pool.ntp.org",
    "time.nist.gov",
    "time.google.com"
  );

  time_t now = time(nullptr);

  unsigned long start = millis();

  while (
    now < 100000 &&
    millis() - start < 30000UL
  ) {

    delay(500);

    now = time(nullptr);

    Serial.print(".");
  }

  Serial.println();

  if (now >= 100000) {

    Serial.println("NTP synchronise");

    Serial.print("UTC epoch : ");
    Serial.println((unsigned long)now);

  } else {

    Serial.println("ERREUR : NTP non synchronise");
  }
}


// ============================================================
// FUSEAU HORAIRE
// ============================================================

void setLocationTimezone(
  uint8_t locationIndex
) {

  if (locationIndex >= LOCATION_COUNT) {
    return;
  }

  Serial.print("Fuseau logique : ");

  Serial.println(
    locations[locationIndex].timezone
  );
}


// ============================================================
// OFFSET LOCAL
// ============================================================

int getLocationUTCOffset(
  uint8_t locationIndex
) {

  if (locationIndex == 0) {

    // Lille
    // Heure d'été : UTC + 2

    return 2 * 3600;
  }

  if (
    locationIndex == 1 ||
    locationIndex == 2
  ) {

    // Veracruz
    // UTC - 6

    return -6 * 3600;
  }

  return 0;
}


// ============================================================
// HEURE LOCALE
// ============================================================

bool getLocationLocalTime(
  uint8_t locationIndex,
  struct tm* result
) {

  if (
    result == nullptr ||
    locationIndex >= LOCATION_COUNT
  ) {
    return false;
  }

  time_t utcNow = time(nullptr);

  if (utcNow < 100000) {
    return false;
  }

  time_t localTimestamp =
    utcNow +
    getLocationUTCOffset(locationIndex);

  struct tm* utcInfo =
    gmtime(&localTimestamp);

  if (!utcInfo) {
    return false;
  }

  *result = *utcInfo;

  return true;
}


// ============================================================
// RECUPERATION METEO
// ============================================================

bool fetchWeather(
  uint8_t locationIndex
) {

  if (
    locationIndex >= LOCATION_COUNT ||
    WiFi.status() != WL_CONNECTED
  ) {
    return false;
  }

  clearWeatherData(locationIndex);

  WiFiClientSecure client;

  client.setInsecure();

  HTTPClient http;

  String url =
    "https://api.open-meteo.com/v1/forecast?";

  url +=
    "latitude=" +
    String(
      locations[locationIndex].latitude,
      4
    );

  url +=
    "&longitude=" +
    String(
      locations[locationIndex].longitude,
      4
    );

  // ==========================================================
  // DONNEES ACTUELLES
  // ==========================================================

  url +=
    "&current="
    "temperature_2m,"
    "apparent_temperature,"
    "relative_humidity_2m,"
    "pressure_msl,"
    "weather_code,"
    "wind_speed_10m";

  // ==========================================================
  // DONNEES HORAIRES
  // ==========================================================

  url +=
    "&hourly="
    "temperature_2m,"
    "relative_humidity_2m,"
    "weather_code";

  // 3 jours pour garantir +48h
  url +=
    "&forecast_days=3";

  url +=
    "&timezone=";

  url +=
    locations[locationIndex].timezone;


  Serial.println();

  Serial.print("METEO : ");

  Serial.println(
    locations[locationIndex].name
  );

  Serial.println(url);


  // ==========================================================
  // REQUETE HTTP
  // ==========================================================

  if (
    !http.begin(
      client,
      url
    )
  ) {

    Serial.println(
      "Erreur HTTP begin"
    );

    return false;
  }

  int httpCode =
    http.GET();

  if (httpCode != HTTP_CODE_OK) {

    Serial.print(
      "Erreur HTTP : "
    );

    Serial.println(
      httpCode
    );

    http.end();

    return false;
  }

  String payload =
    http.getString();

  http.end();


  // ==========================================================
  // JSON
  // ==========================================================

  DynamicJsonDocument doc(
    50000
  );

  DeserializationError error =
    deserializeJson(
      doc,
      payload
    );

  if (error) {

    Serial.print(
      "Erreur JSON : "
    );

    Serial.println(
      error.c_str()
    );

    return false;
  }


  // ==========================================================
  // METEO ACTUELLE
  // ==========================================================

  JsonObject current =
    doc["current"];

  if (current.isNull()) {

    Serial.println(
      "Bloc current absent"
    );

    return false;
  }


  weather[locationIndex].temperature =
    current["temperature_2m"] |
    0.0;

  weather[locationIndex].feelsLike =
    current["apparent_temperature"] |
    0.0;

  weather[locationIndex].humidity =
    current["relative_humidity_2m"] |
    0;

  weather[locationIndex].pressure =
    current["pressure_msl"] |
    0.0;

  weather[locationIndex].weatherCode =
    current["weather_code"] |
    -1;

  weather[locationIndex].windSpeed =
    current["wind_speed_10m"] |
    0.0;

  weather[locationIndex].valid = true;


  // ==========================================================
  // PREVISIONS HORAIRES
  // ==========================================================

  JsonArray times =
    doc["hourly"]["time"].as<JsonArray>();

  JsonArray temperatures =
    doc["hourly"]["temperature_2m"].as<JsonArray>();

  JsonArray humidities =
    doc["hourly"]["relative_humidity_2m"].as<JsonArray>();

  JsonArray codes =
    doc["hourly"]["weather_code"].as<JsonArray>();


  if (
    times.isNull() ||
    temperatures.isNull() ||
    humidities.isNull() ||
    codes.isNull()
  ) {

    Serial.println(
      "Donnees horaires absentes"
    );

    return true;
  }


  // ==========================================================
  // RECHERCHE DE LA PREMIERE HEURE FUTURE
  // ==========================================================

  time_t now = time(nullptr);

  int firstFutureIndex = -1;

  for (
    size_t i = 0;
    i < times.size();
    i++
  ) {

    const char* timeString =
      times[i];

    if (!timeString) {
      continue;
    }

    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;

    if (
      sscanf(
        timeString,
        "%d-%d-%dT%d:%d",
        &year,
        &month,
        &day,
        &hour,
        &minute
      ) != 5
    ) {
      continue;
    }

    struct tm localTm = {};

    localTm.tm_year =
      year - 1900;

    localTm.tm_mon =
      month - 1;

    localTm.tm_mday =
      day;

    localTm.tm_hour =
      hour;

    localTm.tm_min =
      minute;

    localTm.tm_sec = 0;


    int offset =
      getLocationUTCOffset(
        locationIndex
      );

    time_t forecastUtc =
      mktime(&localTm) - offset;


    if (
      forecastUtc > now
    ) {

      firstFutureIndex =
        (int)i;

      break;
    }
  }


  if (firstFutureIndex < 0) {

    Serial.println(
      "Aucune prevision future trouvee"
    );

    return true;
  }


  // ==========================================================
  // +1H / +5H / +10H / +24H / +48H
  // ==========================================================

  uint8_t found = 0;

  for (
    uint8_t f = 0;
    f < FORECAST_COUNT;
    f++
  ) {

    int targetIndex =
      firstFutureIndex +
      FORECAST_OFFSETS[f] -
      1;


    if (
      targetIndex < 0 ||
      targetIndex >= (int)times.size() ||
      targetIndex >= (int)temperatures.size() ||
      targetIndex >= (int)humidities.size() ||
      targetIndex >= (int)codes.size()
    ) {
      continue;
    }


    const char* timeString =
      times[targetIndex];

    if (!timeString) {
      continue;
    }


    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;


    if (
      sscanf(
        timeString,
        "%d-%d-%dT%d:%d",
        &year,
        &month,
        &day,
        &hour,
        &minute
      ) != 5
    ) {
      continue;
    }


    // Heure uniquement : 06 au lieu de 06:00

    snprintf(
      weather[locationIndex]
        .forecast[f]
        .time,

      sizeof(
        weather[locationIndex]
          .forecast[f]
          .time
      ),

      "%02d",

      hour
    );


    weather[locationIndex]
      .forecast[f]
      .temperature =
        temperatures[targetIndex] |
        0.0;


    weather[locationIndex]
      .forecast[f]
      .humidity =
        humidities[targetIndex] |
        0;


    weather[locationIndex]
      .forecast[f]
      .weatherCode =
        codes[targetIndex] |
        -1;


    weather[locationIndex]
      .forecast[f]
      .valid = true;


    found++;
  }


  Serial.print(
    "Previsions trouvees : "
  );

  Serial.println(found);


  return true;
}


// ============================================================
// MISE A JOUR METEO
// ============================================================

void updateWeather() {

  for (
    uint8_t i = 0;
    i < LOCATION_COUNT;
    i++
  ) {

    fetchWeather(i);

    delay(100);
  }
}


// ============================================================
// DESCRIPTION METEO
// ============================================================

const char* getWeatherDescription(
  int code
) {

  if (code == 0) {
    return "CIEL DEGAGE";
  }

  if (
    code == 1 ||
    code == 2 ||
    code == 3
  ) {
    return "NUAGEUX";
  }

  if (
    code == 45 ||
    code == 48
  ) {
    return "BROUILLARD";
  }

  if (
    code >= 51 &&
    code <= 57
  ) {
    return "BRUINE";
  }

  if (
    code >= 61 &&
    code <= 67
  ) {
    return "PLUIE";
  }

  if (
    code >= 71 &&
    code <= 77
  ) {
    return "NEIGE";
  }

  if (
    code >= 80 &&
    code <= 82
  ) {
    return "AVERSES";
  }

  if (
    code == 85 ||
    code == 86
  ) {
    return "AVERSES NEIGE";
  }

  if (
    code >= 95 &&
    code <= 99
  ) {
    return "ORAGE";
  }

  return "INCONNU";
}

// ============================================================
// PROGRAMME PRINCIPAL
// ============================================================

#include "display.h"

uint8_t currentLocation = 0;

unsigned long lastWeatherUpdate = 0;
unsigned long lastLocationSwitch = 0;
unsigned long lastClockUpdate = 0;
unsigned long lastButtonChange = 0;

bool lastButtonState = HIGH;
bool buttonState = HIGH;


// ============================================================
// CHANGEMENT DE VILLE
// ============================================================

void switchLocation() {

  currentLocation++;

  if (currentLocation >= LOCATION_COUNT) {
    currentLocation = 0;
  }

  Serial.println();
  Serial.print("Changement vers : ");
  Serial.println(locations[currentLocation].name);

  setLocationTimezone(currentLocation);

  drawLocationScreen(currentLocation);

  lastLocationSwitch = millis();
}


// ============================================================
// BOUTON
// ============================================================

void handleButton() {

  bool reading = digitalRead(SWITCH_PIN);

  if (reading != lastButtonState) {
    lastButtonChange = millis();
  }

  if ((millis() - lastButtonChange) > DEBOUNCE_DELAY) {

    if (reading != buttonState) {

      buttonState = reading;

      if (buttonState == LOW) {
        switchLocation();
      }
    }
  }

  lastButtonState = reading;
}


// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  delay(200);

  Serial.println();
  Serial.println();
  Serial.println("==============================");
  Serial.println("      -3x0c3t- B04RD");
  Serial.println("         WE4THER");
  Serial.println("==============================");

  pinMode(
    SWITCH_PIN,
    INPUT_PULLUP
  );

  initDisplay();

  connectWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    initTime();
  }

  setLocationTimezone(
    currentLocation
  );

  if (WiFi.status() == WL_CONNECTED) {
    updateWeather();
  }

  drawLocationScreen(
    currentLocation
  );

  lastWeatherUpdate = millis();
  lastLocationSwitch = millis();
  lastClockUpdate = millis();
}


// ============================================================
// LOOP
// ============================================================

void loop() {

  unsigned long now = millis();

  handleButton();


  // ----------------------------------------------------------
  // CHANGEMENT AUTOMATIQUE DE VILLE
  // ----------------------------------------------------------

  if (
    now - lastLocationSwitch >=
    LOCATION_INTERVAL
  ) {

    switchLocation();
  }


  // ----------------------------------------------------------
  // HORLOGE
  // ----------------------------------------------------------

  if (
    now - lastClockUpdate >=
    CLOCK_INTERVAL
  ) {

    lastClockUpdate = now;

    updateClockDisplay(
      currentLocation
    );
  }


  // ----------------------------------------------------------
  // ACTUALISATION METEO
  // ----------------------------------------------------------

  if (
    now - lastWeatherUpdate >=
    WEATHER_INTERVAL
  ) {

    lastWeatherUpdate = now;

    if (WiFi.status() != WL_CONNECTED) {
      connectWiFi();
    }

    if (WiFi.status() == WL_CONNECTED) {

      updateWeather();

      updateWeatherDisplay(
        currentLocation
      );
    }
  }


  delay(5);
}
