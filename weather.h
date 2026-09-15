#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>
#include <time.h>

#define FORECAST_COUNT 5

struct ForecastData {
  char time[6];
  float temperature;
  uint8_t humidity;
  int weatherCode;
  bool valid;
};

struct WeatherData {
  float temperature;
  float feelsLike;
  float pressure;
  float windSpeed;
  uint8_t humidity;
  int weatherCode;

  ForecastData forecast[FORECAST_COUNT];

  bool valid;
};

extern WeatherData weather[];

bool fetchWeather(uint8_t locationIndex);
void updateWeather();

void connectWiFi();
void initTime();
void setLocationTimezone(uint8_t locationIndex);

bool getLocationLocalTime(
  uint8_t locationIndex,
  struct tm* result
);

const char* getWeatherDescription(int code);

#endif