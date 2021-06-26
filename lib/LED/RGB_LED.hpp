#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "./Effects.hpp"

/*
I Have an Adafruit_NeoPixel object and not overloaded, because otherwise I would have had to write the functions twice.
Once for the user - so that the effect no longer runs when the LED color is set, and once for the Effect, that for him it is possible to 
change the LED color without switching itself off. So I can use the functions of Adafruit_NeoPixel for the effect 
and only have to use the other functions once for the user write
*/

class RGB_LED {
private:
	Adafruit_NeoPixel LEDS;
	Effect actualEffekt;

	// the Object, which stores for every Pixel the Color - returns from the effect-function
	RGB_Pixel_Colors PixelColors;

	bool effectIsRunning;
public:
	RGB_LED(uint16_t numPixels, unsigned short pin, neoPixelType type);

	void setActualEffekt(Effect& _effekt);
	// Returns nullptr if there is no effect running
	Effect& getActualEffekt() { return effectIsRunning == true ? actualEffekt : throw LED_error("Tried to get actualEfect - but no effect is running!"); }

	// returns wether there runs a effect or not
	bool effectRunning() const { return effectIsRunning; }

	void operator()(unsigned short effektSpeed);

	// Functions which must to be sepcialized - deactivate Effect if expicit set Color on the led's.
	void setPixelColor(uint16_t pixelPos, RGB_Utils::RGB_Color _color)					{ effectIsRunning = false; LEDS.setPixelColor(pixelPos, _color);			}
	void setPixelColor(uint16_t pixelPos, uint8_t red, uint8_t green, uint8_t blue)		{ effectIsRunning = false; LEDS.setPixelColor(pixelPos, red, green, blue);	}
	void clear()																		{ effectIsRunning = false; LEDS.clear();																}
	void show()																			{ LEDS.show();																}
	void setPin(uint16_t _pin)															{ LEDS.setPin(_pin);														}
	void fill(uint32_t color, uint16_t firstPixelPos=0, uint16_t count=0)				{ effectIsRunning = false; LEDS.fill(color, firstPixelPos, count);			}
	void setBrightness(uint8_t brightness)												{ LEDS.setBrightness(brightness);											}

	uint8_t getBrightness()						const									{ return LEDS.getBrightness();			}
	int16_t getPin()							const									{ return LEDS.getPin();					}
	uint16_t numPixels()						const									{ return LEDS.numPixels();				}
	uint32_t getPixelColor(uint16_t pixelPos)	const									{ return LEDS.getPixelColor(pixelPos);	}
};






// Implementationen

void RGB_LED::setActualEffekt(Effect& _effekt) {
	actualEffekt = _effekt;
	effectIsRunning = true;
}

RGB_LED::RGB_LED(unsigned short numPixels, unsigned short pin, neoPixelType type): LEDS(numPixels, pin, type), PixelColors(LEDS.numPixels()) {
	LEDS.begin();
}

void RGB_LED::operator()(unsigned short effektSpeed) {
	if(effectIsRunning) {
		actualEffekt(PixelColors);
		for(uint16_t i = 0; i < LEDS.numPixels(); i++) {
			LEDS.setPixelColor(i, PixelColors[i]);
		}
		LEDS.show();
		delay(effektSpeed);
	}
}

