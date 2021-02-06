#ifndef _RELAY_HPP_INCLUDED_
#define _RELAY_HPP_INCLUDED_

class Relay{
public:
	Relay() { }
	Relay(unsigned short _pin, bool _status = false);
	Relay(const Relay&) = default;
	Relay& operator=(Relay&) = default;
	~Relay() { }

	void activate()		{ digitalWrite(pin, HIGH);	}
	void deactivate()	{ digitalWrite(pin, LOW);	}
	bool status()		{ return digitalRead(pin);	}

	void setPin(unsigned short _pin);
	unsigned short getPin() { return pin; }
private:
	unsigned short pin;
};

// Implementationen

Relay::Relay(unsigned short _pin, bool _status): pin(_pin) {
	pinMode(_pin, OUTPUT);
	digitalWrite(pin, _status);
}

void Relay::setPin(unsigned short _pin) {
	pin = _pin;
	pinMode(pin, OUTPUT);
}
#endif // _RELAY_HPP_INCLUDED_