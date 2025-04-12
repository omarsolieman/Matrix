// Includes - Ensure FastLED library is installed via Library Manager
#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>
#include <stdlib.h> // Required for strtol, atoi
#include <HTTPClient.h> // NEW: For making HTTP requests
#include <ArduinoJson.h> // NEW: For parsing weather data (Install via Library Manager)

// NEW: Include custom headers for organization
#include "pixel_art.h"
#include "animations.h"

// ---!!! IMPORTANT: CONFIGURE WIFI HERE !!!---
const char* ssid = "ODesigns";      // <<< CHANGE THIS to your WiFi network name
const char* password = "omartaher2004"; // <<< CHANGE THIS to your WiFi password
// -------------------------------------------

// ---!!! IMPORTANT: CONFIGURE WEATHER HERE !!!---
String openWeatherMapApiKey = "896c645816d43f21898eae02bd56f486"; // <<< CHANGE THIS to your OpenWeatherMap API key
String city = "Subang Jaya";                    // <<< CHANGE THIS to your city
String countryCode = "MY";                 // <<< CHANGE THIS to your country code (e.g., US, GB, DE)
#define WEATHER_UNITS "metric"             // "metric" (Celsius) or "imperial" (Fahrenheit)
#define WEATHER_UPDATE_INTERVAL_SECS 900   // Update weather every 15 minutes (900 seconds)
// -------------------------------------------


// --- LED Matrix Settings ---
#define DATA_PIN    4       // Pin connected to DIN (Change if you use a different pin)
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    64      // 8x8 matrix
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8
CRGB leds[NUM_LEDS];          // Array to hold LED color data

#define BRIGHTNESS 50       // Initial brightness (0-255). Start low!

// --- Web Server ---
WebServer server(80);         // Web server on port 80

// --- Application State ---
enum DisplayMode {
  MODE_STATIC_COLOR,
  MODE_RAINBOW_CYCLE,
  MODE_CONFETTI,
  MODE_PALETTE,
  MODE_NOISE,
  MODE_COLOR_WIPE,
  MODE_BOUNCING_PIXEL,
  MODE_FIRE,
  MODE_PIXEL_ART,
  MODE_GAME_OF_LIFE,
  MODE_WEATHER, // NEW: Weather display mode
  MODE_ICON_TEST // NEW: Icon test cycle mode
};
DisplayMode currentMode = MODE_STATIC_COLOR; // Default mode
CRGB current_color = CRGB::Blue;             // Default static color
String currentArtName = "";                  // Name of the currently displayed art/mood
int currentRotation = 0;                     // Rotation setting (0=0, 1=90 CW, 2=180, 3=270 CW)

// --- Weather State --- NEW Section
int weather_condition_code = 800; // Default to 'clear' (OpenWeatherMap code)
float weather_temp = 0.0;
float weather_humidity = 0.0;
String weather_description = "Clear";
String weather_icon_name = "clear_day"; // Corresponds to a pixel art icon name
unsigned long lastWeatherUpdate = 0; // Timer for weather updates


// --- Palettes ---
// MOVED to animations.h

// --- Pixel Art & Mood Data ---
// Color Defines MOVED to pixel_art.h
// Pixel Art Arrays MOVED to pixel_art.h
// GoL Colors MOVED to animations.h


// --- Game of Life Variables ---
// MOVED to animations.h
// Definition of grid arrays moved here, declaration is extern in animations.h
byte gol_grid[NUM_LEDS];         // Current state (0=dead, 1=alive) - Stores state based on final rotated index
byte gol_next_grid[NUM_LEDS];    // Buffer for next state calculation
unsigned long lastGolUpdate = 0; // Timer for GoL updates
bool gol_initialized = false;    // Flag to initialize GoL grid on first run


// --- Forward Declarations ---
// Web Handlers
void handleRoot();
void handleSetColor();
void handleSetMode();
void handleSetArt();
void handleSetRotation();
void handleNotFound();
// LED Utilities & Animations
void fillMatrix(CRGB color);
// displayPixelArt MOVED to animations.h
// XY MOVED to animations.h
// IndexToXY MOVED to animations.h
// animation... functions MOVED to animations.h
// initializeGolGrid MOVED to animations.h
// countLiveNeighbors MOVED to animations.h
// NEW Weather Functions
void fetchWeatherData();
String mapConditionToIconName(int conditionCode, bool isDay); // Helper
// NEW Icon Test Function
void animationIconTest(); // Forward declaration

