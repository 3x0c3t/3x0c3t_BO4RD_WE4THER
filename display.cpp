#include "display.h"

#include <time.h>

#include "config.h"
#include "locations.h"
#include "weather.h"

TFT_eSPI tft = TFT_eSPI();

int SCREEN_W = 240;
int SCREEN_H = 320;

static void printCentered(
  const String& text,
  int y,
  int font,
  uint16_t color
) {
  tft.setTextFont(font);
  tft.setTextColor(color, COLOR_BG);
  tft.setTextDatum(TC_DATUM);

  tft.drawString(
    text,
    SCREEN_W / 2,
    y
  );
}

void initDisplay() {

  tft.init();

  tft.setRotation(TFT_ROTATION);

  SCREEN_W = tft.width();
  SCREEN_H = tft.height();

  tft.setTextWrap(false);

  tft.fillScreen(COLOR_BG);
}

void drawHeader() {

  tft.fillRect(
    0,
    0,
    SCREEN_W,
    22,
    COLOR_BG
  );

  printCentered(
    "-3x0c3t- B04RD",
    2,
    2,
    COLOR_PRIMARY
  );

  tft.drawFastHLine(
    0,
    21,
    SCREEN_W,
    COLOR_DARKGREY
  );
}

void drawFlagFrance(
  int x,
  int y,
  int w,
  int h
) {
  int third = w / 3;

  tft.fillRect(
    x,
    y,
    third,
    h,
    TFT_BLUE
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
    COLOR_TEXT
  );
}

void drawFlagMexico(
  int x,
  int y,
  int w,
  int h
) {
  int third = w / 3;

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

  tft.fillCircle(
    x + w / 2,
    y + h / 2,
    3,
    TFT_GREEN
  );

  tft.drawRect(
    x,
    y,
    w,
    h,
    COLOR_TEXT
  );
}

void drawCurrentFlag(uint8_t locationIndex) {

  if (locationIndex == 0) {
    drawFlagFrance(
      8,
      30,
      34,
      22
    );
  } else {
    drawFlagMexico(
      8,
      30,
      34,
      22
    );
  }
}

void drawLocation(uint8_t locationIndex) {

  if (locationIndex >= LOCATION_COUNT) {
    return;
  }

  drawCurrentFlag(locationIndex);

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(
    COLOR_TEXT,
    COLOR_BG
  );

  tft.drawString(
    locations[locationIndex].country,
    50,
    29
  );

  tft.setTextColor(
    COLOR_PRIMARY,
    COLOR_BG
  );

  tft.drawString(
    locations[locationIndex].name,
    50,
    44
  );

  tft.drawFastHLine(
    0,
    56,
    SCREEN_W,
    COLOR_DARKGREY
  );
}

void drawDateTime() {

  time_t now = time(nullptr);

  struct tm* local = localtime(&now);

  if (!local) {
    return;
  }

  char dateBuffer[16];
  char timeBuffer[16];

  snprintf(
    dateBuffer,
    sizeof(dateBuffer),
    "%02d/%02d/%04d",
    local->tm_mday,
    local->tm_mon + 1,
    local->tm_year + 1900
  );

  snprintf(
    timeBuffer,
    sizeof(timeBuffer),
    "%02d:%02d:%02d",
    local->tm_hour,
    local->tm_min,
    local->tm_sec
  );

  tft.fillRect(
    0,
    58,
    SCREEN_W,
    23,
    COLOR_BG
  );

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);

  tft.setTextColor(
    COLOR_TEXT,
    COLOR_BG
  );

  tft.drawString(
    dateBuffer,
    8,
    61
  );

  tft.setTextDatum(TR_DATUM);

  tft.setTextColor(
    COLOR_PRIMARY,
    COLOR_BG
  );

  tft.drawString(
    timeBuffer,
    SCREEN_W - 8,
    61
  );

  tft.drawFastHLine(
    0,
    82,
    SCREEN_W,
    COLOR_DARKGREY
  );
}

