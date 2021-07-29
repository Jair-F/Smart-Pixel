# Smart-Pixel

## Pins
**The pins are not finally. During development they may change!!**
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


## Planning
- File **"Klassen Planung.io"** is made on the Platform/Website [Draw IO](https://app.diagrams.net/).

## Library's 
- [Arduino-Core für ESP8266-WiFiChip](https://github.com/esp8266/Arduino) (Nodemcu Library)
	* [Installationsanweisung auf der Seite folgen](https://github.com/esp8266/Arduino#installing-with-boards-manager)
		* Insert this Link `http://arduino.esp8266.com/stable/package_esp8266com_index.json` in the Arduino-IDE *File* > *Preferences* > *Additional Boards Manager URLs*
		* *Tools* > *Board* > *BoardManager* > search for *'esp8266'* > click install
- [Websocket](https://github.com/Links2004/arduinoWebSockets)
	* Under [releases](https://github.com/Links2004/arduinoWebSockets/releases) downlaod the newest 'SourceCode.zip'
	* Arduino-IDE: (*Sketch* > *Include Library* > *Add .zip Library*)
- [Adafruit Neopixel](https://github.com/adafruit/Adafruit_NeoPixel) 
	* [Follow installation instructions on the site](https://github.com/adafruit/Adafruit_NeoPixel#installation)
- Display
	* Search in Library Manager(*Sketch* > *Include Library* > *Manage Libraries*) for **ST7735**, the Library from Adafruit.
- RTC
	* Search in Library Manager for **RTClib**, the Library from Adafruit.
- DHT-Sensor
	* Search in Library Manager for **dht sensor library**, the Library from Adafruit.
- [SPIFFS-Uploader](https://github.com/esp8266/Arduino#installing-with-boards-manager) - **This is only the SPIFFS-File uploader(data directory) for the Arduino-IDE. The Library for the ESP8255-Spiffs is already in included in the 'Arduino-Core für ESP8266-WiFi-Chip' library.**
	* [Follow installation instructions on the site](https://github.com/esp8266/arduino-esp8266fs-plugin#installation)
	* On Linux you have to create in your Home-Folder in the Arduino-Directory(~/Arduino) a folder called **tools**. Inside this folder you have to extract the content of the downloaded archive.
