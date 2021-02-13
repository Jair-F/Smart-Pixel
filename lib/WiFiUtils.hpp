#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <exception>
#include <stdexcept>
#include <FS.h>

#include "Exception.hpp"

#ifndef _WIFIUTILS_HPP_INCLUDED_
#define _WIFIUTILS_HPP_INCLUDED_

/**
 * @param stat WiFi-Status
 * @return Textbasierte Fehlermeldung von WiFi-Status "stat"
 */
String wifiStatusUserOutput(wl_status_t stat);

void initWifi();

void initDNS();

void initWebServer();

/**
 * @param filename Pfad zu Datei
 * @return Dateityp von Datei "filename"
 */
String getFileType(String filename);

/**
 * Sendet dem Client via HTTP-Webserver die geforderte Datei
 * @param path Pfad zur Datei, die vom Client angefordert wurde
 * @return true wenn es die Datei gibt und kein fehler beim oeffnen gibt - sonst false
 */
bool handleFromClientRequestedFile(String path);


// --------------------------- Implementationen ---------------------------


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
	if(MDNS.begin(Hostname, WiFi.localIP())) {

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

// https://tttapa.github.io/ESP8266/Chap11%20-%20SPIFFS.html
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

#endif // _WIFIUTILS_HPP_INCLUDED_