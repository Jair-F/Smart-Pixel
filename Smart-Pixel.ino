#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <string>
#include <exception>
#include <stdexcept>
#include <FS.h>

/**
 * Definiert ob man ein WiFi Access Point erstellen soll oder sich zu einem bestehende WiFi verbinden soll
 * true  = ein eignen WiFi Access Point erstellen
 * false = mit einem bestehenden WiFi verbinden
 */
bool WiFiAccessPointMode = true;

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
	if (WiFiAccessPointMode == true) {
		bool WiFiAccessPointCreated = WiFi.softAP("ssid", "password", 1, 0, 4);
		if (! WiFiAccessPointCreated) {
			throw std::runtime_error("WiFi AccessPoint failed to start!");
		}
	} else {
		Serial.println("Networks in range: ");
		int8_t numOfDiscoverdNetworks = WiFi.scanNetworks();
		for(int8_t i = 0; i < numOfDiscoverdNetworks; i++) {
			String mess = WiFi.SSID(i) + ' ' + WiFi.encryptionType(i);
			Serial.println(mess);
		}
		WiFi.scanDelete(); // Gefundene Netzwerke entfernen, im fall dass wenn nochmal gesucht wird, sie nicht erscheinen
		
		wl_status_t wifiStatus = WiFi.begin("WiFi-Name", "password");

		while (wifiStatus != WL_CONNECTED) {
			WiFi.waitForConnectResult();	// Warten, bis WiFi ein Ergebnis hat(Timout,Successfully connected...)
			wifiStatus = WiFi.status();

			Serial.println(wifiStatusUserOutput(wifiStatus));
			delay(1000);
		}
	}
}

// https://tttapa.github.io/ESP8266/Chap08%20-%20mDNS.html
void initDNS() {
	if (MDNS.begin("hostname", WiFi.localIP())) {

	} else {
		throw std::runtime_error("DNS-Service failed to start!");
	}
}

// https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
void initWebServer() {
	webserver.onNotFound (
		[] () {		// inline Funktion - muss nicht extra globale Funktion machen
			webserver.send(404, "text/plan", "404: Site not found");
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

void setup() {
	Serial.begin(9600);

	initSpiffs();
	
	initWifi();

	initDNS();

	initWebServer();
}

void loop() {
	
	webserver.handleClient();
	MDNS.update();
}
