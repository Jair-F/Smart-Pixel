#ifndef _HELPER_HPP_INCLUDED_
#define _HELPER_HPP_INCLUDED_

#include "Exception.hpp"

// Nur fuer ganze und positive Zahlen
String to_string(unsigned long num) {
	String ret;
	while(num > 0) {
		ret += num % 10;
		num /= 10;
	}
	std::reverse(ret.begin(), ret.end());
	return ret;
}

// Funktionen um den RGB-Hex-Code zu konvertieren und in dezimal Werte von 0-255 umwandeln
std::size_t power(unsigned short num, unsigned short times) {
	if(times >= 1) {
		return num * power(num, times - 1);
	}
	return 1;
}

unsigned short singleHexToDec(const char hex) {
	unsigned short ret;
	if(hex >= '0' && hex <= '9') {
		ret = hex - '0';
	} else if(hex >= 'a' && hex <= 'f') {
		ret = hex - 'a' + 10;
	} else if(hex >= 'A' && hex <= 'Z') {
		ret = hex - 'A' + 10;
	}
	return ret;
}

// https://owlcation.com/stem/Convert-Hex-to-Decimal
unsigned short hexToDec(const char* hex, std::size_t size_of_num){
	unsigned short ret = 0;
	for(unsigned short i = 0; i < size_of_num; i++) {
		// Elemente von vorne nach hinten durchlaufen(num)
		ret += singleHexToDec(hex[size_of_num - 1 - i]) * power(16, i);	// -1, weil 0 das erste element ist
	}
	return ret;
}


// https://www.developintelligence.com/blog/2017/02/rgb-to-hex-understanding-the-major-web-color-codes/
// Adafruit_NeoPixel::Color()
static uint32_t RGBHexToColor(const char* rgb_hex) {
	byte red;
	byte green;
	byte blue;

	// Erstes Zeichein ignorieren - ist das #
	red = hexToDec(rgb_hex + 1, 2);
	green = hexToDec(rgb_hex + 3, 2);
	blue = hexToDec(rgb_hex + 5, 2);

	return Adafruit_NeoPixel::Color(red, green, blue);
}


#endif // _HELPER_HPP_INCLUDED_