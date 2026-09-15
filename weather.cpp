#include "weather.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <time.h>

#include "config.h"
#include "locations.h"

// ============================================================
// WEATHER DATA
// ============================================================

WeatherData weather[LOCATION_COUNT];

// ============================================================
// WIFI
// ============================================================

void connectWiFi() {

  Serial.println();
  Serial.println("========================================");
  Serial.println("WIFI");
  Serial.println("========================================");

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

    delay(250);
    Serial.print(".");
  }

  Serial.println();

  if (
    WiFi.status() == WL_CONNECTED
  ) {

    Serial.println("WiFi OK");

    Serial.print("IP : ");
    Serial.println(
      WiFi.localIP()
    );

  } else {

    Serial.println("WiFi ERROR");
  }
}

// ============================================================
// TIMEZONE
// ============================================================

void setLocationTimezone(
  uint8_t locationIndex
) {

  if (
    locationIndex >= LOCATION_COUNT
  ) {
    return;
  }

  setenv(
    "TZ",
    locations[locationIndex].timezone,
    1
  );

  tzset();

  Serial.print("TIMEZONE -> ");
  Serial.println(
    locations[locationIndex].timezone
  );
}

// ============================================================
// NTP
// ============================================================

void initTime() {

  configTime(
    0,
    0,
    "pool.ntp.org",
    "time.nist.gov"
  );

  Serial.println(
    "Synchronisation NTP..."
  );

  time_t now =
    time(nullptr);

  unsigned long start =
    millis();

  while (
    now < 100000 &&
    millis() - start < 15000UL
  ) {

    delay(250);

    now =
      time(nullptr);

    Serial.print(".");
  }

  Serial.println();

  if (
    now >= 100000
  ) {

    Serial.println(
      "NTP OK"
    );

  } else {

    Serial.println(
      "NTP TIMEOUT"
    );
  }
}

// ============================================================
// WEATHER API
// ============================================================

bool fetchWeather(
  uint8_t index
) {

  if (
    WiFi.status() != WL_CONNECTED
  ) {
    return false;
  }

  String url =
    "https://api.open-meteo.com/v1/forecast?"
    "latitude=" +
    String(
      locations[index].latitude,
      4
    ) +
    "&longitude=" +
    String(
      locations[index].longitude,
      4
    ) +
    "&current="
    "temperature_2m,"
    "apparent_temperature,"
    "weather_code,"
    "wind_speed_10m"
    "&timezone=" +
    String(
      locations[index].timezone
    );

  Serial.println();
  Serial.println(
    "Weather request:"
  );

  Serial.println(url);

  WiFiClientSecure client;

  client.setInsecure();

  HTTPClient http;

  if (
    !http.begin(
      client,
      url
    )
  ) {

    Serial.println(
      "HTTP begin ERROR"
    );

    return false;
  }

  int httpCode =
    http.GET();

  if (
    httpCode != HTTP_CODE_OK
  ) {

    Serial.print(
      "HTTP ERROR : "
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
    8192
  );

  DeserializationError error =
    deserializeJson(
      doc,
      payload
    );

  if (error) {

    Serial.print(
      "JSON ERROR : "
    );

    Serial.println(
      error.c_str()
    );

    return false;
  }

  JsonObject current =
    doc["current"];

  if (
    current.isNull()
  ) {

    Serial.println(
      "Current weather unavailable"
    );

    return false;
  }

  weather[index].temperature =
    current[
      "temperature_2m"
    ] | 0.0;

  weather[index].feelsLike =
    current[
      "apparent_temperature"
    ] | 0.0;

  weather[index].weatherCode =
    current[
      "weather_code"
    ] | 0;

  weather[index].windSpeed =
    current[
      "wind_speed_10m"
    ] | 0.0;

  weather[index].valid =
    true;

  Serial.print(
    locations[index].name
  );

  Serial.print(
    " : "
  );

  Serial.print(
    weather[index].temperature
  );

  Serial.println(
    " C"
  );

  return true;
}

// ============================================================
// WEATHER UPDATE
// ============================================================

void updateWeather() {

  if (
    WiFi.status() != WL_CONNECTED
  ) {

    connectWiFi();

    if (
      WiFi.status() != WL_CONNECTED
    ) {
      return;
    }
  }

  Serial.println();
  Serial.println(
    "========================================"
  );

  Serial.println(
    "WEATHER UPDATE"
  );

  Serial.println(
    "========================================"
  );

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
// WEATHER DESCRIPTION
// ============================================================

const char* getWeatherDescription(
  int code
) {

  if (
    code == 0
  )
    return "CLEAR";

  if (
    code == 1 ||
    code == 2 ||
    code == 3
  )
    return "CLOUDY";

  if (
    code == 45 ||
    code == 48
  )
    return "FOG";

  if (
    code >= 51 &&
    code <= 57
  )
    return "DRIZZLE";

  if (
    code >= 61 &&
    code <= 67
  )
    return "RAIN";

  if (
    code >= 71 &&
    code <= 77
  )
    return "SNOW";

  if (
    code >= 80 &&
    code <= 82
  )
    return "SHOWERS";

  if (
    code >= 85 &&
    code <= 86
  )
    return "SNOW SHOWERS";

  if (
    code >= 95 &&
    code <= 99
  )
    return "STORM";

  return "UNKNOWN";
}