#include <string>
#include <vector>
#include <algorithm>

#include <stdexcept>
//#include <exception>

#pragma once

	// !! --------------------- There is still a bug at readConfigFile() - throws an exception -- needs to be fixed --------------------- !!

// Config

// My own exception classes

class config_error: public std::invalid_argument {
public:
	config_error(const String _msg): std::invalid_argument(_msg.c_str()) { } 
	virtual ~config_error() { }
};

class filesystem_error: public std::runtime_error {
public:
	filesystem_error(const String _msg): std::runtime_error(_msg.c_str()) { } 
	virtual ~filesystem_error() { }
};



// Spiffs

#include <FS.h>

/* SPIFFS ist
 FS SPIFFS = FS(FSImplPtr(new spiffs_impl::SPIFFSImpl(
                             FS_PHYS_ADDR,
                             FS_PHYS_SIZE,
                             FS_PHYS_PAGE,
                             FS_PHYS_BLOCK,
                             SPIFFS_MAX_OPEN_FILES)));
*/

// https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html
// https://tttapa.github.io/ESP8266/Chap11%20-%20SPIFFS.html
class Filesystem : public FS {
private:
	
public:
	Filesystem(): FS(SPIFFS) {
		SPIFFSConfig config;
		// Normally the spiffs format itself if he faild to mount
		// Definiert hier, dass er sich bei Fehlschlag beim einbinden(mounten) nicht formatieren soll
		// We define here, that he should not format itself if he failed to mount
		config.setAutoFormat(false);
		this->setConfig(config);
		if(! this->begin()) {
			throw filesystem_error("Failed to start Filesystem");
		}
	}
	virtual ~Filesystem() {
		SPIFFS.end();
	}

	/**
		File open(_path, _mode)
		@param mode
		r	Open text file for reading.  The stream is positioned at the
			beginning of the file.

		r+	Open for reading and writing.  The stream is positioned at the
       		beginning of the file.

		w	Truncate file to zero length or create text file for writing.
			The stream is positioned at the beginning of the file.

		w+	Open for reading and writing.  The file is created if it does
			not exist, otherwise it is truncated.  The stream is
			positioned at the beginning of the file.

		a	Open for appending (writing at end of file).  The file is
			created if it does not exist.  The stream is positioned at the
			end of the file.

		a+	Open for reading and appending (writing at end of file).  The
			file is created if it does not exist.  The initial file
			position for reading is at the beginning of the file, but
			output is always appended to the end of the file.
	*/
};

// Spiffs




class ConfigObject {
private: 
	String configKeyWord;
	String configValue;
public:
	ConfigObject(String _configKeyWord, String _configValue): configKeyWord(_configKeyWord), configValue(_configValue) { }
	~ConfigObject() { }

	bool operator==(const ConfigObject& co);
	bool operator!=(const ConfigObject& co) { return ! (*this == co); }

	String get_configKeyWord() const	{ return configKeyWord;	}
	String get_configValue() const		{ return configValue;	}

	void set_configKeyWord(String _configKeyWord)	{ configKeyWord = _configKeyWord;	}
	void set_configValue(String _configValue)		{ configValue = _configValue;		}
	void set(String _configKeyWord, String _configValue);
};

void ConfigObject::set(String _configKeyWord, String _configValue) {
	configKeyWord = _configKeyWord;
	configValue = _configValue;
}

bool ConfigObject::operator==(const ConfigObject& co) {
	return this->configKeyWord == co.get_configKeyWord() && this->configValue == co.get_configValue();
}





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






/**
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
	ConfigFile(Filesystem& _filesystem, const String _path): filesystem(_filesystem), path(_path) {
		//auto configFile = filesystem.open(_path.c_str(), "r");
		//Serial.println(configFile.fullName());
	}


	// !! --------------------- There is still a bug at readConfigFile() - throws an exception -- needs to be fixed --------------------- !!
	void readConfigFile() {
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
			//Serial.println(line);

			// line[0] is maybe a ConfigGroup if it is "["

			switch(line[0]) {
				case '[': {
					std::size_t endCharacterPos = line.indexOf(']', 1); // Position of the End-Character from ConfigGroup("]")
					// 									   "["			"]"
					String configGroupName = line.substring(1, endCharacterPos);

					insertConfigGroup = configGroupName;
					// Insert ConfigGroup if does not exist;
					if(!this->existConfigGroup(configGroupName)) {
						ConfigGroups.push_back(ConfigGroup(configGroupName));
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

	// If the object co already exist the function will change it. Otherwise it will create a new object
	void insertConfigGroup(const String _configGroupName) {
		if(this->existConfigGroup(_configGroupName)) {
			ConfigGroups.push_back(ConfigGroup(_configGroupName));
		}
	}
	// If the object co already exist the function will change it. Otherwise it will create a new object
	void insert(const String _configGroupName, const String _configKeyWord, const String _configValue) {
		ConfigGroup* cfg = this->getConfigGroup(_configGroupName);
		//cfg->set_ConfigObject(_configKeyWord, _configValue);
		cfg->set_ConfigObject(ConfigObject(_configKeyWord, _configValue));
	}
	// If the object co already exist the function will change it. Otherwise it will create a new object
	void insert(const String _configGroupName, const ConfigObject& _cfo) {
		ConfigGroup* cfg = this->getConfigGroup(_configGroupName);
		//cfg->set_ConfigObject(_cfo.get_configKeyWord(), _cfo.get_configValue());
		cfg->set_ConfigObject(_cfo);
	}

	/**
	 * @return pointer to the object. If Object with _configGroupname does not exist it returns a nullptr;
	 */
	ConfigGroup* getConfigGroup(const String _configGroupName) {
		for(std::size_t i = 0; i < ConfigGroups.size(); i++) {
			if(ConfigGroups[i].get_ConfigGroupName() == _configGroupName) {
				return &(ConfigGroups[i]);
			}
		}
		return nullptr;
	}

	bool existConfigGroup(const String _configGroupName) const {
		for(std::size_t i = 0; i < ConfigGroups.size(); i++) {
			if(ConfigGroups[i].get_ConfigGroupName() == _configGroupName) {
				return true;
			}
		}
		return false;
	}


	void setPath(const String _path) {
		if(filesystem.exists(_path)) {
			path = _path;
		} else {
			Serial.print("Error - path: ");
			Serial.print(_path);
			Serial.println(" does not exist!!");
		}
	}
};


// Config