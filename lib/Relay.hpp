#ifndef _RELAY_HPP_INCLUDED_
#define _RELAY_HPP_INCLUDED_

class Relay {
public:
	Relay() { }
	Relay(unsigned short _pin, String _name = "Relay", bool _status = false);
	Relay(const Relay&) = default;
	~Relay() { }

	void activate()		{ digitalWrite(pin, HIGH);				}
	void deactivate()	{ digitalWrite(pin, LOW);				}
	void switchStatus()	{ digitalWrite(pin, !digitalRead(pin));	}
	bool status()		{ return digitalRead(pin);				}
	
	String getName() { return name; }
	void setName(String _name) { name = _name; }  

	void setPin(unsigned short _pin);
	unsigned short getPin() { return pin; }
private:
	unsigned short pin;
	String name;
};

// Implementationen

Relay::Relay(unsigned short _pin, String _name, bool _status): pin(_pin), name(_name) {
	pinMode(_pin, OUTPUT);
	digitalWrite(pin, _status);
}

void Relay::setPin(unsigned short _pin) {
	pin = _pin;
	pinMode(pin, OUTPUT);
}
#endif // _RELAY_HPP_INCLUDED_