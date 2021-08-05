#ifndef _TOUCHSENSOR_HPP_INCLUDED_
#define _TOUCHSENSOR_HPP_INCLUDED_

#include <Arduino.h>
#include "Touch.hpp"

class TouchSensor {
public:
	TouchSensor(unsigned short _pin): pin(_pin) { pinMode(pin, INPUT); }
	~TouchSensor() { }
	TouchSensor(TouchSensor&) = default;

	/**
	 * @return true = if pressed	false = if not pressed
	 */
	bool getStatus() { return digitalRead(pin); }

	unsigned short getPin() { return pin; }
	void setPin(unsigned short _pin) { *this = TouchSensor(_pin); }

	// this functions nedds to be called in the loop
	void loop();

	/**
	 * @param time The time the sensor should be touched
	 * @param time_type The type of the time(milliseconds, seconds)
	 * @param async true if the check should run in the background(the function must to be called in a loop). false if the function should wait(not return) until he has a result
	 * @return * In async mode: It return false as long as the sensor was not touched the given time. If it was touched the given time or longer it will return true.
	 * 		   * Not in async mode: The function will not return until he has a result. Result: true if sensor was touched the given time - otherwise false.
	 */
	bool touched(unsigned short time, TimeType time_type, bool async = true);

	//Touch& get_lastTouch() { return lastTouch; }

	enum Status {
		TOUCHED = true, RELEASED = false
	};

protected:
	Touch lastTouch; // Milliseconds
	Touch thisTouch; // Milliseconds
	bool lastStatus;

	/**
	 * This is to ensure, we use every touch only one time(if the user checks the touches in a loop it could be that he executes somthing multiple times because we give him
	 * true back because we check the Touch we already checked and gave already true back!)
	 */
	bool lastTouchUsed;	// True if we used last touch

private:
	unsigned short pin;
};

void TouchSensor::loop() {
	bool status = this->getStatus();

	if(status == RELEASED   &&   lastStatus == TOUCHED) {
		lastTouch = thisTouch;
		lastTouchUsed = false;

		#ifdef DEBUG
			Serial.print("Touch-Begin-Time: ");
			Serial.println(lastTouch.get_touchBegin());
			Serial.print("Touch-End-Time:");
			Serial.println(lastTouch.get_touchEnd());
			Serial.print("Touch-Time:");
			Serial.print(lastTouch.get_touchDuration());
			Serial.println("ms");
			Serial.println("End-Touch!");
		#endif // DEBUG
	}
	#ifdef DEBUG
		else if(status == TOUCHED   &&   lastStatus == RELEASED) {
			Serial.println("Touchbegin");
		}
	#endif // DEBUG

	if(status == RELEASED) {
		thisTouch.set_touchBegin(millis());	// If sensor is not touched set actual time to begin. Next time we check it already could be touched and then we wont have te begin time!
	}
	else if(status == TOUCHED) {
		thisTouch.set_touchEnd(millis());
	}
	lastStatus = status;
}

bool TouchSensor::touched(unsigned short time, TimeType time_type, bool async) {
	// We use the lastTouch variable because we dont know how long the actual touch will be

	unsigned short touchTime;	// The touch time converted to milliseconds
	unsigned short tolerance_plus;	// How long(in milliseconds) we are allowed to touch the sensor longer than the nominal value
	unsigned short tolerance_minus;	// Same like "tolerance_plus" but only in minus

	switch (time_type) {
		case (TimeType::milliseconds): {
			tolerance_plus	= 250;
			tolerance_minus	= 250;
			touchTime = time;
			break;
		}
		case (TimeType::seconds): {
			tolerance_plus	= 500;
			tolerance_minus	= 500;
			touchTime = time * 1000;
			break;
		}
		default: {
			Serial.println("Unsupported TimeType gave to Touch sensor to check touched Time!");
			return false;
		}
	}

	if(lastTouchUsed != true) {		// If we dont used last Touch check the Touch
		if(lastTouch.get_touchDuration() <= touchTime + tolerance_plus   &&   lastTouch.get_touchDuration() >= touchTime - tolerance_minus) {
			lastTouchUsed = true;
			return true;
		}
	}
	return false;
}


#endif // _TOUCHSENSOR_HPP_INCLUDED_