#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <string>
#include <exception>
#include <stdexcept>
#include <FS.h>

#include <DHT.h>

#include "lib/Exception.hpp"
#include "lib/Relay.hpp"

/**
 * Definiert ob man ein WiFi Access Point erstellen soll oder sich zu einem bestehende WiFi verbinden soll
 * true  = ein eignen WiFi Access Point erstellen
 * false = mit einem bestehenden WiFi verbinden
 */
bool WiFiAccessPointMode = false;
String Hostname;
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

// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html#check-return-codes
String wifiStatusUserOutput(wl_status_t stat) {
	String message;	// The message that will return(fail or success)
	switch (stat) {
		case WL_CONNECTED: {
			message = "Connected successfully to Network " + WiFi.SSID();
			break;
		}
		case WL_NO_SHIELD: {
			message = "No WiFi-Shield is connected to the board";
			break;
		}
		case WL_IDLE_STATUS: {
			message = "ESP8266 is connecting to " + WiFi.SSID();
			break;
		}
		case WL_NO_SSID_AVAIL: {
			message = "WiFi-Network " + WiFi.SSID() + " is not in range";
			break;
		}
		case WL_SCAN_COMPLETED: {
			message = "WiFi-Scan completed";
			break;
		}
		case WL_CONNECT_FAILED: {
			message = "Failed to connect to Network: " + WiFi.SSID() + " - Wrong Password?";
			break;
		}
		case WL_CONNECTION_LOST: {
			message = "Lost connection to Network " + WiFi.SSID();
			break;
		}
		case WL_DISCONNECTED: {
			message = "WiFi is disconnected -> Connecting to network " + WiFi.SSID();
			break;
		}
		default: {
			message = "An Unknow error occured!\n";
			message += "A restart may help/solve the problem";
			break;
		}
	}
	return message;
}

// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
void initWifi() {
	if(WiFiAccessPointMode == true) {
		bool WiFiAccessPointCreated = WiFi.softAP(WiFiName, WiFiPassword, 1, 0, MaxWiFiCon);
		if(! WiFiAccessPointCreated) {
			throw WiFi_Exception("WiFi AccessPoint failed to start!");
		}
	} else {
		Serial.println("Networks in range: ");
		int8_t numOfDiscoverdNetworks = WiFi.scanNetworks();
		for(int8_t i = 0; i < numOfDiscoverdNetworks; i++) {
			Serial.println(String(WiFi.SSID(i) + ' ' + WiFi.encryptionType(i)));
		}
		WiFi.scanDelete(); // Gefundene Netzwerke entfernen, im fall dass wenn nochmal gesucht wird, sie nicht erscheinen
		
		wl_status_t wifiStatus = WiFi.begin(WiFiName, WiFiPassword);

		while(wifiStatus != WL_CONNECTED) {
			WiFi.waitForConnectResult();	// Warten, bis WiFi ein Ergebnis hat(Timout,Successfully connected...)
			wifiStatus = WiFi.status();

			Serial.println(wifiStatusUserOutput(wifiStatus));
			delay(1000);
		}
	}
}

// https://tttapa.github.io/ESP8266/Chap08%20-%20mDNS.html
void initDNS() {
	if(MDNS.begin("hostname", WiFi.localIP())) {

	} else {
		throw std::runtime_error("DNS-Service failed to start!");
	}
}

// https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
String getFileType(String filename) {
	if		(filename.endsWith(".html"))	return "text/html";
	else if	(filename.endsWith(".css"))		return "text/css";
	else if (filename.endsWith(".js"))		return "text/javascript";
	else if	(filename.endsWith(".ico"))		return "image/vnd.microsoft.icon";
	else if (filename.endsWith(".gz"))		return "application/gzip";
	else if (filename.endsWith(".jpeg"))	return "image/jpeg";
	else if (filename.endsWith("jpg"))		return "image/jpeg";
	else									return "text/plain";
}

// https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
void initWebServer() {
	webserver.onNotFound (
		[] () {		// inline Funktion - muss nicht extra globale Funktion machen
			if(! handleFromClientRequestedFile(webserver.uri())) {	// Wenn es den vom Client verlangten Pfad nicht gibt eine Fehlermeldung schicken
				webserver.send(404, "text/plain", "404: Site not found");
			}
		}
	);
	webserver.begin();
}

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

// https://tttapa.github.io/ESP8266/Chap11%20-%20SPIFFS.html
/**
 * @return true wenn es die Datei gibt und kein fehler beim oeffnen gibt - sonst false
 */
bool handleFromClientRequestedFile(String path) {

	Serial.print("Angefordert: ");
	Serial.print(path);

	if(path.endsWith("/")) { path += "index.html"; } // Wenn ein Ordner verlangt wird, die index.html Datei geben
	String FileType = getFileType(path);
	if(SPIFFS.exists(path)) {
		File file = SPIFFS.open(path, "r");
		if(! file) {
			Serial.println("Fehler beim oeffnen der Datei");
			return false;
		}
		webserver.streamFile(file, FileType);
		file.close();
		Serial.print(" -> ");
		Serial.println(path + " gesendet");
	}
	return true;
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

	initSpiffs();

	readConfigs();

	initWifi();

	initDNS();

	initWebServer();
}

void loop() {
	
	webserver.handleClient();
	MDNS.update();
}