// --- SETUP: Runs once on boot ---
void setup() {
  Serial.begin(115200);
  // Wait for Serial connection to be established (important for native USB)
  while (!Serial) {
    delay(10); // Wait for serial port to connect.
  }
  delay(1000); // Give Serial Monitor time to connect after Serial becomes true
  Serial.println("\n\n--- ESP32 Matrix Controller Booting --- DEBUG --- "); Serial.flush();

  // Initialize FastLED
  Serial.println("Setup: Initializing FastLED..."); Serial.flush();
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  fillMatrix(CRGB::Black); // Start with LEDs off
  FastLED.show();
  Serial.println("Setup: FastLED Initialized"); Serial.flush();

  // Connect to Wi-Fi
  Serial.print("Setup: Connecting to WiFi SSID: "); Serial.println(ssid); Serial.flush();
  WiFi.begin(ssid, password);
  int wifi_retry_count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); Serial.flush(); // Print dots while connecting
    wifi_retry_count++;
    if (wifi_retry_count > 20) { // Timeout after ~10 seconds
        Serial.println("Setup: WiFi Connection Failed! Timeout."); Serial.flush();
        goto wifi_failed; // Jump past WiFi dependent stuff
    }
  }

  Serial.println(); // Newline after dots
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Setup: WiFi connected successfully."); Serial.flush();
    Serial.print("Setup: IP address: http://"); Serial.println(WiFi.localIP()); Serial.flush();
    // Initial weather fetch (only if WiFi connected)
    Serial.println("Setup: Performing initial weather fetch..."); Serial.flush();
    fetchWeatherData(); // Fetch weather data on startup
    lastWeatherUpdate = millis(); // Set timer after first fetch
    Serial.println("Setup: Initial weather fetch attempt complete."); Serial.flush();
  } else {
     Serial.println("Setup: Proceeding without WiFi connection. Weather functionality disabled."); Serial.flush();
  }

wifi_failed: // Label for jumping past WiFi stuff if connection fails
  Serial.println("Setup: Setting up Web Server routes..."); Serial.flush();
  // Setup Web Server Routes (URL Handlers)
  server.on("/", HTTP_GET, handleRoot);      // Serve the main control page
  server.on("/setcolor", HTTP_GET, handleSetColor); // Handle color changes
  server.on("/setmode", HTTP_GET, handleSetMode);   // Handle mode changes
  server.on("/setart", HTTP_GET, handleSetArt);     // Handle pixel art/mood changes
  server.on("/setrotation", HTTP_GET, handleSetRotation); // Handle rotation changes
  server.onNotFound(handleNotFound);            // Handle invalid URLs (404)

  Serial.println("Setup: Starting Web Server..."); Serial.flush();
  server.begin(); // Start the web server
  Serial.println("Setup: HTTP server started"); Serial.flush();

  // Initial display state - Set based on defaults
  Serial.print("Setup: Setting initial display state. Mode: "); Serial.print(currentMode);
  Serial.print(", Art: "); Serial.print(currentArtName == "" ? "None" : currentArtName);
  Serial.print(", Rotation: "); Serial.println(currentRotation * 90); Serial.flush();

  if (currentArtName != "") {
      Serial.println("Setup: Initial display is Pixel Art."); Serial.flush();
      const CRGB *artPtr = findArtByName(currentArtName); // Use helper
      if(artPtr) {
          displayPixelArt(artPtr); // Function is now in animations.h
          FastLED.show(); // Show the art immediately
          Serial.println("Setup: Displayed initial art/mood."); Serial.flush();
      } else {
          Serial.println("Setup Error: Initial art/mood name set, but not found!"); Serial.flush();
          currentArtName = ""; // Reset if not found
          currentMode = MODE_STATIC_COLOR; // Fallback to static
          fillMatrix(current_color);
          FastLED.show(); // Show fallback color
      }
  } else if (currentMode == MODE_STATIC_COLOR) {
      Serial.println("Setup: Initial display is Static Color."); Serial.flush();
      fillMatrix(current_color);
      FastLED.show(); // Show initial color
      Serial.println("Setup: Displayed initial static color."); Serial.flush();
  } else if (currentMode == MODE_WEATHER) {
      Serial.println("Setup: Initial display is Weather."); Serial.flush();
      const CRGB *weatherIconPtr = findArtByName(weather_icon_name);
      if (weatherIconPtr) {
          displayPixelArt(weatherIconPtr);
          FastLED.show();
          Serial.println("Setup: Displayed initial weather icon."); Serial.flush();
      } else {
          Serial.println("Setup Error: Initial weather icon name not found!"); Serial.flush();
          fillMatrix(CRGB::Magenta); // Show error color if initial icon not found
          FastLED.show();
      }
  } else {
      Serial.print("Setup: Initial display is Animation: "); Serial.println(currentMode); Serial.flush();
      if (currentMode == MODE_GAME_OF_LIFE) { // Initialize GoL if starting in that mode
          Serial.println("Setup: Initializing Game of Life."); Serial.flush();
          initializeGolGrid(); // Function is now in animations.h
      }
  }

  Serial.println("--- Setup complete. Entering loop. ---"); Serial.flush();
}

