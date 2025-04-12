#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <FastLED.h>
#include <stdlib.h> // For random()

// --- Required constants (Copied for self-containment) ---
#define NUM_LEDS    64
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8

// --- Extern variables (defined in Matrix.ino) ---
// These variables are declared here so the functions in this file can use them,
// but they are actually defined and stored in the main Matrix.ino file.
extern CRGB leds[NUM_LEDS];          // The array of LED colors
extern CRGB current_color;           // The currently selected static color (used by wipe)
extern int currentRotation;          // The current display rotation (used by XY)
extern byte gol_grid[NUM_LEDS];      // Game of Life current state grid
extern byte gol_next_grid[NUM_LEDS]; // Game of Life next state buffer
extern bool gol_initialized;         // Game of Life initialization flag
extern unsigned long lastGolUpdate;  // Timer for Game of Life updates
extern String weather_icon_name;      // NEW: Need access to the current weather icon name

// --- Extern functions (defined in Matrix.ino) ---
extern void fillMatrix(CRGB color); // Used by displayPixelArt, ColorWipe, GoL init

// --- Extern pixel art arrays (defined in pixel_art.h) --- NEW
// This tells the compiler these exist, even though they are defined elsewhere.
extern const CRGB heart[NUM_LEDS];
extern const CRGB smile[NUM_LEDS];
extern const CRGB arrowUp[NUM_LEDS];
extern const CRGB pacman[NUM_LEDS];
extern const CRGB ghost[NUM_LEDS];
extern const CRGB octopus_neutral[NUM_LEDS];
extern const CRGB octopus_happy[NUM_LEDS];
extern const CRGB octopus_sad[NUM_LEDS];
extern const CRGB weather_error[NUM_LEDS];
extern const CRGB weather_clear_day[NUM_LEDS];
extern const CRGB weather_clear_night[NUM_LEDS];
extern const CRGB weather_few_clouds_day[NUM_LEDS];
extern const CRGB weather_few_clouds_night[NUM_LEDS];
extern const CRGB weather_clouds[NUM_LEDS];
extern const CRGB weather_shower_rain[NUM_LEDS];
extern const CRGB weather_rain_day[NUM_LEDS];
extern const CRGB weather_rain_night[NUM_LEDS];
extern const CRGB weather_thunderstorm[NUM_LEDS];
extern const CRGB weather_snow[NUM_LEDS];
extern const CRGB weather_mist[NUM_LEDS];

// --- Game of Life Settings ---
#define GOL_UPDATE_INTERVAL 150 // Milliseconds between GoL steps
#define GOL_ALIVE CRGB::Red     // GoL Alive color
#define GOL_DEAD CRGB::Black    // GoL Dead color

// --- Palettes ---
DEFINE_GRADIENT_PALETTE( my_palette_gp ) { // Example Gradient Palette
    0,     0,   0, 255,   // Blue
   85,   255,   0,   0,   // Red
  170,   255, 255,   0,   // Yellow
  255,     0, 255,   0 }; // Green

DEFINE_GRADIENT_PALETTE( fire_gp ) { // Fire Palette
    0,     0,   0,   0,   // Black
  100,   100,   0,   0,   // Dark Red
  150,   255,  80,   0,   // Orange
  200,   255, 255,  40,   // Yellow
  255,   255, 255, 255 }; // White

// --- Helper to find Pixel Art by Name --- MOVED here from Matrix.ino
const CRGB* findArtByName(String name) {
    if (name.equalsIgnoreCase("heart")) return heart;
    if (name.equalsIgnoreCase("smile")) return smile;
    if (name.equalsIgnoreCase("arrow_up")) return arrowUp;
    if (name.equalsIgnoreCase("pacman")) return pacman;
    if (name.equalsIgnoreCase("ghost")) return ghost;
    if (name.equalsIgnoreCase("octopus_neutral")) return octopus_neutral;
    if (name.equalsIgnoreCase("octopus_happy")) return octopus_happy;
    if (name.equalsIgnoreCase("octopus_sad")) return octopus_sad;
    // Weather icons
    if (name.equalsIgnoreCase("clear_day")) return weather_clear_day;
    if (name.equalsIgnoreCase("clear_night")) return weather_clear_night;
    if (name.equalsIgnoreCase("few_clouds_day")) return weather_few_clouds_day;
    if (name.equalsIgnoreCase("few_clouds_night")) return weather_few_clouds_night;
    if (name.equalsIgnoreCase("clouds")) return weather_clouds;
    if (name.equalsIgnoreCase("shower_rain")) return weather_shower_rain;
    if (name.equalsIgnoreCase("rain_day")) return weather_rain_day;
    if (name.equalsIgnoreCase("rain_night")) return weather_rain_night;
    if (name.equalsIgnoreCase("thunderstorm")) return weather_thunderstorm;
    if (name.equalsIgnoreCase("snow")) return weather_snow;
    if (name.equalsIgnoreCase("mist")) return weather_mist;
    if (name.equalsIgnoreCase("error")) return weather_error;

    return nullptr; // Not found
}

