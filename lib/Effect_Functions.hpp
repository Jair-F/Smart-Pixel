#pragma once

#include "LED/Effects.hpp"
#include "LED/RGB_Utils.hpp"

/*
	Beispiel fuer eine Effect-Funktion(header)
	RGB_Pixel_Colors& nameOfEffectFunction(RGB_Pixel_Colors& _pixelColors)

	RGB_Pixel_Colors speichert fuer jede LED den Farbwert ab. Die Anzahl an leds kann in diesem Object ausgelesen werden - 
	sie kommt schon voreingestellt und man richtet sich nach diesem Wert. Dann fuegt man fuer jede LED den Farbwert ein (fuer LED[0] an
	_pixelColors[0]...) und gibt dieses Objekt, was man als Referenz bekommen hat wieder als Referenz zurueck.
*/



RGB_Pixel_Colors& rainbow_soft_blink(RGB_Pixel_Colors& _pixelColors) {
	static byte schleife{1};    // Die auszufuehrende Schleife(wird in switch-case abgefragt)
	static byte prozent_helligkeit{0};  // Speicher ab, welche Helligkeit die gerade zum hochzaehldende LED hat.
	switch(schleife){
		case 1:{
			for(byte i{0}; i < _pixelColors.numOfPixels(); i++){
				//                                          rot        gruen         blau
				_pixelColors[i] = RGB_Utils::Color(prozent_helligkeit,  0   , 255 - prozent_helligkeit);
			}
			break;
		}

		case 2:{
			for(byte i{0}; i < _pixelColors.numOfPixels(); i++){
				_pixelColors[i] = RGB_Utils::Color(255 - prozent_helligkeit, prozent_helligkeit, 0);
			}
			break;
		}

		case 3:{
			for(byte i{0}; i < _pixelColors.numOfPixels(); i++){
				_pixelColors[i] = RGB_Utils::Color(0, 255 - prozent_helligkeit, prozent_helligkeit);
			}
			break;
		}
	}
	if(prozent_helligkeit >= 255){
		if(schleife < 3){
			++schleife;
		}else{
			schleife = 1;
		}
		prozent_helligkeit = 0;
	}
	prozent_helligkeit++;
	return _pixelColors;
}

RGB_Pixel_Colors& colorWipe(RGB_Pixel_Colors& _pixelColors) {
	static byte pos=0;
	static byte round=1;
	switch(round){
		case 1:{
			//RGB_LEDS.setPixelColor(pos, RGB_LEDS.Color(255, 0, 0));
			_pixelColors[pos] = RGB_Utils::Color(255, 0, 0);
			break;
		}
		case 2:{
			//RGB_LEDS.setPixelColor(pos, RGB_LEDS.Color(0, 255, 0));
			_pixelColors[pos] = RGB_Utils::Color(0, 255, 0);
			break;
		}
		case 3:{
			//RGB_LEDS.setPixelColor(pos, RGB_LEDS.Color(0, 0, 255));
			_pixelColors[pos] = RGB_Utils::Color(0, 0, 255);
			break;
		}
	}
	//RGB_LEDS.show();
	++pos;
	if(pos >= _pixelColors.numOfPixels()){
		round++;
		if(round > 3){
		  round = 1;
		}
		pos = 0;
	}
	//delay((wait+2)*4); // Pause erhoehen, da es sonst viel zu schnell waere
	return _pixelColors;
}





uint32_t Wheel(byte WheelPos) {
	WheelPos = 255 - WheelPos;
	if(WheelPos < 85) {
		return RGB_Utils::Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	if(WheelPos < 170) {
		WheelPos -= 85;
		return RGB_Utils::Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	WheelPos -= 170;
	return RGB_Utils::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
RGB_Pixel_Colors& rainbowCycle(RGB_Pixel_Colors& _pixelColors) {
	static uint16_t j=0;
	for(uint16_t i=0; i< _pixelColors.numOfPixels(); i++) {
		//RGB_LEDS.setPixelColor(i, Wheel( ((i * 256 / RGB_LEDS.numPixels()) + j) & 255));
		_pixelColors[i] = Wheel( ((i * 256 / _pixelColors.numOfPixels()) + j) & 255);
	}
	if(j++>255){
		j=0;
	}
	//RGB_LEDS.show();
	return _pixelColors;
}