void drawWeatherIcon(
  int x,
  int y,
  int code
) {

  if (code == 0) {

    tft.fillCircle(
      x,
      y,
      7,
      TFT_YELLOW
    );

    for (int i = 0; i < 8; i++) {

      float angle =
        i * 3.14159 / 4.0;

      int x1 =
        x + cos(angle) * 10;

      int y1 =
        y + sin(angle) * 10;

      int x2 =
        x + cos(angle) * 14;

      int y2 =
        y + sin(angle) * 14;

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

  if (code >= 61 ||
      (code >= 80 && code <= 82) ||
      (code >= 95 && code <= 99)) {

    tft.fillCircle(
      x - 5,
      y,
      7,
      COLOR_GREY
    );

    tft.fillCircle(
      x + 4,
      y,
      9,
      COLOR_GREY
    );

    tft.fillRect(
      x - 10,
      y,
      20,
      7,
      COLOR_GREY
    );

    tft.drawLine(
      x - 6,
      y + 10,
      x - 8,
      y + 15,
      TFT_CYAN
    );

    tft.drawLine(
      x,
      y + 10,
      x - 2,
      y + 15,
      TFT_CYAN
    );

    tft.drawLine(
      x + 6,
      y + 10,
      x + 4,
      y + 15,
      TFT_CYAN
    );

    return;
  }

  if (code >= 71 && code <= 77) {

    tft.fillCircle(
      x - 5,
      y,
      7,
      COLOR_GREY
    );

    tft.fillCircle(
      x + 4,
      y,
      8,
      COLOR_GREY
    );

    for (int i = -1; i <= 1; i++) {

      tft.drawLine(
        x + i * 7,
        y + 9,
        x + i * 7,
        y + 14,
        TFT_WHITE
      );
    }

    return;
  }

  tft.fillCircle(
    x - 5,
    y,
    7,
    COLOR_GREY
  );

  tft.fillCircle(
    x + 4,
    y,
    8,
    COLOR_GREY
  );

  tft.fillRect(
    x - 10,
    y,
    20,
    7,
    COLOR_GREY
  );
}

void drawWeather(
  uint8_t locationIndex
) {

  if (locationIndex >= LOCATION_COUNT) {
    return;
  }

  tft.fillRect(
    0,
    84,
    SCREEN_W,
    78,
    COLOR_BG
  );

  tft.setTextDatum(TL_DATUM);

  tft.setTextFont(2);

  tft.setTextColor(
    COLOR_SECONDARY,
    COLOR_BG
  );

  tft.drawString(
    "METEO ACTUELLE",
    8,
    86
  );

  if (!weather[locationIndex].valid) {

    tft.setTextColor(
      COLOR_WARNING,
      COLOR_BG
    );

    tft.drawString(
      "DONNEES INDISPONIBLES",
      8,
      108
    );

    return;
  }

  drawWeatherIcon(
    25,
    125,
    weather[locationIndex].weatherCode
  );

  tft.setTextFont(4);

  tft.setTextColor(
    COLOR_TEXT,
    COLOR_BG
  );

  String temp =
    String(
      weather[locationIndex].temperature,
      1
    );

  temp += " C";

  tft.drawString(
    temp,
    50,
    112
  );

  tft.setTextFont(2);

  tft.setTextColor(
    COLOR_PRIMARY,
    COLOR_BG
  );

  String pressure =
    "P: " +
    String(
      weather[locationIndex].pressure,
      0
    ) +
    " hPa";

  tft.drawString(
    pressure,
    155,
    112
  );

  tft.setTextColor(
    COLOR_TEXT,
    COLOR_BG
  );

  tft.drawString(
    getWeatherDescription(
      weather[locationIndex].weatherCode
    ),
    50,
    137
  );

  String wind =
    "Vent " +
    String(
      weather[locationIndex].windSpeed,
      0
    ) +
    " km/h";

  tft.drawString(
    wind,
    155,
    137
  );

  tft.drawFastHLine(
    0,
    160,
    SCREEN_W,
    COLOR_DARKGREY
  );
}

void drawForecast(
  uint8_t locationIndex
) {

  if (locationIndex >= LOCATION_COUNT) {
    return;
  }

  tft.fillRect(
    0,
    162,
    SCREEN_W,
    SCREEN_H - 162,
    COLOR_BG
  );

  tft.setTextDatum(TL_DATUM);

  tft.setTextFont(2);

  tft.setTextColor(
    COLOR_SECONDARY,
    COLOR_BG
  );

  tft.drawString(
    "METEO A VENIR",
    8,
    164
  );

  int y = 187;

  for (uint8_t i = 0;
       i < FORECAST_COUNT;
       i++) {

    ForecastData& f =
      weather[locationIndex].forecast[i];

    if (!f.valid) {
      continue;
    }

    tft.setTextColor(
      COLOR_PRIMARY,
      COLOR_BG
    );

    tft.drawString(
      f.time,
      8,
      y
    );

    drawWeatherIcon(
      62,
      y + 8,
      f.weatherCode
    );

    tft.setTextColor(
      COLOR_TEXT,
      COLOR_BG
    );

    String temp =
      String(
        f.temperature,
        1
      );

    temp += " C";

    tft.drawString(
      temp,
      90,
      y
    );

    tft.setTextColor(
      COLOR_GREY,
      COLOR_BG
    );

    tft.drawString(
      getWeatherDescription(
        f.weatherCode
      ),
      155,
      y
    );

    y += 20;
  }

  tft.drawFastHLine(
    0,
    SCREEN_H - 8,
    SCREEN_W,
    COLOR_DARKGREY
  );
}

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

void updateClockDisplay() {
  drawDateTime();
}

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