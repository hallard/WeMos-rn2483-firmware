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

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <Hash.h>
#include <SoftwareSerial.h>

// Main project include file
#include "check-rn2483.h"

// If you want to use soft serial, not super reliable with AsyncTCP
// RN2483 TX is default connected to ESP8266 GPIO13 (or RX depends on solder pad)
// RN2483 RX is default connected to ESP8266 GPIO15 (or TX depends on solder pad)
#ifdef USE_SOFT_SERIAL
SoftwareSerial RN2483Serial(13, 15, false, 128);
#else
//SoftwareSerial DebugSerial(SW_SERIAL_UNUSED_PIN ,14, false, 128);
#define SW_SERIAL_TX_PIN  1
SoftwareSerial DebugSerial(SW_SERIAL_UNUSED_PIN , SW_SERIAL_TX_PIN , false, 128);
#endif

//const char* ssid = "CH2I-HOTSPOT";
//const char* password = "Wireless@Ch2i";
const char* ssid = "*******";
const char* password = "*******";

char thishost[17];
unsigned long seconds = 0;

String inputString = "";
bool serialSwapped = false;


/* ======================================================================
Function: sendCommand
Purpose : send command to RN2483 device
Input   : -
Output  : -
Comments: -
====================================================================== */
void sendCommand(const char * cmd) {

  LedRGBON(COLOR_MAGENTA, 0, true);

  // Display on debug
  SERIAL_DEBUG.printf("  -> \"%s\"\r\n", cmd);

  // Send text to serial
  SERIAL_DEVICE.printf("%s\r\n", cmd);
  SERIAL_DEVICE.flush();
}

/* ======================================================================
Function: sendCommand
Purpose : read answer from RN2483 device
Input   : -
Output  : true is device answered before timeout 
Comments: -
====================================================================== */
bool readAnswer(void) {

  bool hasString = false;
  unsigned long start = millis();

  do {
    // Got one serial char ?
    if (SERIAL_DEVICE.available()) {
      // Read it and store it in buffer
      char inChar = (char)SERIAL_DEVICE.read();

      // CR line char, discard ?
      if (inChar == '\r') {
        // Do nothing

      // LF ok let's do our job
      } else if (inChar == '\n') {
        hasString = true;

        // Display on debug
        SERIAL_DEBUG.printf("  <- \"%s\" \r\n", inputString.c_str());
      } else {
        // Add char to input string
        inputString += inChar;
      }
    }

    yield();
    ArduinoOTA.handle();

  // While not async and not got all response and not timed out
  } while( !hasString && (millis()-start < 1500) );

  return (hasString);
}

/* ======================================================================
Function: doTest
Purpose : Setup I/O and other one time startup stuff
Input   : command to test 
Output  : - 
Comments: -
====================================================================== */
void doTest(const char * cmd) 
{
  sendCommand(cmd);

  if ( readAnswer() ) {
    LedRGBON(COLOR_GREEN, 0, true);
  } else {
    LedRGBON(COLOR_RED, 0, true);
  }

  // Clear response
  inputString = "";

  delay(200);
  LedRGBOFF();
}

