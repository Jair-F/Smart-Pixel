#include <Arduino.h>
#include <vector>

#include "ConfigObject.hpp"
#include "../Exception.hpp"

#pragma once


class ConfigGroup {
private:
	std::vector<ConfigObject> ConfigObjects;
	String ConfigGroupName;
public:
	ConfigGroup(String _ConfigGroupName) : ConfigGroupName(_ConfigGroupName) { }
	ConfigGroup(String _ConfigGroupName, std::vector<ConfigObject>& _initList) : ConfigGroupName(_ConfigGroupName), ConfigObjects(_initList) { }

	void set_ConfigGroupName(const String _ConfigGroupName) { ConfigGroupName = _ConfigGroupName; }
	String get_ConfigGroupName() const { return ConfigGroupName; }
	

	// If the object co already exist the function will change it. Otherwise it will create a new object on ConfigObjects
	void set_ConfigObject(const ConfigObject& _co);
	// If the object co already exist the function will change it. Otherwise it will create a new object on ConfigObjects
	//void set_configObject(const String _configKeyWord, const String _configValue) { this->set_configObject(ConfigObject(_configKeyWord, _configValue)); }
	String get_ConfigObjectValue(const String _configKeyWord) const;
	ConfigObject get_ConfigObject(const String _configKeyWord) const;
};

void ConfigGroup::set_ConfigObject(const ConfigObject& _co) {
	// search if the object co exists already
	bool foundObject = false;
	for(std::size_t i = 0; i < ConfigObjects.size(); i++) {
		if(ConfigObjects[i].get_configKeyWord() == _co.get_configKeyWord()) {
			foundObject = true;
			ConfigObjects[i].set_configValue(_co.get_configValue());
		}
	}

	if(! foundObject) {
		ConfigObjects.push_back(_co);
	}
}

String ConfigGroup::get_ConfigObjectValue(const String _configKeyWord) const {
	for(ConfigObject co : ConfigObjects) {
		if(co.get_configKeyWord() == _configKeyWord) {
			return co.get_configValue();
		}
	}
	throw config_error("Config " + _configKeyWord + " not found ");
}

ConfigObject ConfigGroup::get_ConfigObject(const String _configKeyWord) const {
	for(ConfigObject co : ConfigObjects) {
		if(co.get_configKeyWord() == _configKeyWord) {
			return co;
		}
	}
	throw config_error("Config " + _configKeyWord + " not found ");
}
