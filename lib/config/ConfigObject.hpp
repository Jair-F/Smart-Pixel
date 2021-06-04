#include <Arduino.h>

#pragma once


class ConfigObject {
private: 
	String configKeyWord;
	String configValue;
public:
	ConfigObject(String _configKeyWord, String _configValue): configKeyWord(_configKeyWord), configValue(_configValue) { }
	~ConfigObject() { }

	bool operator==(const ConfigObject& co);
	bool operator!=(const ConfigObject& co) { return ! (*this == co); }

	String get_KeyWord() const	{ return configKeyWord;	}
	String get_Value() const		{ return configValue;	}

	void set_KeyWord(String _configKeyWord)	{ configKeyWord = _configKeyWord;	}
	void set_Value(String _configValue)		{ configValue = _configValue;		}
	void set(String _configKeyWord, String _configValue);
};

void ConfigObject::set(String _configKeyWord, String _configValue) {
	configKeyWord = _configKeyWord;
	configValue = _configValue;
}

bool ConfigObject::operator==(const ConfigObject& co) {
	return this->configKeyWord == co.get_KeyWord() && this->configValue == co.get_Value();
}