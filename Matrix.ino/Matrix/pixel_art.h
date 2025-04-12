#ifndef PIXEL_ART_H
#define PIXEL_ART_H

#include <FastLED.h>

// --- LED Matrix Settings (Copied for self-containment) ---
#define NUM_LEDS    64      // 8x8 matrix
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8

// --- Pixel Art & Mood Data ---
// Define colors for easier use in pixel art arrays
#define B CRGB::Black
#define R CRGB::Red
#define Y CRGB::Yellow
#define G CRGB::Green
#define W CRGB::White
#define L CRGB::Blue // Using L for bLue to avoid clash with B (Black)
#define P CRGB::Purple
#define O CRGB::Orange
#define C CRGB::Cyan
#define DP CRGB::DeepPink // For Octopus

// NEW Weather Icon Colors
#define W_SUN CRGB::Yellow
#define W_CLOUD CRGB::WhiteSmoke
#define W_RAIN CRGB::Blue
#define W_SNOW CRGB::White
#define W_LIGHTNING CRGB::Yellow
#define W_MIST CRGB(192, 192, 192)
#define W_NIGHT CRGB::DarkBlue
#define W_MOON CRGB::LightYellow

// Note: These are defined directly here. Ensure this header is included only once or appropriately managed.
const CRGB heart[NUM_LEDS] = {
  B, B, R, R, B, R, R, B, B, R, R, R, R, R, R, B, B, R, R, R, R, R, R, B, B, B, R, R, R, R, B, B,
  B, B, B, R, R, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B };
const CRGB smile[NUM_LEDS] = {
  B, Y, Y, B, B, Y, Y, B, Y, B, B, Y, Y, B, B, Y, Y, B, B, Y, Y, B, B, Y, B, Y, Y, B, B, Y, Y, B,
  B, B, B, B, B, B, B, B, B, R, B, B, B, B, R, B, B, B, R, R, R, R, B, B, B, B, B, B, B, B, B, B };
const CRGB arrowUp[NUM_LEDS] = { // This art should help test rotation
  B, B, B, Y, Y, B, B, B, B, B, Y, Y, Y, Y, B, B, B, Y, Y, Y, Y, Y, Y, B, B, B, B, Y, Y, B, B, B,
  B, B, B, Y, Y, B, B, B, B, B, B, Y, Y, B, B, B, B, B, B, Y, Y, B, B, B, B, B, B, B, B, B, B, B };
const CRGB pacman[NUM_LEDS] = {
  B, Y, Y, Y, Y, Y, B, B, Y, Y, Y, Y, Y, Y, Y, B, Y, Y, Y, Y, Y, B, B, B, Y, Y, Y, Y, B, B, B, B,
  Y, Y, Y, Y, Y, B, B, B, Y, Y, Y, Y, Y, Y, Y, B, B, Y, Y, Y, Y, Y, B, B, B, B, B, B, B, B, B, B };
const CRGB ghost[NUM_LEDS] = {
  B, R, R, R, R, R, R, B, R, R, W, R, R, W, R, R, R, R, B, R, R, B, R, R, R, R, R, R, R, R, R, R,
  R, R, R, R, R, R, R, R, R, B, R, B, R, B, R, B, B, R, B, R, B, R, B, R, B, B, B, B, B, B, B, B };

// Octopus Moods
const CRGB octopus_neutral[NUM_LEDS] = {
  B, B, P, P, P, P, B, B, B, P, P, W, W, P, P, B, P, P, B, P, P, B, P, P, P, P, P, P, P, P, P, P,
  B, P, W, P, P, W, P, B, B, P, P, P, P, P, P, B, B, B, P, B, B, P, B, B, B, B, P, B, B, P, B, B
};
const CRGB octopus_happy[NUM_LEDS] = {
  B, B, DP, DP, DP, DP, B, B, B, DP, DP, W, W, DP, DP, B, DP, DP, B, W, W, B, DP, DP, DP, DP, DP, DP, DP, DP, DP, DP,
  B, DP, W, DP, DP, W, DP, B, B, DP, DP, DP, DP, DP, DP, B, B, B, DP, B, B, DP, B, B, B, B, DP, B, B, DP, B, B
};
const CRGB octopus_sad[NUM_LEDS] = {
  B, B, L, L, L, L, B, B, B, L, L, W, W, L, L, B, L, L, W, B, B, W, L, L, L, L, L, L, L, L, L, L,
  B, L, B, L, L, B, L, B, B, L, L, L, L, L, L, B, B, B, L, B, B, L, B, B, B, B, L, B, B, L, B, B
};

// --- NEW Weather Icons ---
// Icons based on mapping in Matrix.ino -> mapConditionToIconName

const CRGB weather_error[NUM_LEDS] = { // Simple Red X
  R, B, B, B, B, B, B, R, B, R, B, B, B, B, R, B, B, B, R, B, B, R, B, B, B, B, B, R, R, B, B, B,
  B, B, B, R, R, B, B, B, B, B, R, B, B, R, B, B, B, R, B, B, B, B, R, B, R, B, B, B, B, B, B, R
};

const CRGB weather_clear_day[NUM_LEDS] = { // Sun
  B, B, B, W_SUN, W_SUN, B, B, B, B, B, W_SUN, W_SUN, W_SUN, W_SUN, B, B, B, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, B,
W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN,
  B, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, B, B, B, W_SUN, W_SUN, W_SUN, W_SUN, B, B, B, B, B, W_SUN, W_SUN, B, B, B
};

