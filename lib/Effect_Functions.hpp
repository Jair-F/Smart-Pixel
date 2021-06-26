#pragma once

#include "LED/Effects.hpp"

/*
	Beispiel fuer eine Effect-Funktion(header)
	RGB_Pixel_Colors& nameOfEffectFunction(RGB_Pixel_Colors& _pixelColors)

	RGB_Pixel_Colors speichert fuer jede LED den Farbwert ab. Die Anzahl an leds kann in diesem Object ausgelesen werden - 
	sie kommt schon voreingestellt und man richtet sich nach diesem Wert. Dann fuegt man fuer jede LED den Farbwert ein (fuer LED[0] an
	_pixelColors[0]...) und gibt dieses Objekt, was man als Referenz bekommen hat wieder als Referenz zurueck.
*/



RGB_Pixel_Colors& blink(RGB_Pixel_Colors& _pixelColors) {
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
