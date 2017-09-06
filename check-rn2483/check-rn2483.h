// **********************************************************************************
// WeMos RN2483 Transparent TCP to Serial for RN2483 WeMos shield
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// For any explanation of ULPNode see 
// https://hallard.me/category/ulpnode/
//
// Written by Charles-Henri Hallard http://ch2i.eu
//
// History : V1.20 2016-06-11 - Creation
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef _CHECK_RN2483_H
#define _CHECK_RN2483_H

#include <arduino.h>
#include <SoftwareSerial.h>
#include <NeoPixelBus.h>

#include "pushbutton.h"
#include "RGBLed.h"

// Define here the target serial you connected to this app
// -------------------------------------------------------
// Uncomment this line if you want to use soft serial
//#define USE_SOFT_SERIAL

// Uncomment 3 lines below if you have an WS1812B RGB LED 
// like shield here https://github.com/hallard/WeMos-RN2483
#ifdef USE_SOFT_SERIAL
  #define SERIAL_DEVICE RN2483Serial
  #define SERIAL_DEBUG  Serial
#else
  #define SERIAL_DEVICE Serial
  //#define SERIAL_DEBUG  DebugSerial
  #define SERIAL_DEBUG  Serial1

#endif

// Board V1.1+ (for V1.0 it's pin 15)
#define RN2483_RESET_PIN  12

// Exported vars
#ifdef USE_SOFT_SERIAL
extern SoftwareSerial RN2483Serial;
#else
extern SoftwareSerial DebugSerial;
#endif

extern String inputString;
extern unsigned long seconds;

#endif



