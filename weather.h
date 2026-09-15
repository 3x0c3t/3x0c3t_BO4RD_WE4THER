#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>

// ============================================================
// WEATHER DATA
// ============================================================

struct WeatherData {

  float temperature;
  float feelsLike;
  float windSpeed;

  int weatherCode;

  bool valid;
};

// ============================================================
// FUNCTIONS
// ============================================================

void connectWiFi();

void initTime();

void setLocationTimezone(
  uint8_t locationIndex
);

bool fetchWeather(
  uint8_t locationIndex
);

void updateWeather();

const char* getWeatherDescription(
  int code
);

extern WeatherData weather[];

#endif