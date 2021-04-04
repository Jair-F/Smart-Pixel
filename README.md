# Smart-Pixel

## Pin-Belegung
- Touch-Sensor: D1
- Pir-Sensor: D2
- Relay-0: D3
- DHT-Sensor: D4
- Display:
	* LED: D8
	* SCK: D9
	* SDA: D10
	* A0:  D11
	* RESET: D12
	* CS: D13
- (RTC:)


## Library's - Voraussetzungen
- Arduino-Core für ESP8266ßWiFi-Chip (Nodemcu Library)
	* [Hier ihr Github](https://github.com/esp8266/Arduino) Installationsanweisung auf der Seite folgen ([Installing with Boards Manager](https://github.com/esp8266/Arduino#installing-with-boards-manager))
		* Vereinfacht erklärt: Füge [diesen Link](http://arduino.esp8266.com/stable/package_esp8266com_index.json) in der Arduino IDE unter *Datei* > *Voreinstellungen* > *Zusätzliche Boardverwalter-URLs*
		* *Werkzeuge* > *Board* > *Boardverwalter* > suche nach *'esp8266'* > Klicke auf Installieren
- [Websocket](https://github.com/Links2004/arduinoWebSockets)
	* Auf releases gehen(rechts) und neuste 'SourceCode.zip' herunterladen
	* Arduino-IDE: (*Sketch* > *Bibliothek einbinden* > *Zip Bibliothek hinzufügen*)
- Adafruit Neopixel - Arduino Library-Manager (*Sketch** > *Bibliothek einbinden* > *Bibliotheken verwalten*)
- Display
- RTC
- SPIFFS-Uploader - **Das ist nur der File-Uploader für die Arduino IDE. Die Library für das ESP8266 ist schon in der 'Arduino-Core für ESP8266-WiFi-Chip' Library enthalten.**
	* Arduino-ESP8266FS-Plugin [Github-Website](https://github.com/esp8266/arduino-esp8266fs-plugin)
	* Installationsanweisung auf der Seite folgen

