#ifndef LOCATIONS_H
#define LOCATIONS_H

struct Location {
  const char* name;
  const char* country;
  const char* timezone;
  float latitude;
  float longitude;
};

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
    "MEXIQUE",
    "America/Mexico_City",
    19.1738,
    -96.1342
  },

  {
    "XXX",
    "XXX",
    "UTC",
    0.0,
    0.0
  }
};

const uint8_t LOCATION_COUNT = 3;

#endif