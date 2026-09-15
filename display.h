#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

extern int SCREEN_W;
extern int SCREEN_H;

void initDisplay();

void drawLocationScreen(uint8_t locationIndex);

void drawHeader();
void drawLocation(uint8_t locationIndex);
void drawCurrentFlag(uint8_t locationIndex);

void drawDateTime();

void drawWeather(uint8_t locationIndex);
void drawForecast(uint8_t locationIndex);

void updateClockDisplay();
void updateWeatherDisplay(uint8_t locationIndex);

void drawFlagFrance(int x, int y, int w, int h);
void drawFlagMexico(int x, int y, int w, int h);

void drawWeatherIcon(int x, int y, int code);

#endif