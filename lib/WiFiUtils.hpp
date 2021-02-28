#ifndef _WIFIUTILS_HPP_INCLUDED_
#define _WIFIUTILS_HPP_INCLUDED_

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "Exception.hpp"
#include "RGBRing.hpp"
#include "Spiffs.hpp"


void initWifi();

void initDNS();

void initWebServer();

/**
 * @param stat WiFi-Status
 * @return Textbasierte Fehlermeldung von WiFi-Status "stat"
 */
String wifiStatusUserOutput(wl_status_t stat);

/**
 * @param filename Pfad zu Datei
 * @return Dateityp von Datei "filename"
 */
String getFileType(String filename);

/**
 * Schickt die vom Cliente gefordereten Dateien, gibt Fehlermeldungen bei nicht gefundenen Dateien und
 * fuehrt Aktionen auf zurueckgesendete Daten aus.
 */
void handleClientandActions();

/**
 * Fuehrt die vom Client gesendeten Parameter aus
 */
void make_action(const String argName, const String arg);

/**
 * @return leerer String wenn es die Datei nicht gibt, sonst Pfad zur Datei
 * @param path Pfad-URL zur angeforderten Datei
 */
String find_path_to_req_File(String path);


// --------------------------- Implementationen ---------------------------

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
	if(MDNS.begin(Hostname, WiFi.localIP())) {

	} else {
		throw std::runtime_error("DNS-Service failed to start!");
	}
}

// https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
// https://arduino.stackexchange.com/questions/39599/esp8266-cannot-read-post-parameters

//??
// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/server-examples.html
void initWebServer() {
	webserver.onNotFound(handleClientandActions);
	webserver.begin();
}

// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html#check-return-codes
String wifiStatusUserOutput(wl_status_t stat) {
	String message;	// Die Nachricht, die zurueckgegeben wird(Erfolg oder nicht)
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


void handleClientandActions() {
	// Datei-streamen
	String pathToFile = find_path_to_req_File(webserver.uri());
	if(pathToFile.isEmpty()) {	// Wenn es den vom Client verlangten Pfad nicht gibt eine Fehlermeldung schicken
		webserver.send(404, "text/plain", "404: Site not found");
	} else {
		File file = SPIFFS.open(pathToFile, "r");
		if(! file) {
			Serial.println(String("Fehler beim oeffnen der Datei" + pathToFile));
			webserver.send(404, "text/plain", String("Fehler beim oeffnen der Datei" + pathToFile));
		} else {
			webserver.streamFile(file, getFileType(pathToFile));
		}
		file.close();
	}

	// Aktionen auf zurueckgesendete Daten
	if(webserver.method() == HTTP_POST) {
		for(unsigned short i = 0; i < webserver.args(); i++) {
			make_action(webserver.argName(i), webserver.arg(i));
		}
	}
}

void make_action(const String argName, const String arg) {
	Serial.println("Params:");
	Serial.print(argName);
	Serial.print(':');
	Serial.println(arg);
	if(argName == "Color") {
		aktueller_Effekt = nullptr;	// Wenn gerade Effekt gelaufen ist, ihn abschalten
		for(unsigned short pixel = 0; pixel < RGB_LEDS.numPixels(); pixel++) {
			RGB_LEDS.setPixelColor(pixel, RGBHexToColor(arg.c_str()));
		}
		RGB_LEDS.show();
	} else if (argName == "Effekt") {
		for(unsigned short i = 0; i < EffektContainer.size(); i++) {
			if(EffektContainer[i].getName() == arg) {
				if(arg == "Nothing") {
					aktueller_Effekt = nullptr;
				} else {
					aktueller_Effekt = &EffektContainer[i];
				}
			}
		}
	} else if (argName == "EffektSpeed") {
		EffektSpeed = arg.toInt();
	} else if (argName == "WiFiAccessPointMode") {
		WiFiAccessPointMode = arg.toInt();
	} else if (argName == "WiFi-Name") {
		WiFiName = arg;
	} else if (argName == "WiFi-Passwort") {
		WiFiPassword = arg;
	} else if (argName == "MaxConnections") {
		MaxWiFiCon = arg.toInt();
	} else if(argName == "Hostname") {
		Hostname = arg;
	}
	// Als letztes wird immer noch plain als arg gegeben - das nutze ich um am Ende die Configs in den Spiffs zu schreiben
	else if (argName == "plain" && arg.indexOf("WiFiAccessPointMode") > 0 || arg.indexOf("WiFi-Name") > 0 || arg.indexOf("WiFi-Passwort") || arg.indexOf("MaxConnections") || arg.indexOf("Hostname") > 0) {
		writeConfigs();
		Serial.println("Write-Configs - changed");
	}
	/**
	 * "plain" gibt nochmals die Argumente als ein String zurueck wie sie oben im Browser in der Titelleiste stehen wuerde,
	 * wenn im html "method=GET" steht.
	 */
	else if(argName == "plain") {

	} else {
		Serial.println("Unknow Arg:");
		Serial.print(argName);
		Serial.print(':');
		Serial.println(arg);
	}
}

// https://tttapa.github.io/ESP8266/Chap11%20-%20SPIFFS.html
String find_path_to_req_File(String path) {
	
	if(path.endsWith("/")) { path += "index.html"; } // Wenn ein Ordner verlangt wird, die index.html Datei geben
	String FileType = getFileType(path);
	if(SPIFFS.exists(path)) {
		return path;
	} else {
		path.clear();
		return path;
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

#endif // _WIFIUTILS_HPP_INCLUDED_