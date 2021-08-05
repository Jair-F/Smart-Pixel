#include <string>

#ifndef _PIRSENSOR_HPP_INCLUDED_
#define _PIRSENSOR_HPP_INCLUDED_

class PirSensor {
private:
	typedef std::function<void (bool pirSensorStatus)> onChange_Handler;	// Handler if the satus changes

	unsigned short pin;
	String name;
	//DateTime last_active_report;

	bool lastStatus;

	onChange_Handler onChange;	// if movmentSinceLastReset changes this function will be called

public:
	PirSensor(unsigned short _pin, String _name = "Pir-Sensor", onChange_Handler handler = [](bool){}): pin(_pin), name(_name) { if(pin >= 0) { pinMode(pin, INPUT); } }
	~PirSensor() { }
	PirSensor(PirSensor&) = default;

	void setName(String _newName) { name = _newName; }
	String getName() { return name; }

	unsigned short getPin() { return pin; }
	void setPin(unsigned short _pin);

	// Last scan status from the sensor
	bool get_Status() { return lastStatus; }

	// call in loop(checks constantly if something moved)
	void loop();

	void set_onChangeHandler(onChange_Handler handler) { onChange = handler; }

	enum Status {
		movement_detected = true, nothing = false
	};
};

// ------------------------ Implementationen ------------------------


void PirSensor::loop() {
	bool actualStatus = digitalRead(pin);

	if(actualStatus != lastStatus) {
		onChange(actualStatus);
	}

	lastStatus = actualStatus;
}

void PirSensor::setPin(unsigned short _pin) {
	pin = _pin;
	pinMode(pin, INPUT);
}

#endif // _PIRSENSOR_HPP_INCLUDED_