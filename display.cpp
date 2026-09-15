#include "display.h"

#include <time.h>
#include <math.h>

#include "config.h"
#include "locations.h"
#include "weather.h"

// ============================================================
// CURRENT LOCATION
// ============================================================
//
// Déclarée dans WE4THER.ino
// Utilisée ici par l'affichage.
//

extern uint8_t currentLocation;

// ============================================================
// TFT
// ============================================================

TFT_eSPI tft =
  TFT_eSPI();

int SCREEN_W = 240;
int SCREEN_H = 320;

// ============================================================
// CLOCK STATE
// ============================================================

int lastDisplayedSecond = -1;
int lastDisplayedMinute = -1;
int lastDisplayedHour   = -1;

// ============================================================
// INIT DISPLAY
// ============================================================

void initDisplay() {

  tft.init();

  tft.setRotation(
    TFT_ROTATION
  );

  SCREEN_W =
    tft.width();

  SCREEN_H =
    tft.height();

  tft.setTextWrap(
    false
  );

  tft.fillScreen(
    COLOR_BG
  );

  Serial.print(
    "TFT WIDTH  : "
  );

  Serial.println(
    SCREEN_W
  );

  Serial.print(
    "TFT HEIGHT : "
  );

  Serial.println(
    SCREEN_H
  );
}

// ============================================================
// FLAG - FRANCE
// ============================================================

void drawFlagFrance(
  int x,
  int y,
  int w,
  int h
) {

  int third =
    w / 3;

  tft.fillRect(
    x,
    y,
    third,
    h,
    0x001F
  );

  tft.fillRect(
    x + third,
    y,
    third,
    h,
    TFT_WHITE
  );

  tft.fillRect(
    x + third * 2,
    y,
    w - third * 2,
    h,
    TFT_RED
  );

  tft.drawRect(
    x,
    y,
    w,
    h,
    COLOR_GREY
  );
}

// ============================================================
// FLAG - MEXICO
// ============================================================

void drawFlagMexico(
  int x,
  int y,
  int w,
  int h
) {

  int third =
    w / 3;

  tft.fillRect(
    x,
    y,
    third,
    h,
    TFT_GREEN
  );

  tft.fillRect(
    x + third,
    y,
    third,
    h,
    TFT_WHITE
  );

  tft.fillRect(
    x + third * 2,
    y,
    w - third * 2,
    h,
    TFT_RED
  );

  tft.drawCircle(
    x + w / 2,
    y + h / 2,
    3,
    0x8410
  );

  tft.drawRect(
    x,
    y,
    w,
    h,
    COLOR_GREY
  );
}

// ============================================================
// CURRENT FLAG
// ============================================================

void drawCurrentFlag() {

  const int flagW = 30;
  const int flagH = 18;

  const int flagX =
    SCREEN_W / 2 - 67;

  const int flagY =
    48;

  if (
    currentLocation == 0
  ) {

    drawFlagFrance(
      flagX,
      flagY,
      flagW,
      flagH
    );

  } else {

    drawFlagMexico(
      flagX,
      flagY,
      flagW,
      flagH
    );
  }
}

// ============================================================
// HEADER
// ============================================================

void drawHeader() {

  tft.fillRect(
    0,
    0,
    SCREEN_W,
    23,
    COLOR_BG
  );

  tft.setTextDatum(
    TC_DATUM
  );

  tft.setTextColor(
    COLOR_PRIMARY,
    COLOR_BG
  );

  tft.drawString(
    "3x0c3t WE4THER",
    SCREEN_W / 2,
    3,
    2
  );

  tft.drawFastHLine(
    8,
    22,
    SCREEN_W - 16,
    COLOR_PRIMARY
  );
}

// ============================================================
// LOCATION
// ============================================================

void drawLocation() {

  tft.setTextDatum(
    TC_DATUM
  );

  tft.setTextColor(
    COLOR_TEXT,
    COLOR_BG
  );

  tft.drawString(
    locations[currentLocation].name,
    SCREEN_W / 2,
    27,
    3
  );

  drawCurrentFlag();

  tft.setTextColor(
    COLOR_SECONDARY,
    COLOR_BG
  );

  tft.drawString(
    locations[currentLocation].country,
    SCREEN_W / 2 + 23,
    51,
    1
  );
}

