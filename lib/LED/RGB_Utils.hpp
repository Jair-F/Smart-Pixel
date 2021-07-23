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

		// Ignore first character - its the #
		red = hexToDec(rgb_hex + 1, 2);
		green = hexToDec(rgb_hex + 3, 2);
		blue = hexToDec(rgb_hex + 5, 2);

		return Color(red, green, blue);
	}
	RGB_Color RGBHexToColor(String rgb_hex) {
		return RGBHexToColor(rgb_hex.c_str());
	}

	/**
	 * Only for a single color to convert to hex
	 * @param color The Decimal color(0-255)
	 * @return The hexadecimal Value of the color only(without leading '#')
	 */
	String singleColorToHex(uint8_t color) {
		String ret = "";
		
		uint8_t division = color / 16;
		uint8_t remainder = color % 16;
		if(division <= 9) {
			ret += static_cast<char>('0' + division);
		} else {
			ret += static_cast<char>('A' + division - 10);
		}
		if(remainder <= 9) {
			ret += static_cast<char>('0' + remainder);
		} else {
			ret += static_cast<char>('A' + remainder - 10);
		}
		
		return ret;
	}

	// Gives from the Adafruit_Neopixel::Color-function/object only the green color
	// "Color Object Decoder" !!!
	uint8_t blue(RGB_Color co) {
		return co;
	}
	uint8_t red(RGB_Color co) {
		return co >> 16;
	}
	uint8_t green(RGB_Color co) {
		return co >> 8;
	}


	String RGBColorToHex(RGB_Color co) {
		String RGBHex = "#"; // Initialize with the RGB-Hashtag at beginning
		uint8_t red, green, blue;

		blue	= RGB_Utils::blue(co);
		green	= RGB_Utils::green(co);
		red		= RGB_Utils::red(co);

		RGBHex += singleColorToHex(red);
		RGBHex += singleColorToHex(green);
		RGBHex += singleColorToHex(blue);

		return RGBHex;
	}

	String RGBColorToHex(uint8_t red, uint8_t green, uint8_t blue) {
		return RGBColorToHex(Color(red, green, blue));
	}
}