// --- LOOP: Runs repeatedly ---
void loop() {
  server.handleClient(); // Check for and handle incoming web requests

  // --- Periodic Weather Update --- NEW
  if (WiFi.status() == WL_CONNECTED && openWeatherMapApiKey != "YOUR_API_KEY") {
    unsigned long currentMillis = millis();
    // Check if it's time to update weather (avoiding millis() overflow issue)
    if (currentMillis - lastWeatherUpdate >= (WEATHER_UPDATE_INTERVAL_SECS * 1000UL) || lastWeatherUpdate == 0) {
        fetchWeatherData();
        lastWeatherUpdate = currentMillis; // Reset timer AFTER fetch attempt
    }
  }

  // Update the LED matrix based on the current mode
  // All animation functions are now defined in animations.h
  switch (currentMode) {
    case MODE_STATIC_COLOR:
      // State is set by handlers, no continuous update needed here
      break;
    case MODE_RAINBOW_CYCLE:
      animationRainbowCycle();
      break;
    case MODE_CONFETTI:
      animationConfetti();
      break;
    case MODE_PALETTE:
      animationPalette();
      break;
    case MODE_NOISE:
      animationNoise();
      break;
    case MODE_COLOR_WIPE:
      animationColorWipe();
      break;
    case MODE_BOUNCING_PIXEL:
      animationBouncingPixel();
      break;
    case MODE_FIRE:
      animationFire();
      break;
    case MODE_PIXEL_ART:
      // State is set by handler, no continuous update needed here
      break;
    case MODE_GAME_OF_LIFE:
      animationGameOfLife();
      break;
    case MODE_WEATHER: // NEW
      animationWeather(); // Call the weather display function (to be added in animations.h)
      break;
    case MODE_ICON_TEST: // NEW
      animationIconTest(); // Call the icon test function
      break;
  }

  // Show the updated LED colors on the matrix
  // FastLED.delay() includes FastLED.show() and manages frame rate
  // Except for GoL, Weather and IconTest which manage their own timing/updates
  if (currentMode != MODE_GAME_OF_LIFE && currentMode != MODE_WEATHER && currentMode != MODE_ICON_TEST) {
      FastLED.delay(1000 / 60); // ~60 FPS for most animations
  }
  // GoL handles its delay inside its animation function
  // Weather updates are static (icon change) driven by fetch interval, not frame rate. Show happens in animationWeather.
  // IconTest handles its own delay/timing.
}

// --- LED Utility Functions ---

// Fills the entire matrix with a single color (Keep this general utility here)
void fillMatrix(CRGB color) {
  fill_solid(leds, NUM_LEDS, color);
}

