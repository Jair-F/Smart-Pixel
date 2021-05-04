#ifndef _DISPLAY_HPP_INCLUDED_
#define _DISPLAY_HPP_INCLUDED_

// https://randomnerdtutorials.com/guide-to-1-8-tft-display-with-arduino/
// https://www.youtube.com/watch?v=NAyt5kQcn-A - gute erklaerung der pins
// richtig -- https://www.youtube.com/watch?v=9qndU3JJiKk

/*
The Pin Connection:
LCD               ESP8266
VCC               3.3V
GND               GND
CS                   D8
Reset              3.3v
A0 or D/C       D4
SDA (MOSI)   D7 
SCK                 D5 
LED                 3.3V
*/

/*
#include <Adafruit_GFX.h>		// Include core graphics library
#include <Adafruit_ST7735.h>	// Include Adafruit_ST7735 library to drive the display


#define TFT_CS		15	// D8
#define TFT_RST		0	// goes to ground
#define TFT_DC		2	// DC = A0 - D4

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
*/

/*
This is an example on how to use the 1.8" TFT 128x160 SPI ST7735 display using the Adafruit library.

ST7735 TFT SPI display pins for Arduino Uno/Nano:
 * LED =   3.3V
 * SCK =   13
 * SDA =   11
 * A0 =    8
 * RESET = 9
 * CS =    10
 * GND =   GND
 * VCC =   5V

Another version marked as KMR-1.8 SPI:
This version only supports 3.3V logic so put a level shifter for all I/O pins, or a 2.2k resistor between
the display and arduino, and a 3.3k resistor to ground to create a simple voltage divider to produce a 3V output.
 * LED- =  GND
 * LED+ =  15Ω resistor to 5V
 * CS =    10
 * SCL =   13
 * SDA =   11
 * A0  =   8
 * RESET = 9
 * VCC =   5V or 3.3V (the display has it's own 3.3V regulator)
 * GND =   GND

Hardware SPI Pins:
 * Arduino Uno   SCK=13, SDA=11
 * Arduino Nano  SCK=13, SDA=11
 * Arduino Due   SCK=76, SDA=75
 * Arduino Mega  SCK=52, SDA=51

SPI pin names can be confusing. These are the alternative names for the SPI pins:
MOSI = DIN = R/W = SDO = DI = SI = MTSR = SDA = D1 = SDI
CS = CE = RS = SS
DC = A0 = DO = DOUT = SO = MRST
RESET = RST
SCLK = CLK = E = SCK = SCL = D0


Libraries needed:
https://github.com/adafruit/Adafruit-ST7735-Library
https://github.com/adafruit/Adafruit-GFX-Library


Reference page for GFX Library: https://cdn-learn.adafruit.com/downloads/pdf/adafruit-gfx-graphics-library.pdf


Color is expressed in 16 bit with Hexadecimal value.
To select a particular color, go here and copy the "Hexadecimal 16 bit color depth value":
https://ee-programming-notepad.blogspot.com/2016/10/16-bit-color-generator-picker.html

Common colors:
 * BLACK    0x0000
 * BLUE     0x001F
 * RED      0xF800
 * GREEN    0x07E0
 * CYAN     0x07FF
 * MAGENTA  0xF81F
 * YELLOW   0xFFE0
 * WHITE    0xFFFF

A way to select a color is to write: "ST7735_BLACK", or "ST7735_BLUE", etc.
Or just write the code for the color. Either way, it works.


List of custom fonts: https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts

Note about custom font:
 * Text background color is not supported for custom fonts. For these reason you would need to draw a filled 
	 rectangle before drawing the text. But this would cause the text to flicker, so I don't recommend using custom fonts
	 for components that refresh continuously.
 * Using custom fonts slows down the arduino loop, so the refresh rate is lesser than using the standard font.


Sketch made by: InterlinkKnight
Last modification: 01/11/2018
*/



void progressBar(unsigned short abstand_zu_seiten, unsigned short height, unsigned short length, unsigned short y_Pos) {
	
}

#endif // _DISPLAY_HPP_INCLUDED_