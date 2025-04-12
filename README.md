# 🌈 ESP32 WiFi LED Matrix Controller 🌦️🖼️

[![Matrix Demo](placeholder.gif)](placeholder.gif) <!-- TODO: Replace placeholder.gif with an actual demo image/GIF! -->

A feature-rich controller for an 8x8 WS2812B LED matrix powered by an ESP32, controllable via a web interface over WiFi. Display static colors, animations, pixel art, moods, Game of Life, and even live weather icons!

## ✨ Features

*   **Web Interface:** Control the matrix from any device on your network using a clean, responsive web UI.
*   **Multiple Display Modes:**
    *   **Static Color:** Set the entire matrix to a solid color using a color picker.
    *   **Animations:** Rainbow Cycle, Confetti, Palette Blend, Noise, Color Wipe, Bouncing Pixel, Fire.
    *   **Pixel Art:** Display predefined 8x8 images (Heart, Smile, Arrows, Pacman, Ghost).
    *   **Moods:** Display custom pixel art representing moods (e.g., Octopus faces).
    *   **Game of Life:** Conway's Game of Life simulation.
    *   **Weather Display:** Fetches live weather from OpenWeatherMap and displays a corresponding icon (clear, clouds, rain, snow, etc., with day/night variations).
    *   **Icon Test:** Cycles through all available pixel art and weather icons.
*   **Display Rotation:** Rotate the output 0, 90, 180, or 270 degrees via the web UI.
*   **WiFi Configuration:** Easily set your WiFi credentials.
*   **Weather Configuration:** Configure your OpenWeatherMap API key and location.
*   **Modular Code:** Separated logic for core control (`Matrix.ino`), animations/display logic (`animations.h`), and pixel art definitions (`pixel_art.h`).

## 🛠️ Hardware Requirements

*   **ESP32 Development Board:** Any standard ESP32 board should work.
*   **8x8 WS2812B LED Matrix:** A 64-pixel individually addressable LED matrix.
*   **Logic Level Shifter (Recommended):** ESP32 operates at 3.3V, while WS2812B LEDs typically expect 5V logic. A level shifter (e.g., 74AHCT125) on the data line improves stability.
*   **Power Supply:** A 5V power supply capable of delivering enough current for the matrix (64 LEDs * ~50mA/LED at full white brightness ≈ 3.2A recommended, though typical usage is lower). **Do not power the matrix directly from the ESP32's 5V pin!**
*   **Wiring:** Jumper wires, potentially soldering equipment.
*   **Optional:** Capacitor (1000uF) across the power lines near the matrix, resistor (300-500 Ohm) on the data line near the ESP32.

## 💻 Software Requirements

*   **Arduino IDE:** Or a compatible alternative like PlatformIO.
*   **ESP32 Board Support:** Install the ESP32 board definitions in your IDE. ([Instructions for Arduino IDE](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html))
*   **Libraries:** Install the following libraries via the Arduino Library Manager (Sketch -> Include Library -> Manage Libraries...):
    *   `FastLED` by Daniel Garcia
    *   `ArduinoJson` by Benoit Blanchon
*   **Required System Libraries (usually included with ESP32 core):**
    *   `WiFi.h`
    *   `WebServer.h`
    *   `HTTPClient.h`

## ⚙️ Configuration

Before uploading, you **MUST** configure the settings in `Matrix.ino/Matrix/Matrix.ino`:

1.  **WiFi Credentials:**
    ```cpp
    // ---!!! IMPORTANT: CONFIGURE WIFI HERE !!!---
    const char* ssid = "YOUR_WIFI_SSID";      // <<< CHANGE THIS
    const char* password = "YOUR_WIFI_PASSWORD"; // <<< CHANGE THIS
    // -------------------------------------------
    ```