// --- Coordinate Mapping & Display Utilities ---

// Helper function to map logical X, Y coordinates to a linear index
// Accounts for the currentRotation setting.
uint16_t XY( uint8_t x, uint8_t y) {
  if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) {
    return NUM_LEDS; // Out of bounds
  }

  uint8_t rotated_x = x;
  uint8_t rotated_y = y;

  // Apply rotation based on currentRotation setting
  switch (currentRotation) {
    case 1: // 90 degrees CW
      rotated_x = MATRIX_HEIGHT - 1 - y;
      rotated_y = x;
      break;
    case 2: // 180 degrees
      rotated_x = MATRIX_WIDTH - 1 - x;
      rotated_y = MATRIX_HEIGHT - 1 - y;
      break;
    case 3: // 270 degrees CW (or 90 CCW)
      rotated_x = y;
      rotated_y = MATRIX_WIDTH - 1 - x;
      break;
    case 0: // 0 degrees
    default:
      rotated_x = x;
      rotated_y = y;
      break;
  }

  // Calculate the index using simple row-major mapping on the rotated coordinates
  uint16_t index = (rotated_y * MATRIX_WIDTH) + rotated_x;

  if (index >= NUM_LEDS) {
      // Avoid Serial print in header if possible, or make it conditional
      // Serial.printf("Error: XY rotation calc OOB: %d for (%d, %d) rot %d\n", index, x, y, currentRotation);
      return NUM_LEDS; // Indicate error
  }
  return index;
}

// Helper function to map linear index back to logical X, Y coordinates
// Accounts for the currentRotation setting.
// NOTE: Not currently used by core logic but might be useful.
void IndexToXY(uint16_t index, uint8_t &x, uint8_t &y) {
    if (index >= NUM_LEDS) {
        x = 255; y = 255; return;
    }
    uint8_t rotated_y = index / MATRIX_WIDTH;
    uint8_t rotated_x = index % MATRIX_WIDTH;
    switch (currentRotation) {
        case 1: x = rotated_y; y = MATRIX_HEIGHT - 1 - rotated_x; break;
        case 2: x = MATRIX_WIDTH - 1 - rotated_x; y = MATRIX_HEIGHT - 1 - rotated_y; break;
        case 3: x = MATRIX_WIDTH - 1 - rotated_y; y = rotated_x; break;
        case 0: default: x = rotated_x; y = rotated_y; break;
    }
    if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) {
        // Serial.printf("Error: IndexToXY OOB: (%d, %d) for index %d\n", x, y, index);
        x = 255; y = 255;
    }
}

// Displays a predefined pixel art array onto the matrix using the XY mapping.
void displayPixelArt(const CRGB art[NUM_LEDS]) {
  fillMatrix(CRGB::Black); // Clear matrix first using the function from Matrix.ino
  for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
      for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
          uint16_t sourceIndex = (y * MATRIX_WIDTH) + x;
          uint16_t destIndex = XY(x, y); // Calculate the destination index using rotation
          if (destIndex < NUM_LEDS) { leds[destIndex] = art[sourceIndex]; }
      }
  }
  // FastLED.show() is called by the handler in Matrix.ino after calling this
}


// --- Animation Functions ---
void animationRainbowCycle() {
  static uint8_t rainbowHue = 0;
  fill_rainbow(leds, NUM_LEDS, rainbowHue++, 7);
}

void animationConfetti() {
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(random8(), 200, 255);
}

void animationPalette() {
  CRGBPalette16 currentPalette = my_palette_gp; // Use the example palette
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1;
  fill_palette( leds, NUM_LEDS, startIndex, 16, currentPalette, 255, LINEARBLEND);
}

