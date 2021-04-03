#ifndef _WIFIUTILS_HPP_INCLUDED_
#define _WIFIUTILS_HPP_INCLUDED_

#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "Exception.hpp"
#include "RGBRing.hpp"
#include "Spiffs.hpp"
#include "PirSensor.hpp"


void initWifi();

void initWebSockets();

void initDNS();

void initWebServer();

void handleWebSocket(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght);

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
void handleWebServer();

/**
 * Fuehrt die vom Client gesendeten Parameter aus
 */
void make_action(const String argName, const String arg);

/**
 * @return leerer String wenn es die Datei nicht gibt, sonst Pfad zur Datei
 * @param path Pfad-URL zur angeforderten Datei
 */
String find_path_to_req_File(String path);

/**
 * @param aim "From" oder "HTML" oder "CSS", je nachdem ob eine From oder ein Text im HTML geaendert wird
 * @param aimType Das "Atribut", das geaendert werden soll(value, innerHTML, backgroundColor...)
 * @param argName Die ID im HTML-Code
 * @param arg Der Wert, der gesetzt werden soll
 */
void WebSocketSendData(String aim, String aimType, String argName, String arg);

/**
 * Die Website der Clients wird dynamisch durch den Websocket aktualisiert - Sensordaten, EffektSpeed, Effekt, RGB-Farbwert...
 * @param force Wenn die Daten gesendet werden sollen, auch wenn der Timer noch nicht vorbei ist
 */
void dynamicUpdateClientWebsite(bool force = false);

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
	webserver.onNotFound(handleWebServer);
	webserver.begin();
}

void initWebSockets() {
	WebSocket.begin();
	WebSocket.onEvent(handleWebSocket);
}

// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html#check-return-codes
String wifiStatusUserOutput(wl_status_t stat) {
	String message;	// Die Nachricht, die zurueckgegeben wird(Erfolg oder nicht)
	switch (stat) {
		case WL_CONNECTED: {
			message = "Erfolgreich mit dem  Netzwerk " + WiFi.SSID() + " verbunden";
			break;
		}
		case WL_NO_SHIELD: {
			message = "Kein WiFi-Shield mit dem Board verbunden";
			break;
		}
		case WL_IDLE_STATUS: {
			message = "ESP8266 verbindet sich mit: " + WiFi.SSID();
			break;
		}
		case WL_NO_SSID_AVAIL: {
			message = "WiFi-Netzwerk " + WiFi.SSID() + " ist ausser Reichweite";
			break;
		}
		case WL_SCAN_COMPLETED: {
			message = "WiFi-Scan fertig";
			break;
		}
		case WL_CONNECT_FAILED: {
			message = "Verbindung mit dem Netzwerk: " + WiFi.SSID() + " ist fehlgeschlagen - Falsches Passwort?";
			break;
		}
		case WL_CONNECTION_LOST: {
			message = "Verbindung zu Netzwerk " + WiFi.SSID() + " verloren";
			break;
		}
		case WL_DISCONNECTED: {
			message = "WiFi ist nicht verbunden -> Verbinden mit dem Netzwerk " + WiFi.SSID();
			break;
		}
		default: {
			message = "Unbekannter Fehler aufgetreten!\n";
			message += "Ein Neustart hilft vielleicht";
			break;
		}
	}
	return message;
}

void WebSocketSendData(String aim, String aimType, String argName, String arg) {
	WebSocket.broadcastTXT(String(aim + ":" + aimType + ":" + argName + ":" + arg).c_str());
}