/* ======================================================================
Function: setup
Purpose : Setup I/O and other one time startup stuff
Input   : -
Output  : - 
Comments: -
====================================================================== */
void setup() {

  // Close proper
  SERIAL_DEVICE.flush(); 
  SERIAL_DEVICE.begin(57600);

  #ifdef USE_SOFT_SERIAL
  
  #else
  // Pin have been swapped to avoid interaction
  // when programming chip over USB serial so 
  // Real Serial is Mapped to GPIO13/GPIO15
  SERIAL_DEVICE.swap();
  serialSwapped = true;

  // But we'll keep Original pin TX as software Serial
  // for debug. Bt Serial.begin and swap broke 
  // Software Serial configuration pins. As we can not
  // instantiante again we just reconfigure the pin 
    #ifdef SW_SERIAL_TX_PIN
    pinMode(SW_SERIAL_TX_PIN, OUTPUT);
    digitalWrite(SW_SERIAL_TX_PIN, HIGH);
    #endif 
  #endif

  // enable auto baud rate (see RN2483 datasheet)
  SERIAL_DEVICE.write((byte) 0x00);
  SERIAL_DEVICE.flush(); 
  SERIAL_DEVICE.write(0x55);
  SERIAL_DEVICE.flush(); 
  SERIAL_DEVICE.write(0x0A);
  SERIAL_DEVICE.write(0x0D);
 

  SERIAL_DEBUG.begin(115200);
  SERIAL_DEBUG.print(F("\r\nBooting on "));
  SERIAL_DEBUG.println(ARDUINO_BOARD);

  // Set Hostname for OTA and network (add only 2 last bytes of last MAC Address)
  // You can't have _ or . in hostname 
  sprintf_P(thishost, PSTR("Check2483-%04X"), ESP.getChipId() & 0xFFFF);
  SERIAL_DEBUG.print(F("I'm network device named "));
  SERIAL_DEBUG.println(thishost);

  WiFi.hostname(thishost);
  WiFi.mode(WIFI_STA);

  // No empty sketch SSID, try connect 
  if (*ssid!='*' && *password!='*' ) {
    SERIAL_DEBUG.printf("connecting to %s with psk %s\r\n", ssid, password );
    WiFi.begin(ssid, password);
  } else {
    // empty sketch SSID, try autoconnect with SDK saved credentials
    SERIAL_DEBUG.println(F("No SSID/PSK defined in sketch\r\nConnecting with SDK ones if any"));
  }

  // Loop until connected or 20s time out
  while ( WiFi.status() != WL_CONNECTED && millis() < 20000 ) {
    if ((millis() % 333) < 111) {
      LedRGBON(COLOR_ORANGE_YELLOW);
    } else {
      LedRGBOFF();
    }
    delay(1); 
  }

  //new_led_state = ((millis() % 333) < 111) ? LOW:HIGH;// AP Mode or client failed quick blink 111ms on each 1/3sec
  //new_led_state = ((millis() % 1000) < 200) ? LOW:HIGH; // Connected long blink 200ms on each second

  ArduinoOTA.setHostname(thishost);
  ArduinoOTA.begin();

  // OTA callbacks
  ArduinoOTA.onStart([]() {
    SERIAL_DEBUG.println(F("\r\nOTA Starting")); 
    LedRGBOFF();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    uint8_t percent = progress / (total / 100);
    // hue from 0.0 to 1.0 (rainbow) with 33% (of 0.5f) luminosity
    rgb_led.SetPixelColor(0, HslColor(percent * 0.01f , 1.0f, 0.17f ));
    rgb_led.Show();  
  });

  ArduinoOTA.onEnd([]() { 
    rgb_led.SetPixelColor(0, HslColor(COLOR_GREEN/360.0f, 1.0f, 0.25f));
    rgb_led.Show();  
    SERIAL_DEBUG.println(F("Done Rebooting")); 
  });

  ArduinoOTA.onError([](ota_error_t error) {
    rgb_led.SetPixelColor(0, HslColor(COLOR_RED/360.0f, 1.0f, 0.25f));
    rgb_led.Show();  
    SERIAL_DEBUG.println(F("Error")); 
    ESP.restart(); 
  });

  #if defined (BTN_GPIO) 
    pinMode(BTN_GPIO, INPUT);
  #endif

  // Start Server
  WiFiMode_t con_type = WiFi.getMode();
  uint16_t lcolor = 0;
  SERIAL_DEBUG.print(F("Started "));

  if (con_type == WIFI_STA) {
    SERIAL_DEBUG.print(F("WIFI_STA"));
    lcolor=COLOR_GREEN;
  } else if (con_type==WIFI_AP_STA || con_type==WIFI_AP) {
    SERIAL_DEBUG.print(F("WIFI_AP_STA"));
    lcolor=COLOR_CYAN;
  } else {
    SERIAL_DEBUG.print(F("????"));
    lcolor = COLOR_RED;
  }

  // Set Breathing color during startup script
  LedRGBON(lcolor, 0, true);

  // Reset rn2483 module
  pinMode(RN2483_RESET_PIN,OUTPUT);
  digitalWrite(RN2483_RESET_PIN, LOW);
  delay(10);
  digitalWrite(RN2483_RESET_PIN, HIGH);

  doTest("sys get ver");
  doTest("sys set pinmode GPIO1 digout");
  doTest("sys set pinmode GPIO10 digout");
}

/* ======================================================================
Function: loop
Purpose : infinite loop main code
Input   : -
Output  : - 
Comments: -
====================================================================== */
void loop() {
  static unsigned long previousMillis = 0;// last time update
  static uint8_t _state = 0;
  unsigned long currentMillis = millis();
  uint8_t button_port;
  btn_state_e btn_state;

  // Manage our second counter
  if ( millis()-previousMillis > 1000) {
    char buff[32];

     previousMillis = currentMillis;
     seconds++;

    // Every x seconds
    if (seconds % 2 == 0) {
      if (_state==0) {
        doTest("sys get ver");
      } else if (_state==1) {
        doTest("sys set pindig GPIO1 1");
      } else if (_state==2) {
        doTest("sys set pindig GPIO1 0");
      } else if (_state==3) {
        doTest("sys set pindig GPIO10 1");
      } else if (_state==4) {
        doTest("sys set pindig GPIO10 0");
      }

      if (++_state>=5) {
        _state=0;
      }

    }

  }

  #if defined (BTN_GPIO) 
  // Get switch port state 
  button_port = digitalRead(BTN_GPIO);

  // Button pressed 
  if (button_port==BTN_PRESSED){
    // we enter into the loop to manage
    // the function that will be done
    // depending on button press duration
    do {
      // keep watching the push button:
      btn_state = buttonManageState(button_port);

      // read new state button
      button_port = digitalRead(BTN_GPIO);

      // this loop can be as long as button is pressed
      yield();
      ArduinoOTA.handle();
    }
    // we loop until button state machine finished
    while (btn_state != BTN_WAIT_PUSH);


    SERIAL_DEBUG.printf("Button %d\r\n", _btn_Action);

  // button not pressed
  }

  #endif // If BTN_GPIO

  // Handle remote Wifi Updates
  ArduinoOTA.handle();

}
