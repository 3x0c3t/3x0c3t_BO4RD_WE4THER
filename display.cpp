#include "display.h"

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <time.h>
#include <math.h>

#include "config.h"
#include "locations.h"
#include "weather.h"

TFT_eSPI tft = TFT_eSPI();

int SCREEN_W = 240;
int SCREEN_H = 320;

// ============================================================
// UTILITAIRES
// ============================================================

void drawBoldString(
  const String& text,
  int x,
  int y,
  int font,
  uint16_t color,
  uint16_t background,
  uint8_t datum = TL_DATUM
) {
  tft.setTextFont(font);
  tft.setTextSize(1);
  tft.setTextDatum(datum);
  tft.setTextColor(color, background);

  // Première passe
  tft.drawString(text, x, y);

  // Deuxième passe décalée d'un pixel pour simuler le gras
  tft.drawString(text, x + 1, y);
}


// ============================================================
// INITIALISATION
// ============================================================

void initDisplay() {

  tft.init();

  tft.setRotation(TFT_ROTATION);

  SCREEN_W = tft.width();
  SCREEN_H = tft.height();

  tft.setTextWrap(false);

  tft.fillScreen(COLOR_BG);
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

  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(
    COLOR_PRIMARY,
    COLOR_BG
  );

  tft.drawString(
    "-3x0c3t- B04RD",
    SCREEN_W / 2,
    2
  );

  tft.drawFastHLine(
    4,
    21,
    SCREEN_W - 8,
    COLOR_PRIMARY
  );
}


// ============================================================
// DRAPEAU FRANCE
// ============================================================

void drawFlagFrance(
  int x,
  int y,
  int w,
  int h
) {

  int part = w / 3;

  tft.fillRect(
    x,
    y,
    part,
    h,
    TFT_BLUE
  );

  tft.fillRect(
    x + part,
    y,
    part,
    h,
    TFT_WHITE
  );

  tft.fillRect(
    x + part * 2,
    y,
    w - part * 2,
    h,
    TFT_RED
  );

  tft.drawRect(
    x,
    y,
    w,
    h,
    COLOR_TEXT
  );
}


// ============================================================
// DRAPEAU MEXIQUE
// ============================================================

void drawFlagMexico(
  int x,
  int y,
  int w,
  int h
) {

  int part = w / 3;

  tft.fillRect(
    x,
    y,
    part,
    h,
    TFT_GREEN
  );

  tft.fillRect(
    x + part,
    y,
    part,
    h,
    TFT_WHITE
  );

  tft.fillRect(
    x + part * 2,
    y,
    w - part * 2,
    h,
    TFT_RED
  );

  // Représentation simplifiée du blason
  tft.fillCircle(
    x + part + part / 2,
    y + h / 2,
    3,
    TFT_DARKGREEN
  );

  tft.drawRect(
    x,
    y,
    w,
    h,
    COLOR_TEXT
  );
}


// ============================================================
// PAYS / VILLE
// ============================================================