void dynamicUpdateClientWebsite(bool force) {
	static unsigned long last_update = 0;
	if(WebSocket.connectedClients() > 0) {
		if(last_update + DYNAMIC_WEBSITE_UPDATE_INTERVAL < millis() || force == true) {
			WebSocketSendData("HTML",	"innerHTML", 		"Humidity", 					to_string(last_update));
			WebSocketSendData("HTML",	"innerHTML", 		"Temperature", 					to_string(last_update));
			WebSocketSendData("Form",	"value", 			"Relay0", 						relay.getName() + ' ' + ( relay.status() == true ? "ausschalten" : "einschalten" ));
			WebSocketSendData("Form",	"innerHTML", 		"EffektSpeed", 					to_string(EffektSpeed));
			WebSocketSendData("Form",	"value",			"RGB-Color", 					RGBColor);
			WebSocketSendData("CSS",	"backgroundColor",	"Pir-Sensor", 					Pir_Sensor.getActiveReport() == true ? "red" : "green");
			WebSocketSendData("Form",	"radio",			aktueller_Effekt->getName(),	"true");

			if(force == true) {
				WebSocketSendData("Form",	"checkbox", 	"WiFiAccessPointMode",	WiFiAccessPointMode == true ? "true" : "false");
				WebSocketSendData("Form",	"value", 		"WiFi-Name", 			WiFiName);
				WebSocketSendData("Form",	"value", 		"WiFi-Passwort",		WiFiPassword);
				WebSocketSendData("Form",	"value", 		"Hostname",				Hostname);
				WebSocketSendData("Form",	"value", 		"MaxConnections",		to_string(MaxWiFiCon));
			}
			/*
			WebSocket.broadcastTXT(String("Humidity:" + to_string(last_update)).c_str());
			WebSocket.broadcastTXT(String("Temperature:" + to_string(last_update)).c_str());
			WebSocket.broadcastTXT(String("EffektSpeed:" + to_string(EffektSpeed)).c_str());
			*/
			//Serial.println("Websocket-daten Senden");
			//Serial.println(last_update);
			//Serial.println(EffektSpeed);
			last_update = millis();
		}
	}
}

// !! uint8_t = unsigned char !!
// https://tttapa.github.io/ESP8266/Chap14%20-%20WebSocket.html
void handleWebSocket(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
	switch (type) {
	case WStype_DISCONNECTED: {
		Serial.print(num);
		Serial.println(" Verbindung getrennt");
		break;
	}
	case WStype_CONNECTED: {
		IPAddress ip = WebSocket.remoteIP(num);

		Serial.print("Verbindung hergestellt von ");
		Serial.print(ip.toString());
		Serial.println(reinterpret_cast<char*>(payload));
		Serial.println("Die aktuellen Sensordaten werden gesendet, da neue Verbindung");

		dynamicUpdateClientWebsite(true);
		break;
	}
	case WStype_TEXT: {
		Serial.print(num);
		Serial.print(" Text empfangen: ");
		Serial.println(reinterpret_cast<char*>(payload));

		String req(reinterpret_cast<const char*>(payload));
		String argName, arg;
		argName = req.substring(0, req.indexOf(":"));
		arg = req.substring(req.indexOf(":")+1, req.length());
		make_action(argName, arg);
		break;
	}
	case WStype_ERROR: {
		Serial.println("Ein Fehler ist im WebSocket aufgetreten!");
		break;
	}
	default:
		break;
	}
}

void handleWebServer() {
	// Datei-streamen
	String pathToFile = find_path_to_req_File(webserver.uri());
	if(pathToFile.isEmpty()) {	// Wenn es den vom Client verlangten Pfad nicht gibt eine Fehlermeldung schicken
		webserver.send(404, "text/plain", "404: Seite nicht gefunden");
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
	if(argName == "RGB-Color") {
		for(unsigned short i = 0; i < EffektContainer.size(); i++) {	// Wenn gerade Effekt gelaufen ist, ihn abschalten
			if(EffektContainer[i].getName() == "Nothing") {
				aktueller_Effekt = &EffektContainer[i];
				break;
			}
		}
		for(unsigned short pixel = 0; pixel < RGB_LEDS.numPixels(); pixel++) {
			RGB_LEDS.setPixelColor(pixel, RGBHexToColor(arg.c_str()));
		}
		RGBColor = arg;
		RGB_LEDS.show();
	} else if (argName == "Effekt") {
		for(unsigned short i = 0; i < EffektContainer.size(); i++) {
			if(EffektContainer[i].getName() == arg) {
				aktueller_Effekt = &EffektContainer[i];
				break;
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
	} else if(argName == "Relay") {
		relay.switchStatus();
	} else if(argName == "PirSensor") {
		if(arg == "reset") {
			Pir_Sensor.resetActiveReport();
		}
	}
	// Als letztes wird immer noch plain als arg gegeben - das nutze ich um am Ende die Configs in den Spiffs zu schreiben
	else if (argName == "plain" && arg.indexOf("WiFi-Name") > 0 && arg.indexOf("WiFi-Passwort") && arg.indexOf("MaxConnections") && arg.indexOf("Hostname") > 0) {
		writeConfigs();
		Serial.println("Write-Configs - changed");

	}
	/**
	 * Bei Webserver:
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