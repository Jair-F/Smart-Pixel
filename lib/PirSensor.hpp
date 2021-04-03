#include <string>
#include <RTClib.h>

#ifndef _PIRSENSOR_HPP_INCLUDED_
#define _PIRSENSOR_HPP_INCLUDED_

class PirSensor {
public:
	PirSensor(unsigned short _pin, String _name = "Pir-Sensor"): pin(_pin), name(_name), something_moved(false) { if(pin >= 0) { pinMode(pin, INPUT); } }
	~PirSensor() { }
	PirSensor(PirSensor&) = default;

	/**
	 * In loop aufrufen
	 * Schaut kontinuierlich ob sich jemand im Zimmer befindet/bewegt und speichert den letzten "Time-Stamp" davon ab
	 */
	//void check(RTC_DS3231& RTC);
	void check();

	void setName(String _newName) { name = _newName; }
	String getName() { return name; }

	unsigned short getPin() { return pin; }
	void setPin(unsigned short _pin);

	//DateTime getLastActiveReport() { return last_active_report; }
	bool getActiveReport() { return something_moved; }
	void resetActiveReport() { something_moved = false; }
private:
	unsigned short pin;
	String name;
	//DateTime last_active_report;
	bool something_moved;
};

// ------------------------ Implementationen ------------------------

/*
void PirSensor::check(RTC_DS3231& RTC) {
	if(digitalRead(pin) == HIGH) {
		last_active_report = RTC.now();
	}
}
*/

void PirSensor::check(){
	if(digitalRead(pin) == HIGH) {
		something_moved = true;
	}
}

void PirSensor::setPin(unsigned short _pin) {
	pin = _pin;
	pinMode(pin, INPUT);
}

#endif // _PIRSENSOR_HPP_INCLUDED_