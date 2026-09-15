#include "weather.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <time.h>

#include "config.h"
#include "locations.h"

WeatherData weather[LOCATION_COUNT];

static void clearWeatherData(uint8_t locationIndex) {
  weather[locationIndex].temperature = 0.0;
  weather[locationIndex].feelsLike = 0.0;
  weather[locationIndex].pressure = 0.0;
  weather[locationIndex].windSpeed = 0.0;
  weather[locationIndex].weatherCode = -1;
  weather[locationIndex].valid = false;

  for (uint8_t i = 0; i < FORECAST_COUNT; i++) {
    weather[locationIndex].forecast[i].time[0] = '\0';
    weather[locationIndex].forecast[i].temperature = 0.0;
    weather[locationIndex].forecast[i].weatherCode = -1;
    weather[locationIndex].forecast[i].valid = false;
  }
}

void connectWiFi() {
  Serial.println();
  Serial.println("=== WIFI ===");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connexion");

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < WIFI_TIMEOUT) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi OK");
    Serial.print("IP : ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi ECHEC");
  }
}

void initTime() {
  Serial.println("Initialisation NTP...");

  configTime(
    0,
    0,
    "pool.ntp.org",
    "time.nist.gov",
    "time.google.com"
  );

  time_t now = time(nullptr);

  unsigned long start = millis();

  while (now < 100000 && millis() - start < 15000UL) {
    delay(250);
    now = time(nullptr);
  }

  if (now >= 100000) {
    Serial.println("NTP OK");
  } else {
    Serial.println("NTP timeout");
  }
}

void setLocationTimezone(uint8_t locationIndex) {
  if (locationIndex >= LOCATION_COUNT) {
    return;
  }

  setenv("TZ", locations[locationIndex].timezone, 1);
  tzset();

  Serial.print("Timezone : ");
  Serial.println(locations[locationIndex].timezone);
}

bool fetchWeather(uint8_t locationIndex) {
  if (locationIndex >= LOCATION_COUNT) {
    return false;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi non connecte");
    return false;
  }

  clearWeatherData(locationIndex);

  const Location& loc = locations[locationIndex];

  String url =
    "https://api.open-meteo.com/v1/forecast?"
    "latitude=" + String(loc.latitude, 4) +
    "&longitude=" + String(loc.longitude, 4) +
    "&current="
    "temperature_2m,"
    "apparent_temperature,"
    "pressure_msl,"
    "weather_code,"
    "wind_speed_10m"
    "&hourly="
    "temperature_2m,"
    "weather_code"
    "&forecast_days=2"
    "&timezone=" + String(loc.timezone);

  Serial.println();
  Serial.println("=== METEO ===");
  Serial.println(url);

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;

  if (!https.begin(client, url)) {
    Serial.println("HTTP begin ECHEC");
    return false;
  }

  int httpCode = https.GET();

  if (httpCode != HTTP_CODE_OK) {
    Serial.print("HTTP erreur : ");
    Serial.println(httpCode);
    https.end();
    return false;
  }

  String payload = https.getString();
  https.end();

  DynamicJsonDocument doc(45000);

  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print("JSON erreur : ");
    Serial.println(error.c_str());
    return false;
  }

  JsonObject current = doc["current"];

  if (current.isNull()) {
    Serial.println("Current absent");
    return false;
  }

  weather[locationIndex].temperature =
    current["temperature_2m"] | 0.0;

  weather[locationIndex].feelsLike =
    current["apparent_temperature"] | 0.0;

  weather[locationIndex].pressure =
    current["pressure_msl"] | 0.0;

  weather[locationIndex].windSpeed =
    current["wind_speed_10m"] | 0.0;

  weather[locationIndex].weatherCode =
    current["weather_code"] | -1;

  JsonArray hourlyTime = doc["hourly"]["time"];
  JsonArray hourlyTemperature = doc["hourly"]["temperature_2m"];
  JsonArray hourlyCode = doc["hourly"]["weather_code"];

  time_t now = time(nullptr);

  int found = 0;

  for (size_t i = 0;
       i < hourlyTime.size() && found < FORECAST_COUNT;
       i++) {

    const char* timeString = hourlyTime[i];

    if (!timeString) {
      continue;
    }

    struct tm forecastTm = {};

    int year;
    int month;
    int day;
    int hour;
    int minute;

    if (sscanf(
          timeString,
          "%d-%d-%dT%d:%d",
          &year,
          &month,
          &day,
          &hour,
          &minute
        ) != 5) {
      continue;
    }

    forecastTm.tm_year = year - 1900;
    forecastTm.tm_mon = month - 1;
    forecastTm.tm_mday = day;
    forecastTm.tm_hour = hour;
    forecastTm.tm_min = minute;
    forecastTm.tm_sec = 0;

    time_t forecastEpoch = mktime(&forecastTm);

    if (forecastEpoch <= now) {
      continue;
    }

    ForecastData& f =
      weather[locationIndex].forecast[found];

    snprintf(
      f.time,
      sizeof(f.time),
      "%02d:%02d",
      hour,
      minute
    );

    f.temperature =
      hourlyTemperature[i] | 0.0;

    f.weatherCode =
      hourlyCode[i] | -1;

    f.valid = true;

    found++;
  }

  weather[locationIndex].valid = true;

  Serial.println("Meteo OK");

  Serial.print("Temperature : ");
  Serial.println(weather[locationIndex].temperature);

  Serial.print("Pression : ");
  Serial.println(weather[locationIndex].pressure);

  Serial.print("Vent : ");
  Serial.println(weather[locationIndex].windSpeed);

  return true;
}

void updateWeather() {
  for (uint8_t i = 0; i < LOCATION_COUNT; i++) {
    fetchWeather(i);
    delay(100);
  }
}

const char* getWeatherDescription(int code) {

  if (code == 0) {
    return "CIEL DEGAGE";
  }

  if (code == 1 || code == 2 || code == 3) {
    return "NUAGEUX";
  }

  if (code == 45 || code == 48) {
    return "BROUILLARD";
  }

  if (code >= 51 && code <= 57) {
    return "BRUINE";
  }

  if (code >= 61 && code <= 67) {
    return "PLUIE";
  }

  if (code >= 71 && code <= 77) {
    return "NEIGE";
  }

  if (code >= 80 && code <= 82) {
    return "AVERSES";
  }

  if (code == 85 || code == 86) {
    return "AVERSES NEIGE";
  }

  if (code >= 95 && code <= 99) {
    return "ORAGE";
  }

  return "INCONNU";
}