// --- NEW Weather Fetching Function ---
void fetchWeatherData() {
    Serial.println("fetchWeatherData: Attempting fetch..."); Serial.flush();
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("fetchWeatherData: WiFi not connected. Skipping."); Serial.flush();
        weather_icon_name = "error"; // Set error icon
        return;
    }
    if (openWeatherMapApiKey == "YOUR_API_KEY" || city == "" || countryCode == "") {
         Serial.println("fetchWeatherData: API Key or Location not configured. Skipping."); Serial.flush();
         weather_icon_name = "error"; // Set error icon
         return;
    }

    Serial.println("fetchWeatherData: Configuration OK. Setting up HTTP request..."); Serial.flush();
    HTTPClient http;
    String encodedCity = city; // Create a copy to modify
    encodedCity.replace(" ", "%20"); // Replace spaces with %20 for URL encoding
    String serverPath = "https://api.openweathermap.org/data/2.5/weather?q=" + encodedCity + "," + countryCode + "&units=" + WEATHER_UNITS + "&appid=" + openWeatherMapApiKey;
    Serial.print("fetchWeatherData: Request URL: "); Serial.println(serverPath); Serial.flush();

    http.begin(serverPath.c_str());
    Serial.println("fetchWeatherData: Sending HTTP GET..."); Serial.flush();
    int httpResponseCode = http.GET();
    String payload = "{}";
    bool success = false;

    if (httpResponseCode > 0) {
        Serial.print("fetchWeatherData: HTTP Response code: "); Serial.println(httpResponseCode); Serial.flush();
        payload = http.getString();
        success = true;
        // Serial.println("fetchWeatherData: Payload: " + payload); // Uncomment for full payload debug
    } else {
        Serial.print("fetchWeatherData: HTTP Error code: "); Serial.println(httpResponseCode); Serial.flush();
        Serial.printf("fetchWeatherData: HTTP GET failed: %s\n", http.errorToString(httpResponseCode).c_str()); Serial.flush();
    }
    Serial.println("fetchWeatherData: Ending HTTP connection..."); Serial.flush();
    http.end();

    if (success && httpResponseCode == 200) {
        Serial.println("fetchWeatherData: Parsing JSON response..."); Serial.flush();
        StaticJsonDocument<1200> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
            Serial.print(F("fetchWeatherData: deserializeJson() failed: "));
            Serial.println(error.f_str()); Serial.flush();
            weather_icon_name = "error";
            return;
        }
        Serial.println("fetchWeatherData: JSON Parsed OK."); Serial.flush();

        JsonObject weather_0 = doc["weather"][0];
        weather_condition_code = weather_0["id"];
        weather_description = weather_0["description"].as<String>();
        JsonObject main = doc["main"];
        weather_temp = main["temp"];
        weather_humidity = main["humidity"];
        long dt = doc["dt"];
        long timezone = doc["timezone"];
        long sunrise = doc["sys"]["sunrise"];
        long sunset = doc["sys"]["sunset"];
        long local_time = dt + timezone;
        bool isDay = (local_time > sunrise && local_time < sunset);
        weather_icon_name = mapConditionToIconName(weather_condition_code, isDay);

        Serial.println("fetchWeatherData: Data extracted successfully."); Serial.flush();
        Serial.printf("Weather Update: Code=%d, Desc=%s, Temp=%.1f, Hum=%.0f, Icon=%s\n",
                      weather_condition_code, weather_description.c_str(), weather_temp, weather_humidity, weather_icon_name.c_str()); Serial.flush();

    } else {
      Serial.println("fetchWeatherData: Failed to get valid weather response or HTTP error occurred."); Serial.flush();
      weather_icon_name = "error";
    }
    Serial.println("fetchWeatherData: Fetch attempt finished."); Serial.flush();
}

// --- NEW Helper to map OWM code to icon name ---
String mapConditionToIconName(int conditionCode, bool isDay) {
    // Reference: https://openweathermap.org/weather-conditions#Weather-Condition-Codes-2
    // We need icons defined in pixel_art.h named like:
    // clear_day, clear_night, few_clouds_day, few_clouds_night, clouds, shower_rain, rain_day, rain_night,
    // thunderstorm, snow, mist, error
    if (conditionCode >= 200 && conditionCode < 300) { return "thunderstorm"; } // Group 2xx: Thunderstorm
    if (conditionCode >= 300 && conditionCode < 400) { return "shower_rain"; }  // Group 3xx: Drizzle -> shower rain icon
    if (conditionCode >= 500 && conditionCode < 600) { // Group 5xx: Rain
        if (conditionCode == 500 || conditionCode == 501 || conditionCode == 520 || conditionCode == 521) {
            return isDay ? "rain_day" : "rain_night"; // Light/moderate rain
        } else {
            return "shower_rain"; // Heavy rain / other rain types
        }
    }
    if (conditionCode >= 600 && conditionCode < 700) { return "snow"; }         // Group 6xx: Snow
    if (conditionCode >= 700 && conditionCode < 800) { return "mist"; }         // Group 7xx: Atmosphere (Mist, Fog, etc.)
    if (conditionCode == 800) { return isDay ? "clear_day" : "clear_night"; } // Group 800: Clear
    if (conditionCode == 801) { return isDay ? "few_clouds_day" : "few_clouds_night"; } // Group 801: Few Clouds
    if (conditionCode > 801 && conditionCode < 805) { return "clouds"; }       // Group 80x: Clouds (Scattered, Broken, Overcast)

    Serial.printf("Warning: Unmapped weather condition code: %d\n", conditionCode);
    return "error"; // Default/error icon for unmapped codes
}

