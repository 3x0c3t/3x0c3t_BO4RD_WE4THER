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
  weather[locationIndex].weatherCode = -1;
  weather[locationIndex].valid = false;

  for (uint8_t j = 0; j < FORECAST_COUNT; j++) {
    weather[locationIndex].forecast[j].time[0] = '\0';
    weather[locationIndex].forecast[j].temperature = 0.0;
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

  // Toujours en UTC.
  // Le calcul Lille / Veracruz est fait nous-memes.
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

  // IMPORTANT :
  // On ne modifie plus TZ ici.
  //
  // L'heure de l'ecran est calculee explicitement
  // a partir de l'heure UTC.
  //
  // Cette fonction est conservee pour compatibilite
  // avec le reste du programme.

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
    //
    // Heure d'ete en septembre :
    // UTC + 2

    return 2 * 3600;
  }

  if (locationIndex == 1) {

    // Veracruz
    //
    // UTC - 6
    // Pas d'heure d'ete.

    return -6 * 3600;
  }

  return 0;
}


// ============================================================
// HEURE LOCALE POUR UNE VILLE
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

  url +=
    "&current="
    "temperature_2m,"
    "apparent_temperature,"
    "pressure_msl,"
    "weather_code,"
    "wind_speed_10m";

  url +=
    "&hourly="
    "temperature_2m,"
    "weather_code";

  url +=
    "&forecast_days=2";

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

  DynamicJsonDocument doc(
    45000
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

  JsonArray codes =
    doc["hourly"]["weather_code"].as<JsonArray>();

  uint8_t found = 0;

  time_t now = time(nullptr);

  for (
    size_t i = 0;
    i < times.size() &&
    found < FORECAST_COUNT;
    i++
  ) {

    const char* timeString =
      times[i];

    if (!timeString) {
      continue;
    }

    int year;
    int month;
    int day;
    int hour;
    int minute;

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

    // Pour Open-Meteo, l'heure est déjà
    // dans le fuseau de la ville.
    //
    // Conversion simplifiée vers UTC
    // pour comparer avec time(nullptr).

    int offset =
      getLocationUTCOffset(
        locationIndex
      );

    time_t forecastUtc =
      mktime(&localTm) - offset;

    if (
      forecastUtc <= now
    ) {
      continue;
    }

    snprintf(
      weather[locationIndex]
        .forecast[found]
        .time,
      sizeof(
        weather[locationIndex]
          .forecast[found]
          .time
      ),
      "%02d:%02d",
      hour,
      minute
    );

    weather[locationIndex]
      .forecast[found]
      .temperature =
        temperatures[i] |
        0.0;

    weather[locationIndex]
      .forecast[found]
      .weatherCode =
        codes[i] |
        -1;

    weather[locationIndex]
      .forecast[found]
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