void animationNoise() {
  static uint16_t noiseX = random16();
  static uint16_t noiseZ = random16();
  uint8_t scale = 30;
  for(int i = 0; i < NUM_LEDS; i++) {
    uint8_t noise = inoise8(noiseX + i * scale, noiseZ);
    leds[i] = CHSV(noise, 200, 255);
  }
  noiseZ += 10;
}

void animationColorWipe() {
  static uint16_t wipeIndex = 0;
  static bool wipeComplete = true;
  static CRGB wipeColor;

  if (wipeComplete) {
    wipeIndex = 0;
    wipeComplete = false;
    wipeColor = current_color; // Use the global current color from Matrix.ino
    fillMatrix(CRGB::Black); // Use global fillMatrix
    FastLED.show(); // Show cleared screen immediately
  }

  if (wipeIndex < NUM_LEDS) {
    leds[wipeIndex] = wipeColor;
    wipeIndex++;
  } else {
    wipeComplete = true;
  }
}

void animationBouncingPixel() {
    static int bouncePos = 0;
    static int bounceDir = 1;
    static CRGB bounceColor = CHSV(random8(), 255, 255);

    fadeToBlackBy(leds, NUM_LEDS, 40);
    if (bouncePos >= 0 && bouncePos < NUM_LEDS) {
      leds[bouncePos] = bounceColor;
    }
    bouncePos += bounceDir;

    if (bouncePos >= NUM_LEDS) {
      bouncePos = NUM_LEDS - 2;
      bounceDir = -1;
      bounceColor = CHSV(random8(), 255, 255);
    } else if (bouncePos < 0) {
      bouncePos = 1;
      bounceDir = 1;
      bounceColor = CHSV(random8(), 255, 255);
    }
}

void animationFire() {
    static byte heat[NUM_LEDS];
    CRGBPalette16 firePalette = fire_gp; // Use the fire palette defined above

    // Cool down
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((100 / MATRIX_HEIGHT) + 2)));
    }

    // Heat from bottom (logical)
    for (uint8_t lx = 0; lx < MATRIX_WIDTH; lx++) {
       uint16_t index = XY(lx, MATRIX_HEIGHT - 1); // Get index for the logical bottom row
       if (index < NUM_LEDS) {
            heat[index] = qadd8( heat[index], random8(70, 130) );
       }
    }

    // Diffuse/Blur (This simple version might still look weird with rotation)
    // A more robust fire effect would operate purely on logical grid coords before mapping.
    for( int k = NUM_LEDS - 1; k >= 1; k--) {
        heat[k] = (heat[k-1] + heat[k-1] + heat[k]) / 3;
    }

    // Map heat to colors
    for( int j = 0; j < NUM_LEDS; j++) {
      byte colorindex = scale8( heat[j], 240);
      leds[j] = ColorFromPalette( firePalette, colorindex);
    }
}


// --- Game of Life Functions ---

// Initialize the Game of Life grid with a random pattern
void initializeGolGrid() {
  // Serial.println("Initializing Game of Life grid (from animations.h)...");
  randomSeed(millis());
  for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
      for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
          uint16_t index = XY(x, y); // Map logical to physical index
          if (index < NUM_LEDS) {
              // Initialize gol_grid based on the calculated physical index
              gol_grid[index] = (random(10) < 3) ? 1 : 0;
              gol_next_grid[index] = 0; // Clear buffer
          }
      }
  }
  gol_initialized = true; // Set the global flag
  // Serial.println("Game of Life grid initialized (from animations.h).");
}

// Count live neighbors for a cell at logical (x, y) with wrap-around
int countLiveNeighbors(int logical_x, int logical_y) {
  int count = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (i == 0 && j == 0) continue; // Skip self

      int nlx = (logical_x + i + MATRIX_WIDTH) % MATRIX_WIDTH;
      int nly = (logical_y + j + MATRIX_HEIGHT) % MATRIX_HEIGHT;

      uint16_t neighborIndex = XY(nlx, nly); // Get physical index of neighbor

      // Check state in the *current* grid using the physical index
      if (neighborIndex < NUM_LEDS && gol_grid[neighborIndex] == 1) {
        count++;
      }
    }
  }
  return count;
}

