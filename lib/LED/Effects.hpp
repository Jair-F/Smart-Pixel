#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <vector>

#include "./RGB_Utils.hpp"
#include "../Exception.hpp"


class RGB_Pixel_Colors {
private:
	std::vector<RGB_Utils::RGB_Color> colors;
public:	
	RGB_Pixel_Colors(unsigned short numOfPixels) { colors.resize(numOfPixels);	}
	~RGB_Pixel_Colors() { }
	RGB_Utils::RGB_Color& operator[](unsigned short pixelPos) { return colors[pixelPos]; }
	RGB_Utils::RGB_Color& at(unsigned short pixelPos) { return colors.at(pixelPos); }
	unsigned short numOfPixels() { return colors.size(); }
};




// To write the Effect-Function-Pointer shorter and better to read
typedef RGB_Pixel_Colors& (*EffectFuncPointer)(RGB_Pixel_Colors& _pixelColors);

class Effect {
private:
	EffectFuncPointer effectPointer;
	String EffectName;
public:
	Effect() { }
	Effect(String _EffectName, EffectFuncPointer _effectPointer);

	void setFuncPointer(EffectFuncPointer _effectPointer);

	RGB_Pixel_Colors& operator()(RGB_Pixel_Colors& PixelColors);

	String getName() { return EffectName; }
	void setName(String _EffectName) { EffectName = _EffectName; }
};





class EffectGroup {
private:
	std::vector<Effect> effects;
public:
	EffectGroup() { }
	Effect& operator[](String EffectName);
	Effect& operator[](std::size_t n) { return effects[n]; }
	Effect& find(String EffectName);

	bool exist(String EffectName);

	// Adds the effect in every Case(if there already exist an Object with the same EffectName it overwrites it!!!)
	void add(Effect ef);
	
	// Removes the Effect with the Name "EffectName" if it exist. If it does not exist nothing happens!
	void remove(String EffectName);
};





// Implementationen


// Effekt
RGB_Pixel_Colors& Effect::operator()(RGB_Pixel_Colors& PixelColors) {
	// The Object(PixelColors), which stores for every Pixel of the LEDs the Color
	// pass that "color-object" as reference. The Effect changes(call by reference) it and returns it also as reference
	return effectPointer(PixelColors);
}

void Effect::setFuncPointer(EffectFuncPointer _effectPointer) {
	if(_effectPointer != nullptr) {
		this->effectPointer = _effectPointer;
	} else {
		throw LED_error("In Effect-Class the effect-function-pointer tried to set to nullptr!");
	}
}


Effect::Effect(String _EffectName, EffectFuncPointer _effectPointer): EffectName(_EffectName) {
	this->setFuncPointer(_effectPointer);
}
// Effekt



// ------------------------------------------------------------------------------------------------------------------------------



// EffectGroup
void EffectGroup::remove(String EffectName) {
	auto iterator = effects.begin();
	while(iterator != effects.end()) {
		if(iterator->getName() == EffectName) {
			effects.erase(iterator);
			// We ensure while we insert the elements, that there arent duplicates - therfore we can break
			break;
		}
	}
}

void EffectGroup::add(Effect ef) {
	if(! this->exist(ef.getName())) {
		effects.emplace_back(ef);
	} else {
		this->find(ef.getName()) = ef;
	}
}

bool EffectGroup::exist(String EffectName) {
	for(std::size_t i = 0; i < effects.size(); ++i) {
		if(effects[i].getName() == EffectName) {
			return true;
		}
	}
	return false;
}

Effect& EffectGroup::find(String EffectName) {
	for(std::size_t i = 0; i < effects.size(); ++i) {
		if(effects[i].getName() == EffectName) {
			return effects[i];
		}
	}

	throw LED_error("Effect: " + EffectName + " was not found in the Effect-List!");
}

Effect& EffectGroup::operator[](String EffectName) {
	return this->find(EffectName);
}
// EffectGroup