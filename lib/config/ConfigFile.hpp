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
	 * @return pointer to the object. If Object with _configGroupname does not exist it returns a nullptr;
	 */
	ConfigGroup* getConfigGroup(const String _configGroupName);

	bool existConfigGroup(const String _configGroupName) const;

	void setPath(const String _path);

    /*
	void print() {
		for(auto groups : ConfigGroups) {
			Serial.println(groups.get_ConfigGroupName());
			for(auto co : groups.ConfigObjects) {
				Serial.print('\t');
				Serial.print(co.get_configKeyWord());
				Serial.print(':');
				Serial.println(co.get_configValue());
			}
		}
	}
	*/
};

// Implementations

void ConfigFile::insertConfigGroup(const String _configGroupName) {
	if(this->existConfigGroup(_configGroupName)) {
		// more efficient than push_back if we have a temporary object like here(takes the temporary Object in)
		ConfigGroups.emplace_back(ConfigGroup(_configGroupName));
	}
}

void ConfigFile::insert(const String _configGroupName, const String _configKeyWord, const String _configValue) {
	ConfigGroup* cfg = this->getConfigGroup(_configGroupName);
	//cfg->set_ConfigObject(_configKeyWord, _configValue);
	cfg->set_ConfigObject(ConfigObject(_configKeyWord, _configValue));
}

void ConfigFile::insert(const String _configGroupName, const ConfigObject& _cfo) {
	ConfigGroup* cfg = this->getConfigGroup(_configGroupName);
	//cfg->set_ConfigObject(_cfo.get_configKeyWord(), _cfo.get_configValue());
	cfg->set_ConfigObject(_cfo);
}

ConfigGroup* ConfigFile::getConfigGroup(const String _configGroupName) {
	for(std::size_t i = 0; i < ConfigGroups.size(); i++) {
		if(ConfigGroups[i].get_ConfigGroupName() == _configGroupName) {
			return &(ConfigGroups[i]);
		}
	}
	return nullptr;
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
			Serial.print("Error - path: ");
			Serial.print(_path);
			Serial.println(" does not exist!!");
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
			}
		}
	}
	file.close();
}