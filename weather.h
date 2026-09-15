#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>

#define FORECAST_COUNT 3

struct ForecastData {
  char time[6];
  float temperature;
  int weatherCode;
  bool valid;
};

struct WeatherData {
  float temperature;
  float feelsLike;
  float pressure;
  float windSpeed;
  int weatherCode;

  ForecastData forecast[FORECAST_COUNT];

  bool valid;
};

extern WeatherData weather[];

void connectWiFi();
void initTime();
void setLocationTimezone(uint8_t locationIndex);

bool fetchWeather(uint8_t locationIndex);
void updateWeather();

const char* getWeatherDescription(int code);

#endif