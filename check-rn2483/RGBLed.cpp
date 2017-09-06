// **********************************************************************************
// RGB Led source file for WeMos Lora RN2483 Shield
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
#include "RGBLed.h"

MyPixelBus rgb_led(RGB_LED_COUNT, RGB_LED_PIN);
uint8_t rgb_luminosity = 20 ; // Luminosity from 0 to 100% 
uint16_t wifi_led_color ; // Wifi Led Color dependinf on connexion type

/* ======================================================================
Function: LedRGBON
Purpose : Set RGB LED strip color, but does not lit it
Input   : Hue of LED (0..360)
          led number (from 1 to ...), if 0 then all leds
          if led should be lit immediatly
Output  : - 
Comments: 
====================================================================== */
void LedRGBON (uint16_t hue, uint16_t led, bool doitnow)
{
  uint8_t start = 0;
  uint8_t end   = RGB_LED_COUNT-1; // Start at 0

  // Convert to neoPixel API values
  // H (is color from 0..360) should be between 0.0 and 1.0
  // S is saturation keep it to 1
  // L is brightness should be between 0.0 and 0.5
  // rgb_luminosity is between 0 and 100 (percent)
  RgbColor target = HslColor( hue / 360.0f, 1.0f, 0.005f * rgb_luminosity);

  // just one LED ?
  // Strip start 0 not 1
  if (led) {
    led--;
    start = led ;
    end   = start ;
  } 

  for (uint8_t i=start ; i<=end; i++) {
    // do it now ?
    if (doitnow) {
      rgb_led.SetPixelColor(i, target);
      rgb_led.Show();  
    }
  }
}

/* ======================================================================
Function: LedRGBOFF 
Purpose : light off the RGB LED strip
Input   : Led number starting at 1, if 0=>all leds
Output  : - 
Comments: -
====================================================================== */
void LedRGBOFF(uint16_t led)
{
  uint8_t start = 0;
  uint8_t end   = RGB_LED_COUNT-1; // Start at 0

  // just one LED ?
  if (led) {
    led--;
    start = led ;
    end   = start ;
  }

  // stop animation, reset params
  for (uint8_t i=start ; i<=end; i++) {
    // clear the led strip
    rgb_led.SetPixelColor(i, RgbColor(0));
    rgb_led.Show();  
  }
}

