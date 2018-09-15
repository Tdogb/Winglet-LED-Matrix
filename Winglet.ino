#include "FastLED.h"
FASTLED_USING_NAMESPACE
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif


#define DATA_PIN    5
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    32
#define BRIGHTNESS          100
#define FRAMES_PER_SECOND  60
CRGB leds[NUM_LEDS];
const int offsetLUT[6] = {0, 4, 9, 15, 21, 26}; //Each of these are indexes where each row of the LED matrix starts

int currentPos[6] = {0,0,0,0,0,0}; //The number of the LED which the animation is currently on
bool transitioningLED[6] = {0,0,0,0,0,0};//If the LED has changed 
int counter[6] = {0,0,0,0,0,0}; //Counting until when to move on to next LED
bool colorChange[6] = {0,1,0,1,0,1}; //What color each LED is. 0 is Red and 1 is White

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  EVERY_N_MILLISECONDS((int)(1000/FRAMES_PER_SECOND)) {nextLEDFrame();}
  for(int i = 0; i < 6; i++) {
    for(int n = 0; n < BRIGHTNESS; n += 5) {
      //FastLED.delay(1);
      leds[offsetLUT[i] + currentPos[i]].nscale8((transitioningLED[i] ? n : BRIGHTNESS)); //Sets the value of the LED to the for loop, otherwise keeps it the same
      FastLED.show();
    }
  }
}

void nextLEDFrame() {
  row(0, 10, 4, offsetLUT[0], 0);
  row(1, 8, 5, offsetLUT[1], 1);
  row(2, 9, 6, offsetLUT[2], 0);
  row(3, 4, 6, offsetLUT[3], 1);
  row(4, 9, 5, offsetLUT[4], 0);
  row(5, 10, 4, offsetLUT[5], 1);
  FastLED.show();
}
/**
  @param rate Units: frames per second. From 5 to 10 (doesn't have to be this range)
  @param offset Number of LEDS is the offset
  @param totalOffset only used when writing to the LEDs
**/
void row(int index, int rate , int numLeds, int totalOffset, bool direction) {
  if(counter[index] == rate) {
    transitioningLED[index] = true;
    if(currentPos[index] == (direction ? (numLeds-1) : 0)) {
      colorChange[index] = colorChange[index] ? 0 : 1;
      currentPos[index] = (direction ? 0 : numLeds-1);
    }
    else {
      currentPos[index] = currentPos[index] + (direction ? 1 : -1);
    }
    counter[index] = 0;
  }
  else {
    transitioningLED[index] = false;
    counter[index]++;
  }
  
  leds[totalOffset + currentPos[index]] = colorChange[index] ? CRGB::Red : CRGB::White;
}
