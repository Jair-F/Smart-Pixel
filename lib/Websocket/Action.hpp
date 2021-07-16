#pragma once
#include <Arduino.h>



typedef void (*Websocket_handler)(String& _arguments);

class WebsocketAction {
private:
	String keyValue;
	Websocket_handler handler;
public:
	WebsocketAction(String _keyValue, Websocket_handler _handler): keyValue(_keyValue), handler(_handler) { }
	WebsocketAction(const WebsocketAction& wa);

	const String& get_keyValue() const		{ return keyValue; }
	Websocket_handler get_handler()	const	{ return handler; }

	void set_keyValue(String _keyValue)				{ keyValue = _keyValue; }
	void set_handler(Websocket_handler _handler)	{ handler = _handler; }

	void operator()(String arguments);
};


// Implementation

WebsocketAction::WebsocketAction(const WebsocketAction& wa) {
	this->handler = wa.get_handler();
	this->keyValue = wa.get_keyValue();
}

void WebsocketAction::operator()(String arguments) {
	handler(arguments);
}
