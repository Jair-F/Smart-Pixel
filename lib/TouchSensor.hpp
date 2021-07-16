#ifndef _TOUCHSENSOR_HPP_INCLUDED_
#define _TOUCHSENSOR_HPP_INCLUDED_

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

private:
	unsigned short pin;
};

#endif // _TOUCHSENSOR_HPP_INCLUDED_