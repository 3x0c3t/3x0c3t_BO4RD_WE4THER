#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// DISPLAY
// ============================================================

extern TFT_eSPI tft;

extern int SCREEN_W;
extern int SCREEN_H;

// ============================================================
// DISPLAY FUNCTIONS
// ============================================================

void initDisplay();

void drawHeader();

void drawLocation();

void drawCurrentFlag();

void drawDate();

void drawClock();

void drawWeather();

void drawLocationScreen();

void updateClockDisplay();

void updateWeatherDisplay();

void drawFlagFrance(
  int x,
  int y,
  int w,
  int h
);

void drawFlagMexico(
  int x,
  int y,
  int w,
  int h
);

void drawWeatherIcon(
  int x,
  int y,
  int code
);

#endif