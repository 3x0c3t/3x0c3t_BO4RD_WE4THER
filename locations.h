#ifndef LOCATIONS_H
#define LOCATIONS_H

struct Location {
  const char* name;
  const char* country;
  const char* timezone;
  float latitude;
  float longitude;
  uint8_t flagId;
};

const Location locations[] = {
  {
    "HELLEMMES",
    "FRANCE",
    "Europe/Paris",
    50.6167,
    3.11667,
    0
  },
  {
    "CHARLEVILLE MEZIERES",
    "FRANCE",
    "Europe/Paris",
    49.7621,
    4.7202,
    0
  },
  {
    "ANGLET",
    "FRANCE",
    "Europe/Paris",
    43.4833,
    -1.5167,
    0
  },
  {
    "KINGSTOWN",
    "JAMAIQUE",
    "America/St_Vincent",
    13.1600,
    -61.2248,
    2
  },
  {
    "VATICAN",
    "VATICAN",
    "Europe/Rome",
    41.9029,
    12.4534,
    5
  },
  {
    "MEXICO",
    "MEXIQUE",
    "America/Mexico_City",
    19.4326,
    -99.1332,
    1
  },
  {
    "ROSWELL",
    "USA",
    "America/Denver",
    33.3943,
    -104.5230,
    3
  }
};

const uint8_t LOCATION_COUNT = 7;

#endif