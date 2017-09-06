// **********************************************************************************
// RGB Led header file for WeMos Lora RN2483 Shield
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// Written by Charles-Henri Hallard http://ch2i.eu
//
// History : V1.20 2016-06-11 - Creation
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

// Master Project File
#include "check-rn2483.h"

#ifndef RGBLed_H
#define RGBLed_H

// value for HSL color
// see http://www.workwithcolor.com/blue-color-hue-range-01.htm
#define COLOR_RED              0
#define COLOR_ORANGE          30
#define COLOR_ORANGE_YELLOW   45
#define COLOR_YELLOW          60
#define COLOR_YELLOW_GREEN    90
#define COLOR_GREEN          120
#define COLOR_GREEN_CYAN     165
#define COLOR_CYAN           180
#define COLOR_CYAN_BLUE      210
#define COLOR_BLUE           240
#define COLOR_BLUE_MAGENTA   275
#define COLOR_MAGENTA        300
#define COLOR_PINK           350

// RGB Led on GPIO0
#define RGB_LED_PIN 	0
#define RGB_LED_COUNT 1
#define RGBW_LED 	/* I'm using a RGBW WS2812 led */

// Master RDB LED
#define RGB_LED1      1

#ifdef RGBW_LED
  typedef NeoPixelBus<NeoGrbwFeature, NeoEsp8266BitBang800KbpsMethod> MyPixelBus;
#else
  typedef NeoPixelBus<NeoRgbFeature, NeoEsp8266BitBang800KbpsMethod> MyPixelBus;
#endif 

void LedRGBOFF(uint16_t led=0);
void LedRGBON (uint16_t hue, uint16_t led=0, bool doitnow=true);

extern uint16_t wifi_led_color ; 
extern uint8_t rgb_luminosity; // Luminosity from 0 to 100% 
extern uint16_t anim_speed;
extern MyPixelBus rgb_led;


#endif
