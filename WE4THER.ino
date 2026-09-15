#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "config.h"
#include "locations.h"
#include "weather.h"
#include "display.h"

// ============================================================
// 3x0c3t WE4THER
// ============================================================
//
// ESP8266 NodeMCU
// TFT ILI9341 240x320
// ROTATION 2
//
// Lille <-> Veracruz
// Changement automatique toutes les 5 secondes
// Bouton D3 pour changement manuel
//
// Date + heure locales par ville
// Météo Open-Meteo
// ============================================================

// ============================================================
// CURRENT LOCATION
// ============================================================

uint8_t currentLocation = 0;

// ============================================================
// TIMERS
// ============================================================

unsigned long lastWeatherUpdate  = 0;
unsigned long lastLocationSwitch = 0;
unsigned long lastClockUpdate    = 0;
unsigned long lastButtonChange   = 0;

// ============================================================
// BUTTON STATE
// ============================================================

bool lastButtonState = HIGH;
bool buttonState     = HIGH;

// ============================================================
// DISPLAY STATE
// ============================================================

bool screenReady = false;

// ============================================================
// SWITCH LOCATION
// ============================================================

void switchLocation() {

  currentLocation++;

  if (
    currentLocation >= LOCATION_COUNT
  ) {

    currentLocation = 0;
  }

  Serial.println();
  Serial.println(
    "========================================"
  );

  Serial.print(
    "LOCATION -> "
  );

  Serial.println(
    locations[currentLocation].name
  );

  Serial.print(
    "TIMEZONE -> "
  );

  Serial.println(
    locations[currentLocation].timezone
  );

  Serial.println(
    "========================================"
  );

  // ----------------------------------------------------------
  // Change le fuseau AVANT affichage
  // ----------------------------------------------------------

  setLocationTimezone(
    currentLocation
  );

  // ----------------------------------------------------------
  // Redessine ville + drapeau + date + heure + météo
  // ----------------------------------------------------------

  drawLocationScreen();
}

// ============================================================
// BUTTON
// ============================================================

void updateButton() {

  bool reading =
    digitalRead(
      SWITCH_PIN
    );

  if (
    reading != lastButtonState
  ) {

    lastButtonChange =
      millis();

    lastButtonState =
      reading;
  }

  if (
    millis() - lastButtonChange >
    DEBOUNCE_DELAY
  ) {

    if (
      reading != buttonState
    ) {

      buttonState =
        reading;

      if (
        buttonState == LOW
      ) {

        switchLocation();

        lastLocationSwitch =
          millis();
      }
    }
  }
}

// ============================================================
// AUTOMATIC LOCATION SWITCH
// ============================================================

void updateAutomaticLocation() {

  if (
    millis() - lastLocationSwitch >=
    LOCATION_INTERVAL
  ) {

    lastLocationSwitch =
      millis();

    switchLocation();
  }
}

// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(
    115200
  );

  delay(300);

  Serial.println();
  Serial.println();

  Serial.println(
    "========================================"
  );

  Serial.println(
    "3x0c3t WE4THER"
  );

  Serial.println(
    "========================================"
  );

  // ----------------------------------------------------------
  // BUTTON
  // ----------------------------------------------------------

  pinMode(
    SWITCH_PIN,
    INPUT_PULLUP
  );

  // ----------------------------------------------------------
  // TFT
  // ----------------------------------------------------------

  initDisplay();

  // ----------------------------------------------------------
  // WIFI
  // ----------------------------------------------------------

  connectWiFi();

  // ----------------------------------------------------------
  // NTP
  // ----------------------------------------------------------

  if (
    WiFi.status() == WL_CONNECTED
  ) {

    initTime();
  }

  // ----------------------------------------------------------
  // TIMEZONE INITIAL
  // ----------------------------------------------------------

  setLocationTimezone(
    currentLocation
  );

  // ----------------------------------------------------------
  // WEATHER
  // ----------------------------------------------------------

  if (
    WiFi.status() == WL_CONNECTED
  ) {

    updateWeather();
  }

  // ----------------------------------------------------------
  // INITIAL DISPLAY
  // ----------------------------------------------------------

  drawLocationScreen();

  // ----------------------------------------------------------
  // TIMERS
  // ----------------------------------------------------------

  lastLocationSwitch =
    millis();

  lastClockUpdate =
    millis();

  lastWeatherUpdate =
    millis();

  screenReady =
    true;

  Serial.println();
  Serial.println(
    "SYSTEM READY"
  );
}

// ============================================================
// LOOP
// ============================================================

void loop() {

  unsigned long now =
    millis();

  // ----------------------------------------------------------
  // BUTTON
  // ----------------------------------------------------------

  updateButton();

  // ----------------------------------------------------------
  // AUTOMATIC LOCATION
  // ----------------------------------------------------------

  updateAutomaticLocation();

  // ----------------------------------------------------------
  // CLOCK
  // ----------------------------------------------------------

  if (
    now - lastClockUpdate >=
    CLOCK_INTERVAL
  ) {

    lastClockUpdate =
      now;

    updateClockDisplay();
  }

  // ----------------------------------------------------------
  // WEATHER
  // ----------------------------------------------------------

  if (
    now - lastWeatherUpdate >=
    WEATHER_INTERVAL
  ) {

    updateWeather();

    lastWeatherUpdate =
      now;

    if (
      screenReady
    ) {

      updateWeatherDisplay();
    }
  }

  // ----------------------------------------------------------
  // LOOP
  // ----------------------------------------------------------

  delay(5);
}