void drawLocation(
  uint8_t locationIndex
) {

  if (locationIndex >= LOCATION_COUNT) {
    return;
  }

  // Bloc agrandi vers le bas
  const int yTop = 25;
  const int blockHeight = 43;

  tft.fillRect(
    0,
    yTop,
    SCREEN_W,
    blockHeight,
    COLOR_BG
  );

  // Drapeau plus grand
  const int flagX = 8;
  const int flagY = 30;
  const int flagW = 42;
  const int flagH = 28;

  if (locationIndex == 0) {

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

  // PAYS
  tft.setTextDatum(
    TL_DATUM
  );

  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(
    COLOR_PRIMARY,
    COLOR_BG
  );

  tft.drawString(
    locations[locationIndex].country,
    60,
    29
  );

  // VILLE
  tft.setTextFont(4);
  tft.setTextSize(1);
  tft.setTextColor(
    COLOR_TEXT,
    COLOR_BG
  );

  tft.drawString(
    locations[locationIndex].name,
    60,
    47
  );

  // Ligne basse du bloc
  tft.drawFastHLine(
    4,
    68,
    SCREEN_W - 8,
    COLOR_DARKGREY
  );
}


// ============================================================
// COMPATIBILITE ANCIENNE FONCTION
// ============================================================

void drawLocationScreen(
  uint8_t locationIndex
) {

  tft.fillScreen(
    COLOR_BG
  );

  drawHeader();

  drawLocation(
    locationIndex
  );

  drawDateTime();

  drawWeather(
    locationIndex
  );

  drawForecast(
    locationIndex
  );
}


// ============================================================
// DATE + HEURE
// ============================================================

void drawDateTime() {

  time_t now = time(nullptr);

  if (now < 100000) {
    return;
  }

  struct tm* timeInfo =
    localtime(&now);

  if (!timeInfo) {
    return;
  }

  char dateBuffer[16];
  char timeBuffer[12];

  snprintf(
    dateBuffer,
    sizeof(dateBuffer),
    "%02d/%02d/%04d",
    timeInfo->tm_mday,
    timeInfo->tm_mon + 1,
    timeInfo->tm_year + 1900
  );

  snprintf(
    timeBuffer,
    sizeof(timeBuffer),
    "%02d:%02d:%02d",
    timeInfo->tm_hour,
    timeInfo->tm_min,
    timeInfo->tm_sec
  );

  // ==========================================================
  // BLOC HORLOGE
  // ==========================================================

  const int y = 70;
  const int h = 31;

  // Fond vert
  tft.fillRect(
    4,
    y,
    SCREEN_W - 8,
    h,
    COLOR_SECONDARY
  );

  // Contour
  tft.drawRect(
    4,
    y,
    SCREEN_W - 8,
    h,
    COLOR_SECONDARY
  );

  // Date à gauche
  tft.setTextDatum(
    ML_DATUM
  );

  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(
    TFT_BLACK,
    COLOR_SECONDARY
  );

  tft.drawString(
    dateBuffer,
    12,
    y + h / 2
  );

  // ==========================================================
  // HEURE GRANDE + GRAS
  // ==========================================================

  drawBoldString(
    String(timeBuffer),
    SCREEN_W - 12,
    y + h / 2,
    4,
    TFT_BLACK,
    COLOR_SECONDARY,
    MR_DATUM
  );
}


// ============================================================
// ICÔNES METEO
// ============================================================

void drawWeatherIcon(
  int x,
  int y,
  int code
) {

  // Ciel dégagé
  if (code == 0) {

    tft.fillCircle(
      x,
      y,
      8,
      TFT_YELLOW
    );

    for (int i = 0; i < 8; i++) {

      float a =
        i * PI / 4.0;

      int x1 =
        x + cos(a) * 11;

      int y1 =
        y + sin(a) * 11;

      int x2 =
        x + cos(a) * 14;

      int y2 =
        y + sin(a) * 14;

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

  // Nuages
  if (
    code >= 1 &&
    code <= 3
  ) {

    tft.fillCircle(
      x - 5,
      y + 2,
      6,
      COLOR_GREY
    );

    tft.fillCircle(
      x + 3,
      y - 2,
      8,
      COLOR_GREY
    );

    tft.fillCircle(
      x + 10,
      y + 3,
      5,
      COLOR_GREY
    );

    tft.fillRect(
      x - 5,
      y + 2,
      20,
      7,
      COLOR_GREY
    );

    return;
  }

  // Brouillard
  if (
    code == 45 ||
    code == 48
  ) {

    for (int i = -5; i <= 5; i += 5) {

      tft.drawFastHLine(
        x - 12,
        y + i,
        24,
        COLOR_GREY
      );
    }

    return;
  }

  // Pluie / bruine / averses
  if (
    (code >= 51 && code <= 67) ||
    (code >= 80 && code <= 82)
  ) {

    tft.fillCircle(
      x - 5,
      y - 2,
      6,
      COLOR_GREY
    );

    tft.fillCircle(
      x + 4,
      y - 4,
      7,
      COLOR_GREY
    );

    tft.fillRect(
      x - 7,
      y - 1,
      18,
      7,
      COLOR_GREY
    );

    for (int i = 0; i < 3; i++) {

      int rx =
        x - 7 + i * 7;

      tft.drawLine(
        rx,
        y + 7,
        rx - 2,
        y + 13,
        TFT_CYAN
      );
    }

    return;
  }

  // Neige
  if (
    (code >= 71 && code <= 77) ||
    code == 85 ||
    code == 86
  ) {

    tft.fillCircle(
      x - 5,
      y - 2,
      6,
      COLOR_GREY
    );

    tft.fillCircle(
      x + 4,
      y - 4,
      7,
      COLOR_GREY
    );

    tft.fillRect(
      x - 7,
      y - 1,
      18,
      7,
      COLOR_GREY
    );

    for (int i = 0; i < 3; i++) {

      int sx =
        x - 7 + i * 7;

      tft.fillCircle(
        sx,
        y + 12,
        2,
        TFT_WHITE
      );
    }

    return;
  }

  // Orage
  if (
    code >= 95 &&
    code <= 99
  ) {

    tft.fillCircle(
      x - 5,
      y - 2,
      6,
      COLOR_GREY
    );

    tft.fillCircle(
      x + 4,
      y - 4,
      7,
      COLOR_GREY
    );

    tft.fillRect(
      x - 7,
      y - 1,
      18,
      7,
      COLOR_GREY
    );

    tft.fillTriangle(
      x + 1,
      y + 5,
      x - 4,
      y + 14,
      x + 1,
      y + 12,
      TFT_YELLOW
    );

    tft.fillTriangle(
      x + 1,
      y + 12,
      x + 6,
      y + 4,
      x + 1,
      y + 7,
      TFT_YELLOW
    );

    return;
  }

  // Icône inconnue
  tft.drawCircle(
    x,
    y,
    9,
    COLOR_GREY
  );
}


// ============================================================
// METEO ACTUELLE
// ============================================================

void drawWeather(
  uint8_t locationIndex
) {

  if (locationIndex >= LOCATION_COUNT) {
    return;
  }

  const int yTop = 103;
  const int height = 57;

  tft.fillRect(
    0,
    yTop,
    SCREEN_W,
    height,
    COLOR_BG
  );

  tft.setTextDatum(
    TL_DATUM
  );

  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(
    COLOR_PRIMARY,
    COLOR_BG
  );

  tft.drawString(
    "METEO ACTUELLE",
    6,
    yTop + 1
  );

  if (!weather[locationIndex].valid) {

    tft.setTextColor(
      COLOR_ERROR,
      COLOR_BG
    );

    tft.drawString(
      "DONNEES INDISPONIBLES",
      6,
      yTop + 24
    );

    return;
  }

  // Température
  char tempBuffer[16];

  snprintf(
    tempBuffer,
    sizeof(tempBuffer),
    "%.1f C",
    weather[locationIndex].temperature
  );

  tft.setTextFont(4);
  tft.setTextColor(
    COLOR_TEXT,
    COLOR_BG
  );

  tft.drawString(
    tempBuffer,
    6,
    yTop + 20
  );

  // Icône
  drawWeatherIcon(
    112,
    yTop + 32,
    weather[locationIndex].weatherCode
  );

  // Conditions
  tft.setTextFont(1);
  tft.setTextColor(
    COLOR_TEXT,
    COLOR_BG
  );

  tft.drawString(
    getWeatherDescription(
      weather[locationIndex].weatherCode
    ),
    135,
    yTop + 20
  );

  // Pression
  char pressureBuffer[24];

  snprintf(
    pressureBuffer,
    sizeof(pressureBuffer),
    "%.0f hPa",
    weather[locationIndex].pressure
  );

  tft.drawString(
    pressureBuffer,
    135,
    yTop + 34
  );

  // Vent
  char windBuffer[24];

  snprintf(
    windBuffer,
    sizeof(windBuffer),
    "VENT %.1f km/h",
    weather[locationIndex].windSpeed
  );

  tft.drawString(
    windBuffer,
    6,
    yTop + 43
  );
}


// ============================================================
// PREVISIONS
// ============================================================

void drawForecast(
  uint8_t locationIndex
) {

  if (locationIndex >= LOCATION_COUNT) {
    return;
  }

  const int yTop = 162;

  tft.fillRect(
    0,
    yTop,
    SCREEN_W,
    SCREEN_H - yTop,
    COLOR_BG
  );

  tft.setTextDatum(
    TL_DATUM
  );

  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(
    COLOR_PRIMARY,
    COLOR_BG
  );

  tft.drawString(
    "METEO A VENIR",
    6,
    yTop + 1
  );

  if (!weather[locationIndex].valid) {
    return;
  }

  for (
    uint8_t i = 0;
    i < FORECAST_COUNT;
    i++
  ) {

    int y =
      yTop + 20 + i * 25;

    ForecastData& forecast =
      weather[locationIndex].forecast[i];

    if (!forecast.valid) {
      continue;
    }

    // Séparateur
    tft.drawFastHLine(
      6,
      y - 3,
      SCREEN_W - 12,
      COLOR_DARKGREY
    );

    // Heure
    tft.setTextFont(2);
    tft.setTextColor(
      COLOR_TEXT,
      COLOR_BG
    );

    tft.drawString(
      forecast.time,
      7,
      y
    );

    // Icône
    drawWeatherIcon(
      62,
      y + 6,
      forecast.weatherCode
    );

    // Température
    char tempBuffer[12];

    snprintf(
      tempBuffer,
      sizeof(tempBuffer),
      "%.1f C",
      forecast.temperature
    );

    tft.setTextFont(2);

    tft.drawString(
      tempBuffer,
      90,
      y
    );

    // Description
    tft.setTextFont(1);

    tft.drawString(
      getWeatherDescription(
        forecast.weatherCode
      ),
      150,
      y + 2
    );
  }
}


// ============================================================
// DATE / HEURE : RAFRAICHISSEMENT
// ============================================================

void updateClockDisplay() {

  drawDateTime();
}


// ============================================================
// METEO : RAFRAICHISSEMENT
// ============================================================

void updateWeatherDisplay(
  uint8_t locationIndex
) {

  drawWeather(
    locationIndex
  );

  drawForecast(
    locationIndex
  );
}


// ============================================================
// FONCTION COMPATIBLE AVEC LE HEADER
// ============================================================

void drawCurrentFlag(
  uint8_t locationIndex
) {

  if (locationIndex >= LOCATION_COUNT) {
    return;
  }

  if (locationIndex == 0) {

    drawFlagFrance(
      8,
      30,
      42,
      28
    );

  } else {

    drawFlagMexico(
      8,
      30,
      42,
      28
    );
  }
}