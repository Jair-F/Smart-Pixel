# Smart-Pixel

## Pin-Belegung
**Bisher nur Beispiele - die Pins stimmen nur fürs erste - müssen später noch richtig eingetragen werden!!**
- Touch-Sensor: D0
- Pir-Sensor: D1
- Relay-0: D3
- DHT-Sensor: A0
- RGB-LED: D2
- Display:
	* LED: 3.3V
	* SCK: D5
	* SDA: D10
	* A0:  D4
	* RESET: 3.3V
	* CS: D8
- (RTC:)


## Planung
- Datei **"Klassen Planung.io"** ist auf der Plattform [Draw IO](https://app.diagrams.net/) gemacht worden.

## Library's - Voraussetzungen
- [Arduino-Core für ESP8266-WiFiChip](https://github.com/esp8266/Arduino) (Nodemcu Library)
	* [Installationsanweisung auf der Seite folgen](https://github.com/esp8266/Arduino#installing-with-boards-manager)
		* Vereinfacht erklärt: Füge `http://arduino.esp8266.com/stable/package_esp8266com_index.json` in der Arduino IDE unter *Datei* > *Voreinstellungen* > *Zusätzliche Boardverwalter-URLs*
		* *Werkzeuge* > *Board* > *Boardverwalter* > suche nach *'esp8266'* > Klicke auf Installieren
- [Websocket](https://github.com/Links2004/arduinoWebSockets)
	* Unter [releases](https://github.com/Links2004/arduinoWebSockets/releases) die neuste 'SourceCode.zip' herunterladen
	* Arduino-IDE: (*Sketch* > *Bibliothek einbinden* > *Zip Bibliothek hinzufügen*)
- [Adafruit Neopixel](https://github.com/adafruit/Adafruit_NeoPixel) 
	* [Installationsanweisungen auf der Seite folgen](https://github.com/adafruit/Adafruit_NeoPixel#installation)
- Display
	* In Library Manager nach **ST7735** suchen, die Bibliothek von Adafruit.
- RTC
	* In Library Manager nach **RTClib** suchen, die Bibliothek von Adafruit.
- DHT-Sensor
	* In Library Manager nach **dht sensor library** suchen, die Bibliothek von Adafruit.
- [SPIFFS-Uploader](https://github.com/esp8266/Arduino#installing-with-boards-manager) - **Das ist nur der File-Uploader für die Arduino IDE. Die Library für das ESP8266-Spiffs ist schon in der 'Arduino-Core für ESP8266-WiFi-Chip' Library enthalten.**
	* [Installationsanweisung auf der Seite folgen](https://github.com/esp8266/arduino-esp8266fs-plugin#installation)
	* Unter Linux muss man im Home-Verzeichnis unter Arduino(~/Arduino) den Ordner **tools** erstellen. Da muss man den Inhalt der heruntergeladenen Datei entpaken.