2.  **Weather Settings:**
    *   Get a free API key from [OpenWeatherMap](https://openweathermap.org/appid).
    ```cpp
    // ---!!! IMPORTANT: CONFIGURE WEATHER HERE !!!---
    String openWeatherMapApiKey = "YOUR_API_KEY"; // <<< CHANGE THIS
    String city = "YourCity";                    // <<< CHANGE THIS
    String countryCode = "US";                 // <<< CHANGE THIS (e.g., US, GB, DE)
    #define WEATHER_UNITS "metric"             // "metric" (Celsius) or "imperial" (Fahrenheit)
    #define WEATHER_UPDATE_INTERVAL_SECS 900   // Update interval (15 minutes default)
    // -------------------------------------------
    ```
3.  **LED Data Pin:**
    ```cpp
    // --- LED Matrix Settings ---
    #define DATA_PIN    4       // Pin connected to DIN (Change if needed)
    ```

## 🚀 Setup & Usage

1.  **Connect Hardware:**
    *   Connect the ESP32's `GND` to the power supply `GND` and the matrix `GND`.
    *   Connect the power supply `5V` to the matrix `5V` input. **Do NOT connect matrix 5V to ESP32 Vin/5V.**
    *   Connect the ESP32 `DATA_PIN` (GPIO 4 by default) to the matrix `DIN` (Data Input), preferably via a logic level shifter.
    *   Power the ESP32 via its USB port or a separate power input.
2.  **Configure:** Edit `Matrix.ino` as described in the Configuration section.
3.  **Upload:** Select your ESP32 board and port in the Arduino IDE and upload the `Matrix.ino` sketch.
4.  **Find IP Address:** Open the Arduino Serial Monitor (set baud rate to 115200). After connecting to WiFi, the ESP32 will print its IP address:
    ```
    Setup: WiFi connected successfully.
    Setup: IP address: http://192.168.1.XXX
    ```
5.  **Control via Web:** Open a web browser on a device connected to the *same WiFi network* and navigate to the IP address printed in the Serial Monitor (e.g., `http://192.168.1.XXX`).
6.  **Use UI:** Use the buttons and color picker on the web page to control the matrix modes and settings.

## 📁 File Structure

*   `Matrix.ino/Matrix.ino`: Main sketch file handling WiFi, web server setup, mode switching, and the main loop. Contains core configuration.
*   `Matrix.ino/Matrix/animations.h`: Contains functions for all animation modes, Game of Life logic, weather display logic, icon testing, coordinate mapping (`XY`), and helper functions (`findArtByName`).
*   `Matrix.ino/Matrix/pixel_art.h`: Contains the actual `CRGB` array definitions for all predefined pixel art and weather icons.

## 🎨 Adding Custom Pixel Art

1.  **Define Art:** Open `Matrix.ino/Matrix/pixel_art.h`. Define a new `CRGB` array for your 8x8 art. Use color names (e.g., `CRGB::Red`) or hex values (e.g., `CRGB(0xFF00FF)`). `B` represents Black (off).
    ```cpp
    // Example: Define a new 'rocket' icon
    const CRGB rocket[NUM_LEDS] = {
      B, B, B, R, R, B, B, B, // R = Red, W = White, O = Orange
      B, B, R, W, W, R, B, B,
      B, B, R, W, W, R, B, B,
      B, R, W, W, W, W, R, B,
      B, R, W, W, W, W, R, B,
      R, W, W, W, W, W, W, R,
      B, B, O, O, O, O, B, B,
      B, B, O, O, O, O, B, B
    };
    ```
2.  **Declare Extern:** Add an `extern const CRGB yourArtName[NUM_LEDS];` line in `Matrix.ino/Matrix/animations.h` within the `Extern pixel art arrays` section.
    ```cpp
    // --- Extern pixel art arrays (defined in pixel_art.h) ---
    // ... existing externs ...
    extern const CRGB weather_mist[NUM_LEDS];
    extern const CRGB rocket[NUM_LEDS]; // <-- Add your new one
    ```
3.  **Add to `findArtByName`:** In `Matrix.ino/Matrix/animations.h`, add an `if` condition to the `findArtByName` function to map a name string to your new array.
    ```cpp
    const CRGB* findArtByName(String name) {
        // ... existing checks ...
        if (name.equalsIgnoreCase("mist")) return weather_mist;
        if (name.equalsIgnoreCase("error")) return weather_error;
        if (name.equalsIgnoreCase("rocket")) return rocket; // <-- Add your new one

        return nullptr; // Not found
    }
    ```
4.  **Add to Web UI (Optional):** In `Matrix.ino`, within the `handleRoot` function, add a new link button for your art:
    ```html
     <a href="/setart?name=rocket" class="button-link"><span class="emoji">🚀</span>Rocket</a>
    ```
5.  **Add to Icon Test (Optional):** In `Matrix.ino/Matrix/animations.h`, add the name string to the `allArtNames` array used by `animationIconTest`.
    ```cpp
    const String allArtNames[] = {
        // ... existing names ...
        "thunderstorm", "snow", "mist", "error", "rocket" // <-- Add your new one
    };
    ```

## 🐛 Troubleshooting

*   **No Web Interface:** Check Serial Monitor for WiFi connection status and the correct IP address. Ensure your control device is on the same network.
*   **Weather Icon is 'error':**
    *   Check Serial Monitor for WiFi connection errors during startup.
    *   Verify your OpenWeatherMap API key, City, and Country Code in `Matrix.ino`. Ensure the API key is active.
    *   Check Serial Monitor for HTTP error codes (like 401 Unauthorized - bad API key, 404 Not Found - bad city, 400 Bad Request - formatting issue).
*   **LEDs Don't Light Up / Flickering:**
    *   Check wiring, especially GND connections.
    *   Ensure adequate 5V power supply directly to the matrix.
    *   Verify the `DATA_PIN` setting in `Matrix.ino`.
    *   Try adding a logic level shifter if not already using one.
*   **Wrong Colors / Order:** Check the `COLOR_ORDER` setting in `Matrix.ino` (common options are `GRB`, `RGB`, `BRG`).

---

Enjoy your dynamic LED Matrix! ✨ 