// Update and display the Game of Life simulation
void animationGameOfLife() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastGolUpdate >= GOL_UPDATE_INTERVAL) {
    lastGolUpdate = currentMillis;

    if (!gol_initialized) {
        initializeGolGrid();
        // Display initial state immediately (using physical indices)
        for (int i = 0; i < NUM_LEDS; i++) {
             leds[i] = (gol_grid[i] == 1) ? GOL_ALIVE : GOL_DEAD;
        }
        FastLED.show();
        return; // Skip update on first run
    }

    // Calculate next generation based on logical coordinates
    for (uint8_t ly = 0; ly < MATRIX_HEIGHT; ly++) {
      for (uint8_t lx = 0; lx < MATRIX_WIDTH; lx++) {
        uint16_t index = XY(lx, ly); // Physical index for this logical cell
        if (index >= NUM_LEDS) continue;

        int neighbors = countLiveNeighbors(lx, ly);
        byte currentState = gol_grid[index]; // Read current state from physical index
        byte nextState = currentState;

        if (currentState == 1 && (neighbors < 2 || neighbors > 3)) nextState = 0;
        else if (currentState == 0 && neighbors == 3) nextState = 1;

        gol_next_grid[index] = nextState; // Store next state at physical index
      }
    }

    // Copy next grid to current grid
    memcpy(gol_grid, gol_next_grid, sizeof(gol_grid));

    // Update LEDs based on the *new* current grid state (using physical indices)
    for (int i = 0; i < NUM_LEDS; i++) {
       leds[i] = (gol_grid[i] == 1) ? GOL_ALIVE : GOL_DEAD;
    }

    FastLED.show(); // Show the updated state
  }
  // No FastLED.delay() needed here
}

// --- NEW Weather Display Function ---
void animationWeather() {
    // Serial.println("animationWeather: Called."); // <-- REMOVED LOG
    // This function is called repeatedly when in MODE_WEATHER.
    // It just needs to display the *current* icon based on the last fetch.
    // The actual fetching happens periodically in the main loop.

    // Serial.printf("animationWeather: Attempting to display icon: '%s'\n", weather_icon_name.c_str()); // <-- REMOVED LOG
    const CRGB* iconPtr = findArtByName(weather_icon_name); // Find pointer using helper

    if (iconPtr) {
        displayPixelArt(iconPtr); // Display the found icon
        // Serial.println("animationWeather: Icon found and displayed."); // <-- REMOVED LOG
    } else {
        // Fallback: If the icon name is somehow invalid, display the error icon
        displayPixelArt(weather_error);
        Serial.printf("Weather Error: Icon '%s' not found! Displaying error icon.\n", weather_icon_name.c_str()); // <-- KEPT ERROR LOG
    }
    FastLED.show(); // Show the displayed icon
    // Serial.println("animationWeather: FastLED.show() called."); // <-- REMOVED LOG

    // Since the icon only changes when weather is fetched, we don't need rapid updates.
    // Add a small delay to prevent this function hogging the CPU if needed,
    // although FastLED.show() has some built-in delay.
    // If other background tasks were added, a delay here might be useful.
    delay(100); // Small delay for weather mode
}

// --- NEW Icon Test Function ---
#define ICON_TEST_INTERVAL 2000 // Milliseconds between icon changes

// Array of all known art names (must match findArtByName cases)
const String allArtNames[] = {
    "heart", "smile", "arrow_up", "pacman", "ghost",
    "octopus_neutral", "octopus_happy", "octopus_sad",
    "clear_day", "clear_night", "few_clouds_day", "few_clouds_night",
    "clouds", "shower_rain", "rain_day", "rain_night",
    "thunderstorm", "snow", "mist", "error"
};
const int numAllArtNames = sizeof(allArtNames) / sizeof(allArtNames[0]);

void animationIconTest() {
    static int currentArtIndex = 0;
    static unsigned long lastIconChange = 0;
    unsigned long currentMillis = millis();

    // Check if it's time to change the icon
    if (currentMillis - lastIconChange >= ICON_TEST_INTERVAL) {
        lastIconChange = currentMillis;

        String artName = allArtNames[currentArtIndex];
        const CRGB* artPtr = findArtByName(artName);

        Serial.printf("IconTest: Displaying [%d/%d] '%s'\n", currentArtIndex + 1, numAllArtNames, artName.c_str());

        if (artPtr) {
            displayPixelArt(artPtr);
        } else {
            displayPixelArt(weather_error); // Should not happen if list is correct
            Serial.printf("IconTest Error: Art '%s' not found!\n", artName.c_str());
        }
        FastLED.show();

        // Move to the next index, wrapping around
        currentArtIndex = (currentArtIndex + 1) % numAllArtNames;
    }
    // No FastLED.delay() needed here, timing is handled by the interval check
}

#endif // ANIMATIONS_H 