#include <FastLED.h>

#define PIN_BUTTON  2
#define PIN_LED     3
#define NUM_LEDS    12

#define DEF_BRIGHTNESS 255

#define STATE_OFF   0
#define STATE_ON1   1
#define STATE_ON2   2
#define STATE_ON3   3
#define STATE_FLAME 4

CRGB leds[NUM_LEDS];

byte APP_STATE = 0;
int ledColorR = 255;
int ledColorG = 53;
int ledColorB = 0;

// Flame effect variables
byte flameBright = 0;
bool flameBrightIncr = true;
byte flameBrightIncrease = 3;
byte flamePeriod = 0;
unsigned long timerFlame = 0;

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), handleButtonPress, RISING);

  FastLED.addLeds<WS2812, PIN_LED, RGB>(leds, NUM_LEDS);
  clearLeds();

  setAppState(1);
}

void loop() {
  if (APP_STATE == STATE_FLAME) {
    runFlameEffect();
  }
}

// --- LED Effects ---
void runOnEffect() {
  byte brightness = 0;
  switch (APP_STATE) {
    case STATE_ON1:
      brightness = 50;
      break;
    case STATE_ON2:
      brightness = 150;
      break;
    case STATE_ON3:
      brightness = 255;
      break;
    default:
      brightness = DEF_BRIGHTNESS;
      break;
  }
  for (byte i = 0; i < NUM_LEDS; i++) {
      setLed(i, ledColorR, ledColorG, ledColorB, brightness);
  }
  FastLED.show();
  delay(100);
}



void runFlameEffect() {
  if (millis() >= timerFlame + flamePeriod) {

    byte lastBright = flameBright;
    bool lastIncr = flameBrightIncr;

    if (flameBrightIncr) {
      flameBright += flameBrightIncrease;
    } else {
      flameBright -= flameBrightIncrease;
    }
  
    if (flameBright >= 220) {
      flameBrightIncr = false;
      flameBright = 220;
    }
    if (flameBright <= 40) {
      flameBrightIncr = true;
      flameBright = 40;
    }

    while (lastBright != flameBright) {
      for (byte i = 0; i < NUM_LEDS; i++) {
          setLed(i, ledColorR, ledColorG, ledColorB, lastBright);
      }
      FastLED.show();

      if (lastIncr) lastBright++;
      else lastBright--;      
    }

    timerFlame += flamePeriod;

    flamePeriod = random(30, 150);
  }
}

// --- App state ---
void setAppState(byte state) {
  clearLeds();
  if (APP_STATE != state) {
    APP_STATE = state;
  }

  switch (APP_STATE) {
    case STATE_ON1:
    case STATE_ON2:
    case STATE_ON3:
      runOnEffect();
      break;
    case STATE_FLAME:
      // runFlameEffect();
      timerFlame = millis() - 250;
      flameBright = 150;
      flameBrightIncr = false;
      break;
    default:
      break;
  }
}

// --- Button handler ---
void handleButtonPress() {
  byte nextState = APP_STATE + 1;
  if (nextState > 4) nextState = 0;
  setAppState(nextState);
}

// --- LED utility functions ---
void clearLeds() {
  for (byte i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
}
void setLed(byte led, byte r, byte g, byte b) {
  leds[led] = CRGB(g, r, b);
  leds[led].maximizeBrightness(DEF_BRIGHTNESS);
}
void setLed(byte led, byte r, byte g, byte b, byte brightness) {
  leds[led] = CRGB(g, r, b);
  leds[led].maximizeBrightness(brightness);
}
