#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <exception>
#include <stdexcept>
#include <FS.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

/**
 * Definiert ob man ein WiFi Access Point erstellen soll oder sich zu einem bestehende WiFi verbinden soll
 * true  = ein eignen WiFi Access Point erstellen
 * false = mit einem bestehenden WiFi verbinden
 */
bool WiFiAccessPointMode = true;
String Hostname; // Lokale Domain
String WiFiName;
String WiFiPassword;
/**
 * Maximale Anzahl an Clients, die sich mit dem WiFi verbinden koennen.
 * Ist nur wichtig, wenn ein WiFi-Access-Point erstellet wird(WiFiAccessPointMode = true).
 */
unsigned short MaxWiFiCon;

ESP8266WebServer webserver(WiFi.localIP(), 80);

//ESP8266WiFiClass WiFi;

//MDNSResponder MDNS;

#include "lib/Exception.hpp"
#include "lib/Relay.hpp"
#include "lib/WiFiUtils.hpp"
#include "lib/PirSensor.hpp"
#include "lib/TouchSensor.hpp"


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
	if(SPIFFS.exists("/wifi.config")) {
		File WiFiConfig = SPIFFS.open("/wifi.config", "r");
		if(! WiFiConfig) {
			// Fehlermeldung(Dateioeffnung gescheitert)
			Serial.println("Konnte Datei /wifi.config nicht zum Lesen oeffnen/erstellen!");
			throw Spiffs_Exception("Konnte Datei /wifi.config nicht zum Lesen oeffnen/erstellen!");
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

void writeConfigs(String _WiFiName, String _WiFiPassword, String _Hostname, bool _WiFiAccessPointMode = WiFiAccessPointMode, unsigned short _MaxWiFiCon = MaxWiFiCon) {
	File WiFiConfig = SPIFFS.open("/wifi.config", "w");
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

void setup() {
	Serial.begin(9600);
	Serial.println("Serial started");

	initSpiffs();
	Serial.println("SPIFFS started");

	readConfigs();
	Serial.println("Configs read");

	initWifi();
	Serial.println("WiFi started");

	initDNS();
	Serial.println("DNS started");

	initWebServer();
	Serial.println("Web-Server started");
}

void loop() {

	webserver.handleClient();
	MDNS.update();
}
