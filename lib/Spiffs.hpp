#ifndef _SPIFSS_HPP_INCLUDED_
#define _SPIFSS_HPP_INCLUDED_

#include <FS.h>
#include "WiFiUtils.hpp"
#include "Exception.hpp"

#define CONFIG_FILE "/wifi.config"

// https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html
// https://tttapa.github.io/ESP8266/Chap11%20-%20SPIFFS.html
void initSpiffs() {
	SPIFFSConfig config;
	// Normalerweise formatiert der Spiffs sich selbst, wenn er sich nicht erfolgreich einbinden(mounten) konnten
	// Definiert hier, dass er sich bei Fehlschlag beim einbinden(mounten) nicht formatieren soll
	config.setAutoFormat(false);
	SPIFFS.setConfig(config);
	if (! SPIFFS.begin()) {
		throw std::runtime_error("Spiff-Filesystem failed to mount!");
	}
}

String readFileUntil(File& file, char terminator = '\n') {
	String ret;
	char c;
	file.readBytes(&c, 1);
	while(c != terminator && file.position() != file.size()) {
		ret += c;
		file.readBytes(&c, 1);
	}
	return ret;
}

void readConfigs() {
	// wifi.config ---> WiFi Konfiguration
	Serial.println("WiFi-Config:");
	if(SPIFFS.exists(CONFIG_FILE)) {
		File WiFiConfig = SPIFFS.open(CONFIG_FILE, "r");
		if(! WiFiConfig) {
			// Fehlermeldung(Dateioeffnung gescheitert)
			String message = "Konnte Datei "; message += CONFIG_FILE; message += " nicht zum Lesen oeffnen/erstellen!";
			Serial.println(message);
			throw Spiffs_Exception(message.c_str());
		}
		while(WiFiConfig.position() < WiFiConfig.size()) {
			String data;
			String group;
			group = WiFiConfig.readStringUntil('=');
			data = WiFiConfig.readStringUntil('\n');
			Serial.print(group);
			Serial.print(": ");
			Serial.println(data);

			if(group == "WiFiName") {
				WiFiName = data;
			} else if(group == "WiFiPassword") {
				WiFiPassword = data;
			} else if(group == "MaxConnections") {
				MaxWiFiCon = data.toInt();
			} else if(group == "WiFiAccessPointMode") {
				WiFiAccessPointMode = data.toInt();
			} else if(group == "Hostname") {
				Hostname = data;
			} else {
				Serial.print("Undefinierter Parameter in wifi.config: ");
				Serial.print(group);
				Serial.print(": ");
				Serial.print(data);
				Serial.println(" - Ignoriere");
			}
		}
		WiFiConfig.close();
	} else {
		// Fehlermeldung/Warnung auf dem Display, standartkonfiguration erstellen
		Serial.println("Pfad: /wifi.config existiert nicht!");
	}
}

void writeConfigs(String _WiFiName = WiFiName, String _WiFiPassword = WiFiPassword, String _Hostname = Hostname, bool _WiFiAccessPointMode = WiFiAccessPointMode, unsigned short _MaxWiFiCon = MaxWiFiCon) {
	File WiFiConfig = SPIFFS.open(CONFIG_FILE, "w");
	if(! WiFiConfig) {
		// Fehlermeldung (Fehler beim Offnen zum Config schreiben)
		Serial.println("Fehler beim schreiben in wifi.config");
	} else {
		WiFiConfig.write(	String("WiFiName="				+ _WiFiName				+ '\n').c_str()	);
		WiFiConfig.write(	String("WiFiPassword="			+ _WiFiPassword			+ '\n').c_str()	);
		WiFiConfig.write(	String("Hostname="				+ _Hostname				+ '\n').c_str()	);
		
		// Muss anderst geschrieben werden, da _WiFiAccessPointMode und _MaxWiFiCon nicht Strings/Char-Arrays sind (und es keinen String +-Operator fuer diese gibt)
		WiFiConfig.write("WiFiAccessPointMode=");
		if(_WiFiAccessPointMode == true) {
			WiFiConfig.write('1');
		} else {
			WiFiConfig.write('0');
		}
		WiFiConfig.write('\n');

		WiFiConfig.write("MaxConnections=");
		WiFiConfig.write( String(_MaxWiFiCon).c_str() );
		WiFiConfig.write('\n');
	}
	WiFiConfig.close();
}

#endif // _SPIFSS_HPP_INCLUDED_