const CRGB weather_clear_night[NUM_LEDS] = { // Moon
  B, B, B, W_MOON, W_MOON, B, B, B, B, B, W_MOON, W_MOON, W_MOON, B, B, B, B, W_MOON, W_MOON, W_MOON, W_MOON, B, B, B,
 B, W_MOON, W_MOON, W_MOON, W_MOON, W_MOON, B, B, B, W_MOON, W_MOON, W_MOON, W_MOON, W_MOON, B, B, B, B, W_MOON, W_MOON, W_MOON, B, B, B,
  B, B, B, W_MOON, W_MOON, B, B, B, B, B, B, B, B, B, B, B
};

const CRGB weather_few_clouds_day[NUM_LEDS] = { // Sun behind small cloud
  B, B, W_SUN, W_SUN, B, B, B, B, B, W_SUN, W_SUN, W_SUN, W_SUN, B, B, B, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, B, W_CLOUD,
W_SUN, W_SUN, W_SUN, W_SUN, W_SUN, W_CLOUD, W_CLOUD, W_CLOUD, B, W_SUN, W_SUN, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, B, B, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD,
  B, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, B, B, B, B, W_CLOUD, W_CLOUD, W_CLOUD, B, B
};

const CRGB weather_few_clouds_night[NUM_LEDS] = { // Moon behind small cloud
  B, B, W_MOON, W_MOON, B, B, B, B, B, W_MOON, W_MOON, W_MOON, B, B, B, B, W_MOON, W_MOON, W_MOON, W_MOON, B, W_CLOUD, W_CLOUD, W_CLOUD,
W_MOON, W_MOON, W_MOON, W_MOON, W_MOON, W_CLOUD, W_CLOUD, W_CLOUD, B, W_MOON, W_MOON, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD,
 B, B, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, B, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, B,
  B, B, B, W_CLOUD, W_CLOUD, W_CLOUD, B, B
};

const CRGB weather_clouds[NUM_LEDS] = { // Full clouds
  B, B, B, B, B, B, B, B, B, W_CLOUD, W_CLOUD, B, B, W_CLOUD, W_CLOUD, B, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD,
W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD,
 W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, B, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, B,
  B, B, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, B, B
};

const CRGB weather_shower_rain[NUM_LEDS] = { // Cloud with heavier rain drops
  B, W_CLOUD, W_CLOUD, B, B, W_CLOUD, W_CLOUD, B, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD,
 W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, B, B, W_RAIN, B, W_RAIN, B, B, B,
  B, W_RAIN, B, W_RAIN, B, W_RAIN, B, B, B, W_RAIN, B, B, W_RAIN, B, W_RAIN, B,
  W_RAIN, B, W_RAIN, B, W_RAIN, B, B, W_RAIN
};

const CRGB weather_rain_day[NUM_LEDS] = { // Sun behind cloud with light rain
  B, B, W_SUN, W_SUN, B, B, B, B, B, W_SUN, W_SUN, W_SUN, W_SUN, B, B, B, W_SUN, W_SUN, W_SUN, W_CLOUD, W_CLOUD, W_CLOUD, B, B,
W_SUN, W_SUN, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, B, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD,
  B, W_RAIN, B, W_CLOUD, W_CLOUD, W_CLOUD, B, B, B, B, W_RAIN, B, W_RAIN, B, W_RAIN, B,
  B, W_RAIN, B, W_RAIN, B, B, W_RAIN, B
};

const CRGB weather_rain_night[NUM_LEDS] = { // Moon behind cloud with light rain
  B, B, W_MOON, W_MOON, B, B, B, B, B, W_MOON, W_MOON, W_MOON, B, B, B, B, W_MOON, W_MOON, W_CLOUD, W_CLOUD, W_CLOUD, B, B, B,
W_MOON, W_MOON, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, B, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD,
  B, W_RAIN, B, W_CLOUD, W_CLOUD, W_CLOUD, B, B, B, B, W_RAIN, B, W_RAIN, B, W_RAIN, B,
  B, W_RAIN, B, W_RAIN, B, B, W_RAIN, B
};

const CRGB weather_thunderstorm[NUM_LEDS] = { // Cloud with lightning bolt
  B, W_CLOUD, W_CLOUD, B, B, W_CLOUD, W_CLOUD, B, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD,
 W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, B, B, B, W_LIGHTNING, B, B, B, B,
  B, B, W_LIGHTNING, W_LIGHTNING, B, B, B, B, B, W_LIGHTNING, W_LIGHTNING, W_LIGHTNING, B, B, B, B,
  B, B, W_LIGHTNING, B, B, B, B, B
};

const CRGB weather_snow[NUM_LEDS] = { // Cloud with snowflakes
  B, W_CLOUD, W_CLOUD, B, B, W_CLOUD, W_CLOUD, B, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD,
 W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, W_CLOUD, B, W_SNOW, B, B, W_SNOW, B, B, B,
  W_SNOW, B, W_SNOW, B, B, W_SNOW, B, W_SNOW, B, B, W_SNOW, B, W_SNOW, B, B, W_SNOW,
  B, W_SNOW, B, B, W_SNOW, B, W_SNOW, B
};

const CRGB weather_mist[NUM_LEDS] = { // Grey lines for mist/fog
  W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, B, B, B, B, B, B, B, B, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST,
 B, B, B, B, B, B, B, B, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, B, B, B, B, B, B, B, B,
 W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, W_MIST, B, B, B, B, B, B, B, B
};


#endif // PIXEL_ART_H 