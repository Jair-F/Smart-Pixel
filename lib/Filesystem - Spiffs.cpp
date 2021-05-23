#include <string>
#include <vector>
#include <algorithm>

#include <stdexcept>
//#include <exception>

#pragma once


// Config

// My own exception classes

class config_error: public std::invalid_argument {
public:
	config_error(const std::string _msg): std::invalid_argument(_msg) { } 
	virtual ~config_error() { }
};

class filesystem_error: public std::runtime_error {
public:
	filesystem_error(const std::string _msg): std::runtime_error(_msg) { } 
	virtual ~filesystem_error() { }
};



class ConfigObject {
private: 
	std::string configKeyWord;
	std::string configValue;
public:
	ConfigObject(std::string _configKeyWord, std::string _configValue): configKeyWord(_configKeyWord), configValue(_configValue) { }
	~ConfigObject() { }

	bool operator==(const ConfigObject& co);
	bool operator!=(const ConfigObject& co) { return ! (*this == co); }

	std::string get_configKeyWord() const	{ return configKeyWord;	}
	std::string get_configValue() const		{ return configValue;	}

	void set_configKeyWord(std::string _configKeyWord)	{ configKeyWord = _configKeyWord;	}
	void set_configValue(std::string _configValue)		{ configValue = _configValue;		}
	void set(std::string _configKeyWord, std::string _configValue);
};

void ConfigObject::set(std::string _configKeyWord, std::string _configValue) {
	configKeyWord = _configKeyWord;
	configValue = _configValue;
}

bool ConfigObject::operator==(const ConfigObject& co) {
	return this->configKeyWord == co.get_configKeyWord() && this->configValue == co.get_configValue();
}





class ConfigGroup {
private:
	std::vector<ConfigObject> ConfigObjects;
	std::string ConfigGroupName;
public:
	ConfigGroup(std::string _ConfigGroupName) : ConfigGroupName(_ConfigGroupName) { }
	ConfigGroup(std::string _ConfigGroupName, std::vector<ConfigObject>& _initList) : ConfigGroupName(_ConfigGroupName), ConfigObjects(_initList) { }

	void set_ConfigGroupName(const std::string _ConfigGroupName) { ConfigGroupName = _ConfigGroupName; }
	std::string get_ConfigGroupName() { return ConfigGroupName; }
	

	// If the object co already exist the function will change it. Otherwise it will create a new object on ConfigObjects
	void set_ConfigObject(const ConfigObject& _co);
	std::string get_ConfigObjectValue(const std::string& _configKeyWord) const;
	ConfigObject get_ConfigObject(const std::string& _configKeyWord) const;
};

void ConfigGroup::set_ConfigObject(const ConfigObject& _co) {
	// search if the object co exists already
	auto objectExist = std::find(ConfigObjects.begin(), ConfigObjects.end(), _co);
	
	// Wenn es gefunden wurde
	if(objectExist != ConfigObjects.end()) {
		objectExist->set(_co.get_configKeyWord(), _co.get_configValue());
	} 
	// Ansonsten neu einfuegen
	else {
		ConfigObjects.insert(ConfigObjects.end(), _co);
	}
}

std::string ConfigGroup::get_ConfigObjectValue(const std::string& _configKeyWord) const {
	for(ConfigObject co : ConfigObjects) {
		if(co.get_configKeyWord() == _configKeyWord) {
			return co.get_configValue();
		}
	}
	throw config_error("Config " + _configKeyWord + " not found ");
}

ConfigObject ConfigGroup::get_ConfigObject(const std::string& _configKeyWord) const {
	for(ConfigObject co : ConfigObjects) {
		if(co.get_configKeyWord() == _configKeyWord) {
			return co;
		}
	}
	throw config_error("Config " + _configKeyWord + " not found ");
}

class ConfigFile {
private:
	std::vector<ConfigGroup> ConfigGroups;
public:

};


// Config


// Spiffs

#include <FS.h>

class Filesystem {
private:

public:
	Filesystem() {
		SPIFFSConfig config;
		// Normally the spiffs format itself if he faild to mount
		// Definiert hier, dass er sich bei Fehlschlag beim einbinden(mounten) nicht formatieren soll
		// We define here, that he should not format itself if he failed to mount
		config.setAutoFormat(false);
		SPIFFS.setConfig(config);
		if(! SPIFFS.begin()) {
			throw filesystem_error("Failed to start Filesystem");
		}
	}
	~Filesystem() {
		SPIFFS.end();
	}
};

class MyFile: public fs::File {
private:
	File file;
	std::string file_type;
public:
	MyFile(const std::string& _path);

	void set_path(const std::string& _path);
	std::string get_path() const;
};

MyFile::MyFile(const std::string& _path) {
	
}

void MyFile::set_path(const std::string& _path) {
	
}

// Spiffs