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
    "ZONGOLICA",
    "MEXIQUE",
    "America/Mexico_City",
    18.66672,
    -96.99821,
    1
  },
  {
    "KINGSTON",
    "JAMAIQUE",
    "America/Jamaica",
    18.0000,
    -76.7833,
    2
  },
  {
    "HOUSTON",
    "TEXAS",
    "America/Chicago",
    29.762778,
    -95.383056,
    3
  },
  {
    "KATMANDOU",
    "NEPAL",
    "Asia/Kathmandu",
    27.7172,
    85.3240,
    4
  }
};
const uint8_t LOCATION_COUNT = 5;
#endif