// ============================================================
// DATE
// ============================================================

void drawDate() {

  time_t now =
    time(nullptr);

  struct tm* localTime =
    localtime(&now);

  if (
    !localTime
  ) {
    return;
  }

  char dateBuffer[32];

  snprintf(
    dateBuffer,
    sizeof(dateBuffer),
    "%02d/%02d/%04d",
    localTime->tm_mday,
    localTime->tm_mon + 1,
    localTime->tm_year + 1900
  );

  tft.setTextDatum(
    TC_DATUM
  );

  tft.setTextColor(
    COLOR_GREY,
    COLOR_BG
  );

  tft.drawString(
    dateBuffer,
    SCREEN_W / 2,
    72,
    2
  );
}

// ============================================================
// CLOCK
// ============================================================

void drawClock() {

  time_t now =
    time(nullptr);

  struct tm* localTime =
    localtime(&now);

  if (
    !localTime
  ) {
    return;
  }

  char timeBuffer[16];

  snprintf(
    timeBuffer,
    sizeof(timeBuffer),
    "%02d:%02d:%02d",
    localTime->tm_hour,
    localTime->tm_min,
    localTime->tm_sec
  );

  tft.setTextDatum(
    TC_DATUM
  );

  tft.setTextColor(
    COLOR_PRIMARY,
    COLOR_BG
  );

  tft.drawString(
    timeBuffer,
    SCREEN_W / 2,
    88,
    3
  );
}

// ============================================================
// WEATHER ICON
// ============================================================

void drawWeatherIcon(
  int x,
  int y,
  int code
) {

  if (
    code == 0
  ) {

    tft.fillCircle(
      x,
      y,
      12,
      TFT_YELLOW
    );

    for (
      int i = 0;
      i < 8;
      i++
    ) {

      float angle =
        i * PI / 4.0;

      int x1 =
        x +
        cos(angle) * 17;

      int y1 =
        y +
        sin(angle) * 17;

      int x2 =
        x +
        cos(angle) * 22;

      int y2 =
        y +
        sin(angle) * 22;

      tft.drawLine(
        x1,
        y1,
        x2,
        y2,
        TFT_YELLOW
      );
    }

    return;
  }

  if (
    code == 1 ||
    code == 2 ||
    code == 3
  ) {

    tft.fillCircle(
      x - 10,
      y + 3,
      8,
      COLOR_GREY
    );

    tft.fillCircle(
      x,
      y - 3,
      11,
      COLOR_GREY
    );

    tft.fillCircle(
      x + 11,
      y + 3,
      8,
      COLOR_GREY
    );

    tft.fillRect(
      x - 17,
      y + 3,
      34,
      9,
      COLOR_GREY
    );

    return;
  }

  if (
    code >= 51 &&
    code <= 67
  ) {

    tft.fillCircle(
      x - 9,
      y - 2,
      8,
      COLOR_GREY
    );

    tft.fillCircle(
      x,
      y - 7,
      10,
      COLOR_GREY
    );

    tft.fillCircle(
      x + 10,
      y - 1,
      7,
      COLOR_GREY
    );

    tft.fillRect(
      x - 14,
      y - 1,
      29,
      9,
      COLOR_GREY
    );

    for (
      int i = -1;
      i <= 1;
      i++
    ) {

      tft.drawLine(
        x + i * 9,
        y + 12,
        x + i * 9 - 3,
        y + 20,
        TFT_CYAN
      );
    }

    return;
  }

  tft.drawCircle(
    x,
    y,
    15,
    COLOR_PRIMARY
  );

  tft.setTextColor(
    COLOR_PRIMARY,
    COLOR_BG
  );

  tft.setTextDatum(
    MC_DATUM
  );

  tft.drawString(
    "?",
    x,
    y,
    2
  );
}

// ============================================================
// WEATHER
// ============================================================