// --- Web Server Handlers ---

// ==========================================================================
// Serves the main HTML page - MODIFIED to add Weather Mode
// ==========================================================================
void handleRoot() {
  // Convert current color to HEX string for the color picker default value
  char hexColor[8];
  snprintf(hexColor, sizeof(hexColor), "#%02x%02x%02x", current_color.r, current_color.g, current_color.b);

  // Use C++ Raw String Literal R"raw( ... )raw" for the HTML content
  String html = R"raw(
<!DOCTYPE html>
<html lang="en" class="dark">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Matrix Control</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <style>
      /* Basic styling for links to look like buttons */
      a.button-link, button.button-link {
        display: flex; align-items: center; justify-content: center;
        padding: 0.5rem 0.75rem; border-radius: 0.375rem; box-shadow: 0 1px 2px 0 rgba(0, 0, 0, 0.05);
        font-size: 0.875rem; font-weight: 500; text-decoration: none;
        transition: background-color 0.15s ease-in-out;
        background-color: #374151; color: #d1d5db; /* gray-700 gray-300 */
        border: none; cursor: pointer; text-align: center;
      }
      a.button-link:hover, button.button-link:hover { background-color: #4b5563; /* gray-600 */ }
      a.button-link span.emoji, button.button-link span.emoji { margin-right: 0.4em; display: inline-block; }

      /* Style for active rotation button */
      a.button-link.active { background-color: #4f46e5; color: white; /* indigo-600 */ }
      a.button-link.active:hover { background-color: #4338ca; /* indigo-700 */ }


      /* Color input styling */
      input[type="color"]::-webkit-color-swatch-wrapper { padding: 0; }
      input[type="color"]::-webkit-color-swatch { border: none; border-radius: 4px; }
      input[type="color"]::-moz-color-swatch { border: none; border-radius: 4px; }
      #status { transition: opacity 0.5s ease-in-out; }
    </style>
    <script>
      tailwind.config = {
        darkMode: 'class', theme: { extend: { fontFamily: { sans: ['Inter', 'system-ui', 'sans-serif'], }, } }
      }
    </script>
</head>
<body class="bg-gray-900 text-gray-200 font-sans flex justify-center items-start min-h-screen p-4 sm:p-6">
    <div class="bg-gray-800 p-6 sm:p-8 rounded-lg shadow-xl w-full max-w-md">
        <h1 class="text-2xl font-bold text-center mb-6 text-white">LED Matrix Control</h1>
        <div id="status" class="text-center mb-4 text-sm min-h-[1.2em]"></div>

        <div class="mb-6">
            <h2 class="text-lg font-semibold mb-3 text-indigo-400 border-b border-gray-700 pb-1">Static Color</h2>
            <form action="/setcolor" method="GET" class="flex items-center space-x-3">
                <div class="relative flex-shrink-0">
                    <input type="color" id="color" name="color" value=")raw";
  html += String(hexColor); // Inject current color hex value from C++
  html += R"raw(" class="w-12 h-10 p-0 border-0 rounded cursor-pointer appearance-none bg-gray-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-offset-gray-800 focus:ring-indigo-500">
                </div>
                <button type="submit" class="button-link flex-grow"> <span class="emoji">🎨</span>Set Static Color
                </button>
            </form>
        </div>

        <div class="mb-6">
            <h2 class="text-lg font-semibold mb-3 text-indigo-400 border-b border-gray-700 pb-1">Animation Modes</h2>
            <div class="grid grid-cols-2 sm:grid-cols-3 gap-3">
                <a href="/setmode?mode=rainbow" class="button-link"><span class="emoji">🌈</span>Rainbow</a>
                <a href="/setmode?mode=confetti" class="button-link"><span class="emoji">✨</span>Confetti</a>
                <a href="/setmode?mode=palette" class="button-link"><span class="emoji">🎨</span>Palette</a>
                <a href="/setmode?mode=noise" class="button-link"><span class="emoji">〰️</span>Noise</a>
                <a href="/setmode?mode=wipe" class="button-link"><span class="emoji">💧</span>Wipe</a>
                <a href="/setmode?mode=bounce" class="button-link"><span class="emoji">↔️</span>Bounce</a>
                <a href="/setmode?mode=fire" class="button-link"><span class="emoji">🔥</span>Fire</a>
            </div>
        </div>

        <div class="mb-6">
            <h2 class="text-lg font-semibold mb-3 text-indigo-400 border-b border-gray-700 pb-1">Simulations & Info</h2> <!-- Changed Title -->
            <div class="grid grid-cols-2 sm:grid-cols-3 gap-3">
                 <a href="/setmode?mode=gameoflife" class="button-link"><span class="emoji">🧬</span>Game of Life</a>
                 <a href="/setmode?mode=weather" class="button-link"><span class="emoji">🌦️</span>Weather</a> <!-- NEW Weather Button -->
                 <a href="/setmode?mode=icontest" class="button-link"><span class="emoji">🖼️</span>Icon Test</a> <!-- NEW Icon Test Button -->
            </div>
        </div>

        <div class="mb-6">
            <h2 class="text-lg font-semibold mb-3 text-indigo-400 border-b border-gray-700 pb-1">Pixel Art</h2>
            <div class="grid grid-cols-2 sm:grid-cols-3 gap-3">
                 <a href="/setart?name=heart" class="button-link"><span class="emoji">❤️</span>Heart</a>
                 <a href="/setart?name=smile" class="button-link"><span class="emoji">😊</span>Smile</a>
                 <a href="/setart?name=arrow_up" class="button-link"><span class="emoji">⬆️</span>Arrow Up</a>
                 <a href="/setart?name=pacman" class="button-link"><span class="emoji">🟡</span>Pacman</a>
                 <a href="/setart?name=ghost" class="button-link"><span class="emoji">👻</span>Ghost</a>
            </div>
        </div>

        <div class="mb-6">
            <h2 class="text-lg font-semibold mb-3 text-indigo-400 border-b border-gray-700 pb-1">Moods</h2>
            <div class="grid grid-cols-2 sm:grid-cols-3 gap-3">
                 <a href="/setart?name=octopus_neutral" class="button-link"><span class="emoji">🐙</span>Neutral</a>
                 <a href="/setart?name=octopus_happy" class="button-link"><span class="emoji">😄</span>Happy</a>
                 <a href="/setart?name=octopus_sad" class="button-link"><span class="emoji">😥</span>Sad</a>
            </div>
        </div>

        <div>
            <h2 class="text-lg font-semibold mb-3 text-indigo-400 border-b border-gray-700 pb-1">Settings</h2>
            <p class="text-sm text-gray-400 mb-2">Display Rotation:</p>
            <div class="grid grid-cols-4 gap-2">
                 <a href="/setrotation?angle=0"   class="button-link rotation-btn" data-angle="0">0°</a>
                 <a href="/setrotation?angle=90"  class="button-link rotation-btn" data-angle="90">90°</a>
                 <a href="/setrotation?angle=180" class="button-link rotation-btn" data-angle="180">180°</a>
                 <a href="/setrotation?angle=270" class="button-link rotation-btn" data-angle="270">270°</a>
            </div>
        </div>

    </div>

    <script>
        console.log("Control Page Loaded.");
        // Function to highlight the active rotation button
        function setActiveRotation(angle) {
            const buttons = document.querySelectorAll('.rotation-btn');
            buttons.forEach(button => {
                if (button.getAttribute('data-angle') == angle) {
                    button.classList.add('active');
                } else {
                    button.classList.remove('active');
                }
            });
        }

        document.addEventListener('DOMContentLoaded', () => {
             console.log("DOM fully loaded.");
             // Get initial rotation state (passed from C++) and highlight the button
             const initialRotationAngle = )raw";
  html += String(currentRotation * 90); // Inject current rotation angle
  html += R"raw(;
             setActiveRotation(initialRotationAngle);

             // Add click listeners if we were using JS fetch, but simple links work for now.
        });
    </script>
</body>
</html>
)raw";

  server.send(200, "text/html", html); // Send the constructed HTML page
}

// Handler for setting static color - No changes needed
void handleSetColor() {
  Serial.println("handleSetColor called...");
  if (server.hasArg("color")) {
    String hexColorStr = server.arg("color"); // Gets "#RRGGBB"
    Serial.printf("  Color arg: %s\n", hexColorStr.c_str());
    if (hexColorStr.startsWith("#")) { hexColorStr = hexColorStr.substring(1); }

    if (hexColorStr.length() == 6) {
      char *endptr; long colorValue = strtol(hexColorStr.c_str(), &endptr, 16);
      if (*endptr == '\0') {
          current_color = CRGB((colorValue >> 16) & 0xFF, (colorValue >> 8) & 0xFF, colorValue & 0xFF);
          currentMode = MODE_STATIC_COLOR;
          currentArtName = ""; gol_initialized = false; // Reset GoL flag
          Serial.printf("  State updated: mode=%d, color=(%d,%d,%d), art='%s'\n", currentMode, current_color.r, current_color.g, current_color.b, currentArtName.c_str());
          fillMatrix(current_color); FastLED.show();
          Serial.println("  fillMatrix & FastLED.show() called.");
          server.send(200, "text/plain", "OK");
      } else { Serial.println("  Error: Invalid characters in color string."); server.send(400, "text/plain", "Invalid color format (non-hex characters)"); }
    } else { Serial.println("  Error: Invalid color format length."); server.send(400, "text/plain", "Invalid color format (length != 6)"); }
  } else { Serial.println("  Error: Missing 'color' parameter."); server.send(400, "text/plain", "Missing 'color' parameter"); }
}


// Handler for setting display mode - MODIFIED for Weather
void handleSetMode() {
  Serial.println("handleSetMode called...");
  if (server.hasArg("mode")) {
    String modeStr = server.arg("mode");
    Serial.printf("  Mode arg: %s\n", modeStr.c_str());
    bool modeSet = true;
    DisplayMode requestedMode = currentMode;

    if (modeStr.equalsIgnoreCase("static")) requestedMode = MODE_STATIC_COLOR;
    else if (modeStr.equalsIgnoreCase("rainbow")) requestedMode = MODE_RAINBOW_CYCLE;
    else if (modeStr.equalsIgnoreCase("confetti")) requestedMode = MODE_CONFETTI;
    else if (modeStr.equalsIgnoreCase("palette")) requestedMode = MODE_PALETTE;
    else if (modeStr.equalsIgnoreCase("noise")) requestedMode = MODE_NOISE;
    else if (modeStr.equalsIgnoreCase("wipe")) requestedMode = MODE_COLOR_WIPE;
    else if (modeStr.equalsIgnoreCase("bounce")) requestedMode = MODE_BOUNCING_PIXEL;
    else if (modeStr.equalsIgnoreCase("fire")) requestedMode = MODE_FIRE;
    else if (modeStr.equalsIgnoreCase("gameoflife")) requestedMode = MODE_GAME_OF_LIFE;
    else if (modeStr.equalsIgnoreCase("weather")) requestedMode = MODE_WEATHER; // NEW
    else if (modeStr.equalsIgnoreCase("icontest")) requestedMode = MODE_ICON_TEST; // NEW
    else { Serial.println("  Error: Invalid mode requested."); server.send(400, "text/plain", "Invalid mode"); modeSet = false; }

    if (modeSet) {
      // Reset flags/state if switching away from specific modes
      if (requestedMode != MODE_GAME_OF_LIFE) { gol_initialized = false; }

      currentMode = requestedMode;
      currentArtName = ""; // Clear pixel art name when changing mode

      Serial.printf("  State updated: mode=%d, art='%s'\n", currentMode, currentArtName.c_str());

      bool needsShow = false;
      // Set initial state for modes that need it
      if (currentMode == MODE_STATIC_COLOR) { fillMatrix(current_color); needsShow = true; Serial.println("  fillMatrix called for static color."); }
      else if (currentMode == MODE_COLOR_WIPE || currentMode == MODE_BOUNCING_PIXEL || currentMode == MODE_FIRE) { fillMatrix(CRGB::Black); needsShow = true; Serial.println("  fillMatrix called for black background."); }
      else if (currentMode == MODE_GAME_OF_LIFE) { Serial.println("  Switched to Game of Life mode."); fillMatrix(CRGB::Black); needsShow = true; } // Ensure GoL starts cleared
      else if (currentMode == MODE_WEATHER) { // NEW
          Serial.println("  Switched to Weather mode.");
          // Trigger an immediate display update based on current weather state
          animationWeather(); // This function should handle finding/displaying icon & FastLED.show()
          needsShow = false; // animationWeather handles its own show
          // Optionally trigger a fetch if data is old or missing? Loop handle this now.
          // fetchWeatherData(); // Fetch immediately on mode switch? Or let loop handle interval?
      } else if (currentMode == MODE_ICON_TEST) { // NEW
           Serial.println("  Switched to Icon Test mode.");
           // animationIconTest handles its own initial display and show
           needsShow = false;
      }

      if(needsShow) { FastLED.show(); Serial.println("  FastLED.show() called."); }
      server.send(200, "text/plain", "OK");
    }
  } else { Serial.println("  Error: Missing 'mode' parameter."); server.send(400, "text/plain", "Missing 'mode' parameter"); }
}

// Handler for setting pixel art (or mood)
void handleSetArt() {
  Serial.println("handleSetArt called...");
  if (server.hasArg("name")) {
    String artName = server.arg("name");
     Serial.printf("  Art/Mood arg: %s\n", artName.c_str());
    bool artFound = false;
    const CRGB *artPtr = nullptr;
    String tempArtName = "";

    // Use the helper function to find the art pointer
    artPtr = findArtByName(artName);
    if (artPtr) {
        tempArtName = artName; // Store the requested name
        artFound = true;
    }

    if (artFound) {
      currentArtName = tempArtName; // Store the name of the art being displayed
      currentMode = MODE_PIXEL_ART;
      gol_initialized = false; // Reset GoL flag
      Serial.printf("  State updated: mode=%d, art='%s'\n", currentMode, currentArtName.c_str());
      displayPixelArt(artPtr); // Function is now in animations.h
      FastLED.show();
      Serial.println("  displayPixelArt & FastLED.show() called.");
      server.send(200, "text/plain", "OK");
    } else { Serial.printf("  Error: Unknown art/mood name requested: %s\n", artName.c_str()); server.send(400, "text/plain", "Unknown art/mood name"); }
  } else { Serial.println("  Error: Missing art/mood 'name' parameter."); server.send(400, "text/plain", "Missing art/mood 'name' parameter"); }
}

// Handler for setting rotation - MODIFIED to redraw weather if active
void handleSetRotation() {
    Serial.println("handleSetRotation called...");
    if (server.hasArg("angle")) {
        int angle = server.arg("angle").toInt(); // Use toInt() for integer conversion
        Serial.printf("  Angle arg: %d\n", angle);
        int newRotation = 0; // Default to 0 degrees
        bool validAngle = true;

        switch (angle) {
            case 90: newRotation = 1; break;
            case 180: newRotation = 2; break;
            case 270: newRotation = 3; break;
            case 0: newRotation = 0; break;
            default:
                Serial.println("  Error: Invalid angle value.");
                server.send(400, "text/plain", "Invalid angle value (must be 0, 90, 180, or 270)");
                validAngle = false; break;
        }

        if (validAngle) {
            if (currentRotation != newRotation) {
                currentRotation = newRotation;
                Serial.printf("  Rotation updated to: %d (%d degrees)\n", currentRotation, angle);

                // Redraw the current display immediately if it's static/art/GoL/Weather
                if (currentMode == MODE_PIXEL_ART && currentArtName != "") {
                    Serial.println("  Redrawing pixel art due to rotation change...");
                    const CRGB *artPtr = findArtByName(currentArtName);
                     if (artPtr) {
                         displayPixelArt(artPtr); // Function is now in animations.h
                         FastLED.show();
                     }
                } else if (currentMode == MODE_GAME_OF_LIFE && gol_initialized) {
                     Serial.println("  Redrawing Game of Life due to rotation change...");
                     initializeGolGrid(); // Function is now in animations.h
                     for (int i = 0; i < NUM_LEDS; i++) {
                         leds[i] = (gol_grid[i] == 1) ? GOL_ALIVE : GOL_DEAD; // Use GoL colors from animations.h
                     }
                     FastLED.show();
                     lastGolUpdate = millis(); // Reset timer to avoid immediate update
                } else if (currentMode == MODE_WEATHER) { // NEW: Redraw weather icon
                     Serial.println("  Redrawing weather icon due to rotation change...");
                     animationWeather(); // Redraws current icon & shows
                } else if (currentMode == MODE_STATIC_COLOR) {
                    // No redraw needed for solid color, rotation doesn't change it.
                    // But we can still show it to confirm visually
                    fillMatrix(current_color);
                    FastLED.show();
                }
                // Other animations update on next frame anyway
            } else {
                 Serial.println("  Rotation unchanged.");
            }
             server.send(200, "text/plain", "OK");
        }
        // Error response already sent if angle was invalid
    } else {
        Serial.println("  Error: Missing 'angle' parameter.");
        server.send(400, "text/plain", "Missing 'angle' parameter");
    }
}


// Handler for requests to unknown URLs - No changes needed
void handleNotFound() {
  String path = server.uri(); // Get the requested path
  Serial.println("handleNotFound called for path: " + path);
  if (path.endsWith("favicon.ico")) {
      server.send(204); // No Content response for favicon
  } else {
      server.send(404, "text/plain", "404: Not Found");
  }
}
