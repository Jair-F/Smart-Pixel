#include <Arduino.h>
#include <vector>

#include "ConfigGroup.hpp"
#include "../Filesystem.hpp"

#pragma once


/**
 * Anweisungen, wie die ConfigFile auszusehen hat:
 * 
 * Leerzeichen und Tabulatoren sind nur am Anfang zur Einrueckung erlaubt - nicht vor dem "=" Zeichen und nicht danach - ansonsten wird
 * es zum ConfigKeyWord oder ConfigValue gezaehlt!
 * 
 * 
 * ConfigKeyWord darf kein "[" oder "]" oder "=" am Anfang haben
 * 
 * ConfigGroup:
 * 		ConfigGroup fängt mit "[" an, dann ConfigGroupName und dann "]" z.B. [WiFi]
 * 		ConfigGroupName darf daher nicht "[" oder "]" beinhalten
 * 
 * Dann ConfigKeyWord gefolgt von einem "=" ohne Leerzeichen(ist nicht erlaubt) und darauf ConfigValue
 *  
*/
class ConfigFile {
private:
	std::vector<ConfigGroup> ConfigGroups;
	Filesystem& filesystem;
	String path;
public:
	ConfigFile(Filesystem& _filesystem): filesystem(_filesystem) { }
	ConfigFile(Filesystem& _filesystem, const String _path): filesystem(_filesystem), path(_path) { }

	void readConfigFile();

	// If the object co already exist the function will change it. Otherwise it will create a new object
	void insertConfigGroup(const String _configGroupName);

	// If the object co already exist the function will change it. Otherwise it will create a new object
	void insert(const String _configGroupName, const String _configKeyWord, const String _configValue);

	// If the object co already exist the function will change it. Otherwise it will create a new object
	void insert(const String _configGroupName, const ConfigObject& _cfo);

	/**
	 * @return reference to the object. If Object with _configGroupname it throws an exception;
	 */
	ConfigGroup& getConfigGroup(const String _configGroupName);

	ConfigGroup& operator[](const String _configGroupName);

	bool existConfigGroup(const String _configGroupName) const;

	void setPath(const String _path);

	std::size_t numOfConfigGroups() { return ConfigGroups.size(); }

	ConfigGroup& operator[](std::size_t _pos) { return ConfigGroups[_pos]; }

	// return the whole config data structure as a string for printing out ...
	String print() {
		String ret;
		for(std::size_t i = 0; i < ConfigGroups.size(); ++i) {
			ret += ConfigGroups[i].get_ConfigGroupName();
			ret += '\n';
			//Serial.println(ConfigGroups[i].get_ConfigGroupName());
			for(std::size_t a = 0; a < ConfigGroups[i].numOfConfigObjects(); ++a) {
				ret += '\t';
				//Serial.print('\t');
				ret += ConfigGroups[i][a].get_configKeyWord();
				//Serial.print(ConfigGroups[i][a].get_configKeyWord());
				ret += ':';
				//Serial.print(':');
				ret += ConfigGroups[i][a].get_configValue();
				//Serial.println(ConfigGroups[i][a].get_configValue());
				ret += '\n';
			}
		}
		return ret;
	}
};

// Implementations

void ConfigFile::insertConfigGroup(const String _configGroupName) {
	if(this->existConfigGroup(_configGroupName)) {
		// more efficient than push_back if we have a temporary object like here(takes the temporary Object in)
		ConfigGroups.emplace_back(ConfigGroup(_configGroupName));
	}
}

void ConfigFile::insert(const String _configGroupName, const String _configKeyWord, const String _configValue) {
	ConfigGroup& cfg = this->getConfigGroup(_configGroupName);
	//cfg->set_ConfigObject(_configKeyWord, _configValue);
	cfg.set_ConfigObject(ConfigObject(_configKeyWord, _configValue));
}

void ConfigFile::insert(const String _configGroupName, const ConfigObject& _cfo) {
	ConfigGroup& cfg = this->getConfigGroup(_configGroupName);
	//cfg->set_ConfigObject(_cfo.get_configKeyWord(), _cfo.get_configValue());
	cfg.set_ConfigObject(_cfo);
}

ConfigGroup& ConfigFile::operator[](const String _configGroupName) {
	for(std::size_t i = 0; i < ConfigGroups.size(); i++) {
		if(ConfigGroups[i].get_ConfigGroupName() == _configGroupName) {
			return ConfigGroups[i];
		}
	}
	throw config_error("ConfigGroup " + _configGroupName + " not found in config-file " + path + "!");
}

ConfigGroup& ConfigFile::getConfigGroup(const String _configGroupName) {
	return this->operator[](_configGroupName);
}

bool ConfigFile::existConfigGroup(const String _configGroupName) const {
	for(std::size_t i = 0; i < ConfigGroups.size(); i++) {
		if(ConfigGroups[i].get_ConfigGroupName() == _configGroupName) {
			return true;
		}
	}
	return false;
}

void ConfigFile::setPath(const String _path) {
	if(filesystem.exists(_path)) {
		path = _path;
	} else {
		throw filesystem_error("Error - path: " + _path + " does not exist!!");
	}
}

void ConfigFile::readConfigFile() {
	File file = filesystem.open(path.c_str(), "r");

	// ConfigGroup in which we actually insert the ConfigObjects
	String insertConfigGroup;

	while(file.position() < file.size()) {
		// Ignore Spaces - seek forward
		char buffer;
		file.readBytes(&buffer, 1);
		while(buffer == '\t' || buffer == ' ' || buffer == '\n' && file.position() < file.size()) {
			file.readBytes(&buffer, 1);
		}
		// go 1 back - then we are on the beging of the text
		file.seek(file.position() - 1);

		// and then read Read-Line
		String line = file.readStringUntil('\n');

		// line[0] is maybe a ConfigGroup if it is "["
		switch(line[0]) {
			case '[': {
				std::size_t endCharacterPos = line.indexOf(']', 1); // Position of the End-Character from ConfigGroup("]")
				// 									   "["			"]"
				String configGroupName = line.substring(1, endCharacterPos);

				insertConfigGroup = configGroupName;
				// Insert ConfigGroup if does not exist;
				if(!this->existConfigGroup(configGroupName)) {
					// more efficient than push_back if we have a temporary object like here(takes the temporary Object in)
					ConfigGroups.emplace_back(ConfigGroup(configGroupName));
				}
				break;
			}
			default: {
				std::size_t endConfigKeywordPos	= line.indexOf('=', 0);

				String configKeyWord = line.substring(0, endConfigKeywordPos);
				String configValue = line.substring(endConfigKeywordPos + 1, line.length());

				this->insert(insertConfigGroup, configKeyWord, configValue);
				break;
			}
		}
	}
	file.close();
}