#pragma once

#include <Arduino.h>
#include <vector>

#include "ConfigObject.hpp"
#include "../Exception.hpp"


class ConfigGroup {
private:
	std::vector<ConfigObject> ConfigObjects;
	String ConfigGroupName;
public:
	ConfigGroup(String _ConfigGroupName) : ConfigGroupName(_ConfigGroupName) { }
	ConfigGroup(String _ConfigGroupName, std::vector<ConfigObject>& _initList) : ConfigGroupName(_ConfigGroupName), ConfigObjects(_initList) { }

	void set_GroupName(const String _ConfigGroupName) { ConfigGroupName = _ConfigGroupName; }
	String get_GroupName() const { return ConfigGroupName; }

	// If the object co already exist the function will change it. Otherwise it will create a new object on ConfigObjects
	void set_ConfigObject(const ConfigObject& _co);
	// If the object co already exist the function will change it. Otherwise it will create a new object on ConfigObjects
	//void set_configObject(const String _configKeyWord, const String _configValue) { this->set_configObject(ConfigObject(_configKeyWord, _configValue)); }
	String get_ConfigObjectValue(const String _configKeyWord) const;
	ConfigObject& get_ConfigObject(const String& _configKeyWord);
	ConfigObject& get_ConfigObject(const char* _configKeyWord);

	ConfigObject& operator[](const String& _configKeyWord);
	ConfigObject& operator[](const char* _configKeyWord);

	std::size_t numOfConfigObjects() { return ConfigObjects.size(); }

	ConfigObject& operator[](std::size_t _pos) { return ConfigObjects[_pos]; }
};

void ConfigGroup::set_ConfigObject(const ConfigObject& _co) {
	// search if the object co exists already
	bool foundObject = false;
	for(std::size_t i = 0; i < ConfigObjects.size(); i++) {
		if(ConfigObjects[i].get_KeyWord() == _co.get_KeyWord()) {
			foundObject = true;
			ConfigObjects[i].set_Value(_co.get_Value());
		}
	}

	if(! foundObject) {
		ConfigObjects.push_back(_co);
	}
}

String ConfigGroup::get_ConfigObjectValue(const String _configKeyWord) const {
	for(ConfigObject co : ConfigObjects) {
		if(co.get_KeyWord() == _configKeyWord) {
			return co.get_Value();
		}
	}
	throw config_error("Config " + _configKeyWord + " not found ");
}

ConfigObject& ConfigGroup::operator[](const String& _configKeyWord) {
	return this->get_ConfigObject(_configKeyWord);
}

ConfigObject& ConfigGroup::operator[](const char* _configKeyWord) {
	return this->get_ConfigObject(String(_configKeyWord));
}

ConfigObject& ConfigGroup::get_ConfigObject(const String& _configKeyWord) {
	for(std::size_t i = 0; i < ConfigObjects.size(); i++) {
		if(ConfigObjects[i].get_KeyWord() == _configKeyWord) {
			return ConfigObjects[i];
		}
	}
	throw config_error("Config " + _configKeyWord + " not found in ConfigGroup " + ConfigGroupName + "!");
}


ConfigObject& ConfigGroup::get_ConfigObject(const char* _configKeyWord) {
	return this->get_ConfigObject(String(_configKeyWord));
}
