#ifndef LOCATIONS_H
#define LOCATIONS_H

#include <Arduino.h>

// ============================================================
// LOCATION
// ============================================================

struct Location {

  const char* name;
  const char* country;
  const char* timezone;

  float latitude;
  float longitude;
};

// ============================================================
// LOCATIONS
// ============================================================

const Location locations[] = {

  {
    "LILLE",
    "FRANCE",
    "Europe/Paris",
    50.6292,
    3.0573
  },

  {
    "VERACRUZ",
    "MEXICO",
    "America/Mexico_City",
    19.1738,
    -96.1342
  }
};

const uint8_t LOCATION_COUNT = 2;

#endif