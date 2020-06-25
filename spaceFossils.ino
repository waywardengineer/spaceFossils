
#include <Adafruit_NeoPixel.h>


// Which pin on the Arduino is connected to the NeoPixels?

#define LED_PIN     2



// NeoPixel brightness, 0 (min) to 255 (max)
const int maximumConcurrentPulses = 10;
const int numberOfLeds = 15;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(numberOfLeds, LED_PIN, NEO_GRBW + NEO_KHZ800);


uint8_t baseColor[4] = {0, 0, 0, 100};
int pulseLocations[maximumConcurrentPulses];
uint8_t pulseIntensities[maximumConcurrentPulses][4];
int pulseDirections[maximumConcurrentPulses];
int pulseStepDelay[maximumConcurrentPulses];
uint8_t pulseStepsLeft[maximumConcurrentPulses];
uint8_t pulseWidths[maximumConcurrentPulses];
uint8_t rgbwColors[numberOfLeds][4];
long nextPulseCheckTime[maximumConcurrentPulses];

void setup() {
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set overall brightness (max = 255)
  for (int pulse=0; pulse < maximumConcurrentPulses; pulse++){
    pulseStepsLeft[pulse] = 0;
  }
}

void loop() {
  //set the base color for each LED- they all start out with the same color and then the pulses are added
  for (int led=0; led < numberOfLeds; led++){
      for (int color=0; color < 4; color++){
        rgbwColors[led][color] = baseColor[color];      
      }
  }

  //iterate through each pulse and add it's colors to the base color if it's active, or decide whether to reactivate if it's not active
  for (int pulse=0; pulse < maximumConcurrentPulses; pulse++){
    if (pulseStepsLeft[pulse] > 0){ 
      //this pulse is active, add it to the pixels
      if (millis() > nextPulseCheckTime[pulse]){
        //advance the pulse to the next step
        pulseStepsLeft[pulse] -=1;
        nextPulseCheckTime[pulse] = millis() + pulseStepDelay[pulse];
        pulseLocations[pulse] += pulseDirections[pulse];
        pulseLocations[pulse] = (numberOfLeds + pulseLocations[pulse]) % numberOfLeds;
      }
      for (int stepsAwayFromCenter=0; stepsAwayFromCenter < pulseWidths[pulse]; stepsAwayFromCenter++){
        int upLocation = (pulseLocations[pulse] + stepsAwayFromCenter) % numberOfLeds;
        int dnLocation = (numberOfLeds + pulseLocations[pulse] - stepsAwayFromCenter) % numberOfLeds;
        for (int color=0; color < 3; color++){
          int colorAddition = (pulseIntensities[pulse][color] * (pulseWidths[pulse] - stepsAwayFromCenter));
          rgbwColors[upLocation][color] += min(colorAddition, (255 - rgbwColors[upLocation][color]));
          rgbwColors[dnLocation][color] += min(colorAddition, (255 - rgbwColors[dnLocation][color]));
        }
      }
    }
    else {
      // the pulse isn't active, reactivate it
      nextPulseCheckTime[pulse] = millis() + random(20000);
      switch(random(3)){
        //randomly choose a preset set of values for the pulse
        case 0:
          pulseStepsLeft[pulse] = 15;
          pulseLocations[pulse] = 0;
          pulseStepDelay[pulse] = 50;
          pulseWidths[pulse] =  2;
          pulseIntensities[pulse][0] = 10;
          pulseIntensities[pulse][1] = 0;
          pulseIntensities[pulse][2] = 50;
          pulseIntensities[pulse][3] = 0; 
          pulseDirections[pulse] = 1;
          break;
        case 1:
          pulseStepsLeft[pulse] = 15;
          pulseLocations[pulse] = 0;
          pulseStepDelay[pulse] = 250;
          pulseWidths[pulse] =  3;
          pulseIntensities[pulse][0] = 10;
          pulseIntensities[pulse][1] = 50;
          pulseIntensities[pulse][2] = 0;
          pulseIntensities[pulse][3] = 0; 
          pulseDirections[pulse] = -1;
          break;
        case 2:
          pulseStepsLeft[pulse] = 2;
          pulseLocations[pulse] = 0;
          pulseStepDelay[pulse] = 250;
          pulseWidths[pulse] =  15;
          pulseIntensities[pulse][0] = 255;
          pulseIntensities[pulse][1] = 0;
          pulseIntensities[pulse][2] = 0;
          pulseIntensities[pulse][3] = 0; 
          pulseDirections[pulse] = -1;
          break;      }
    }
  }
  for (int led=0; led < numberOfLeds; led++){
    //set the colors of each LED in memory
    strip.setPixelColor(led, strip.Color(rgbwColors[led][0], rgbwColors[led][1], rgbwColors[led][2], rgbwColors[led][3])); 
  }
  //send the color data from the micro to the leds
  strip.show();
}
