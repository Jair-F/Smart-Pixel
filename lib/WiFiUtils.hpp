#ifndef _WIFIUTILS_HPP_INCLUDED_
#define _WIFIUTILS_HPP_INCLUDED_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "Exception.hpp"
#include "Helper.hpp"

extern MDNSResponder MDNS;
extern ESP8266WiFiClass WiFi;
extern bool WiFiAccessPointMode;
extern String WiFiName;
extern String WiFiPassword;
extern unsigned short MaxWiFiCon;

void initWifi();

void initDNS();

/**
 * @param stat WiFi-Status
 * @return Textbasierte Fehlermeldung von WiFi-Status "stat"
 */
String wifiStatusUserOutput(wl_status_t stat);


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

#endif // _WIFIUTILS_HPP_INCLUDED_