void drawWeather() {

  const int centerX =
    SCREEN_W / 2;

  const int iconY = 125;
  const int tempY = 151;
  const int descY = 178;
  const int windY = 198;

  if (
    weather[currentLocation].valid
  ) {

    drawWeatherIcon(
      centerX,
      iconY,
      weather[currentLocation].weatherCode
    );

  } else {

    tft.setTextDatum(
      TC_DATUM
    );

    tft.setTextColor(
      COLOR_ERROR,
      COLOR_BG
    );

    tft.drawString(
      "NO WEATHER",
      centerX,
      tempY,
      2
    );

    return;
  }

  char temperatureBuffer[24];

  snprintf(
    temperatureBuffer,
    sizeof(temperatureBuffer),
    "%.1f C",
    weather[currentLocation].temperature
  );

  tft.setTextDatum(
    TC_DATUM
  );

  tft.setTextColor(
    COLOR_WARNING,
    COLOR_BG
  );

  tft.drawString(
    temperatureBuffer,
    centerX,
    tempY,
    3
  );

  tft.setTextColor(
    COLOR_SECONDARY,
    COLOR_BG
  );

  tft.drawString(
    getWeatherDescription(
      weather[currentLocation].weatherCode
    ),
    centerX,
    descY,
    2
  );

  char windBuffer[24];

  snprintf(
    windBuffer,
    sizeof(windBuffer),
    "WIND %.1f KM/H",
    weather[currentLocation].windSpeed
  );

  tft.setTextColor(
    COLOR_GREY,
    COLOR_BG
  );

  tft.drawString(
    windBuffer,
    centerX,
    windY,
    1
  );

  tft.drawFastHLine(
    8,
    SCREEN_H - 8,
    SCREEN_W - 16,
    COLOR_DARKGREY
  );
}

// ============================================================
// COMPLETE LOCATION SCREEN
// ============================================================

void drawLocationScreen() {

  // ----------------------------------------------------------
  // Pas de fillScreen()
  // ----------------------------------------------------------

  tft.fillRect(
    0,
    23,
    SCREEN_W,
    SCREEN_H - 23,
    COLOR_BG
  );

  drawLocation();

  drawDate();

  drawClock();

  drawWeather();

  // ----------------------------------------------------------
  // SAVE CLOCK STATE
  // ----------------------------------------------------------

  time_t now =
    time(nullptr);

  struct tm* localTime =
    localtime(&now);

  if (
    localTime
  ) {

    lastDisplayedSecond =
      localTime->tm_sec;

    lastDisplayedMinute =
      localTime->tm_min;

    lastDisplayedHour =
      localTime->tm_hour;
  }
}

// ============================================================
// CLOCK UPDATE
// ============================================================

void updateClockDisplay() {

  time_t now =
    time(nullptr);

  struct tm* localTime =
    localtime(&now);

  if (
    !localTime
  ) {
    return;
  }

  int currentSecond =
    localTime->tm_sec;

  int currentMinute =
    localTime->tm_min;

  int currentHour =
    localTime->tm_hour;

  // ----------------------------------------------------------
  // MINUTE / HEURE
  // ----------------------------------------------------------

  if (
    currentMinute != lastDisplayedMinute ||
    currentHour != lastDisplayedHour
  ) {

    tft.fillRect(
      35,
      68,
      SCREEN_W - 70,
      40,
      COLOR_BG
    );

    drawDate();

    drawClock();

    lastDisplayedMinute =
      currentMinute;

    lastDisplayedHour =
      currentHour;

    lastDisplayedSecond =
      currentSecond;

    return;
  }

  // ----------------------------------------------------------
  // SECONDE
  // ----------------------------------------------------------

  if (
    currentSecond != lastDisplayedSecond
  ) {

    tft.fillRect(
      35,
      84,
      SCREEN_W - 70,
      25,
      COLOR_BG
    );

    drawClock();

    lastDisplayedSecond =
      currentSecond;
  }
}

// ============================================================
// WEATHER DISPLAY UPDATE
// ============================================================

void updateWeatherDisplay() {

  tft.fillRect(
    0,
    110,
    SCREEN_W,
    SCREEN_H - 118,
    COLOR_BG
  );

  drawWeather();
}