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
    "MEXICO",
    "MEXIQUE",
    "America/Mexico_City",
    19.4326,
    -99.1332,
    1
  },
  {
    "CORDOBA MX",
    "MEXIQUE",
    "America/Mexico_City",
    18.8843,
    -96.9475,
    1
  },
  {
    "TERRE ADELI",
    "ANTARCTIQUE",
    "Antarctica/DumontDUrville",
    -66.6633,
    140.0010,
    0
  },
  {
    "VATICAN",
    "VATICAN",
    "Europe/Rome",
    41.9029,
    12.4534,
    5
  }
};

const uint8_t LOCATION_COUNT = 5;

#endif