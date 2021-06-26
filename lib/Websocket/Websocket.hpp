#pragma once
#include <Arduino.h>
#include <WebSocketsServer.h>

#include "ActionGroup.hpp"
#include "../Exception.hpp"
#include "../Helper.hpp"	// for to_string()



class Websocket: public WebSocketsServer {
private:
	WebsocketActionGroup actions;

	// seperator between the keyValue and their arguments(WebsocketAction) in the string which sends the client back to the server
	char seperator = '|';
public:
	Websocket(uint16_t port, const String& origin="", const String& protocol="arduino");
	Websocket(WebsocketActionGroup& wag, uint16_t port, const String& origin="", const String& protocol="arduino");
	virtual ~Websocket() { }

	void addAction(const WebsocketAction& ag);
	void addAction(String keyValue, Websocket_handler handler);

	/**
	 * @param _seperator seperator between the keyValue and their arguments(WebsocketAction) in the string which sends the client back to the server
	 */
	void set_seperator(char _seperator) { seperator = _seperator; }
protected:
	/** 
	 * Function, which is called on a Websocket Event, it directs it to the function saved in actions according to their keyValue
	 * !!-- Cant be a class member function because then I cant pass it to at the Constructuro at this->onEvent(...). Therefore
	 * it must to be a Lambda Function in the class because i need to access the Websocket variables without. I can't pass a reference or
	 * an other objetc of the Websocket, because the Websocket defined the Handler-function without that.
	 */
	WebSocketsServer::WebSocketServerEvent WebsocketEvent = [&](uint8_t ClientNum, WStype_t WebsocketStatus, uint8_t* payload, std::size_t payload_length) {
		switch(WebsocketStatus) {
			case WStype_CONNECTED: {
				Serial.println("Client: " + remoteIP(ClientNum).toString() + " connected");
				break;
			}
			case WStype_DISCONNECTED: {
				Serial.println("Client: " + remoteIP(ClientNum).toString() + " disconnected");
				break;
			}
			case WStype_ERROR: {
				Serial.println("Websocket-ERROR: Client: " + remoteIP(ClientNum).toString() + " Payload: \"" + reinterpret_cast<char*>(payload) + "\"");
				break;
			}
			case WStype_BIN: {		// Maybe we could do here cool things

				break;
			}
			case WStype_TEXT: {		// Got Text
				String keyValue, arguments;
			
				// Extract keyValue and arguments

				// Maybe catch an error because payload dont have '\0'. need to set it with payload_length
				String _payload(reinterpret_cast<char*>(payload));
				int seperator_pos;
				try {
					// _payload.indexOf(...) returns -1 if there was an error(also if seperator was not found)
					seperator_pos = _payload.indexOf(seperator, 0);	// Search from the begining

					if(seperator_pos == -1) {
						throw Websocket_error("Got Unsupported Websocket-request-format from client: " + remoteIP(ClientNum).toString() + ") payload: \"" + _payload + "\"");
					}
				}
				catch(Websocket_error& exc) {
					Serial.println("Websocket_ERROR: ");
					Serial.println(exc.what());
					Serial.println("Ignoring...");
					return;
				}
				catch(std::exception& exc) {
					Serial.println("ERROR: ");
					Serial.println(exc.what());
					Serial.println("Ignoring...");
					return;
				}
				catch(...) {
					Serial.println("Unknown ERROR: ");
					Serial.println("Ignoring...");
					return;
				}

				keyValue = _payload.substring(0, seperator_pos);
				arguments = _payload.substring(seperator_pos + 1, _payload.length());

				for(unsigned int i = 0; i < actions.numOfActions(); ++i) {
					if(actions[i].get_keyValue() == keyValue) {
						actions[i](arguments);
						break;
					}
				}
				break;
			}
			case WStype_FRAGMENT: {

				break;
			}
			case WStype_FRAGMENT_BIN_START: {

				break;
			}
			case WStype_FRAGMENT_FIN: {

				break;
			}
			case WStype_FRAGMENT_TEXT_START: {
				break;
			}
			case WStype_PING: {
				
				break;
			}
			case WStype_PONG: {
				break;
			}
			default: {
				Serial.println("Client: " + remoteIP(ClientNum).toString() + " send unknown or unsupported Websockt Action: " + to_string(WebsocketStatus) + " Payload: \"" +reinterpret_cast<char*>(payload) + "\"");
				break;
			}
		}
	};
};

Websocket::Websocket(uint16_t port, const String& origin, const String& protocol): WebSocketsServer(port, origin, protocol) {
	this->onEvent(WebsocketEvent);
	// Reduce resources of transfer unneeded data
	//this->disableHeartbeat();
}

Websocket::Websocket(WebsocketActionGroup& wag, uint16_t port, const String& origin, const String& protocol): WebSocketsServer(port, origin, protocol), actions(wag) { }

void Websocket::addAction(const WebsocketAction& ag) {
	actions.add(ag);
}

/*
void Websocket::addAction(String keyValue, Websocket_handler handler) {
	this->addAction(WebsocketAction(keyValue, handler));
}
*/
