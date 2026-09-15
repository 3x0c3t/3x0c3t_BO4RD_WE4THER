#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// 3x0c3t WE4THER - CONFIGURATION
// ============================================================

// ============================================================
// WIFI
// ============================================================

#define WIFI_SSID     "N0wifiHere"
#define WIFI_PASSWORD "@PassWeurd"

// ============================================================
// SWITCH
// ============================================================

#define SWITCH_PIN D3

// ============================================================
// TIMINGS
// ============================================================

const unsigned long WEATHER_INTERVAL  = 600000UL;  // 10 min
const unsigned long LOCATION_INTERVAL = 5000UL;    // 5 sec
const unsigned long CLOCK_INTERVAL    = 1000UL;     // 1 sec
const unsigned long WIFI_TIMEOUT      = 30000UL;
const unsigned long DEBOUNCE_DELAY    = 50UL;

// ============================================================
// TFT
// ============================================================

#define TFT_ROTATION 2

// ============================================================
// COLORS
// ============================================================

#define COLOR_BG        TFT_BLACK
#define COLOR_TEXT      TFT_WHITE
#define COLOR_PRIMARY   TFT_CYAN
#define COLOR_SECONDARY TFT_GREEN
#define COLOR_WARNING   TFT_YELLOW
#define COLOR_ERROR     TFT_RED
#define COLOR_GREY      0x7BEF
#define COLOR_DARKGREY  0x39C7

#endif