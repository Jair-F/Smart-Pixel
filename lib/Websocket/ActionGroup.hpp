#pragma once
#include <Arduino.h>
#include <vector>
#include "Action.hpp"


class WebsocketActionGroup {
private:
	std::vector<WebsocketAction> actions;
public:
	WebsocketActionGroup();
	// If a element with the same keyValue exist like wa, it overwrites it. Otherwise it inserts wa.
	void add(const WebsocketAction& wa);
	// void add(WebsocketActionGroup& wag);

	std::size_t numOfActions()	const 				{ return actions.size(); }
	WebsocketAction& operator[](std::size_t i)		{ return actions[i]; }
	WebsocketAction& at(std::size_t i)				{ return actions.at(i); }
};

WebsocketActionGroup::WebsocketActionGroup() { }

void WebsocketActionGroup::add(const WebsocketAction& wa) {
	for(WebsocketAction& tmp : actions) {
		if(tmp.get_keyValue() == wa.get_keyValue()) {
			tmp = wa;
			break;
		}
	}
	actions.push_back(wa);
}

/*
void WebsocketActionGroup::add(WebsocketActionGroup& wag) {
	for(std::size_t i = 0; i < wag.numOfActions(); ++i) {
		
	}
}
*/

