/*
An Open Source Arduino based DJI Led project.
Program : DJI Led (Firmware)
Version : V0.1, April 10th 2016
Author(s): Juan Pedro Gonzalez

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:
The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
For more detail on the MIT license, see <http://opensource.org/licenses/MIT>
*/
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing to set this to 1
#define PIN 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      8

// LED brightness (0-255)
#define BRIGHTNESS 30

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
//
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); // 32 is the number of LEDs in my case i'm using 4 LED strips with 8 pixels each.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.

  // GPIO pins 2 and 4 for analogRead from Naza
  pinMode(2, INPUT);  // Naza Green LED
  pinMode(4, INPUT);  // Naza Red LED

  // Set the brightness
  pixels.setBrightness(BRIGHTNESS);
}

/**
 * Read the color code from the Naza controller.
 */
unsigned long int getNazaStatusColor() {
  static const unsigned long int nazaStatusColors[] = { 0x00FF00, 0xFF0000, 0xFFFF00 }; // Green, Red, Yellow
  static unsigned long int nazaStatusColor = 0x000000; // 0x000000 = Off
  
  // Read Naza LED color
  // -------------------
  // Read Naza Green LED
  int sensorValue0 = analogRead(1); // Digital pin 2 is Analog 1 on Trinket
  // Read Naza Red LED
  int sensorValue1 = analogRead(2); // Digital pin 4 is Analog 2 on Trinket

  // e.g., 1 + 2 = 3 (yellow), 0 + 2 = 2 (red)
  byte statusColorIndex = (sensorValue0 > 500) + ((sensorValue1 > 500) << 1);

  if (statusColorIndex != 0) {
    nazaStatusColor = nazaStatusColors[(statusColorIndex - 1)];
  }
  
  return nazaStatusColor;
}

/**
 * Front LEDs on a Phantom are always the navigation lights.
 */
void displayFrontNavigationLights()
{
  unsigned long int frontNavigationColor = 0xFF0000;
  
  for (byte i=0; i < 8; i++) {
      pixels.setPixelColor(i+16, frontNavigationColor); // Front left  (M2)
      pixels.setPixelColor(i+24, frontNavigationColor); // Front right (M1)
  }
}

/**
 * Back LEDs are used to show the NAZA status.
 */
void displayBackNavigationLigths() {
  unsigned long int backNavigationColor = getNazaStatusColor();
  
  for (byte i=0; i < 8; i++) {
      pixels.setPixelColor(i, backNavigationColor);   // Back right  (M4)
      pixels.setPixelColor(i+8, backNavigationColor); // Back left (M3)
  }
}

/**
 * Main loop.
 */
void loop() {
  displayFrontNavigationLights();
  displayBackNavigationLigths();
  
  // Sends the updated pixel color to the hardware.
  pixels.show();                              
}
