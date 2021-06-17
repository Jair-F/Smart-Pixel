#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// for Function "hexToDec"
#include "../Helper.hpp"

namespace RGB_Utils {
	static uint32_t Color(uint8_t red, uint8_t green, uint8_t blue) {
		return Adafruit_NeoPixel::Color(red, green, blue);
	}
	typedef decltype(RGB_Utils::Color(0, 0, 0)) RGB_Color;


	// https://www.developintelligence.com/blog/2017/02/rgb-to-hex-understanding-the-major-web-color-codes/
	RGB_Color RGBHexToColor(const char* rgb_hex) {
		byte red;
		byte green;
		byte blue;

		// Erstes Zeichein ignorieren - ist das #
		red = hexToDec(rgb_hex + 1, 2);
		green = hexToDec(rgb_hex + 3, 2);
		blue = hexToDec(rgb_hex + 5, 2);

		return Color(red, green, blue);
	}

	String RGBColorToHex(Color co) {
		#error "Needs to be implemented RGBColorToHex!!"
	}

	String RGBColorToHex(uint8_t red, uint8_t green, uint8_t blue) {
		#error "Needs to be implemented RGBColorToHex!!"
	}

	// Gives from the Adafruit_Neopixel::Color-function/object only the green color
	// "Color Object Decoder" !!!
	uint8_t blue(RGB_Color co) {
		return co;
	}
	
	uint8_t red(RGB_Color co) {
		return static_cast<uint8_t>(co >> 16);
	}
	
	uint8_t green(RGB_Color co) {
		return static_cast<uint8_t>(co >> 8);
	}
}