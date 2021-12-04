# Smart-Pixel

## Description
This Code is for the [Arduino NodeMCU-Microcontroller](https://en.wikipedia.org/wiki/NodeMCU) (preferrably for the model NodeMCU 1). The following periphery is present:
- A WiFi-Chip on the NodeMCU-Board - to connect to a WiFi or create an own WiFi-Access-Point
- A [Touch-Sensor](https://www.dx.com/p/produino-jog-type-touch-sensor-capacitive-touch-switch-module-for-arduino-blue-2038545.html) to controll the Board pysically (restart, reset)
- A temperature and humidity sensor ([DHT11](https://learn.adafruit.com/dht))
- A [motion detector](https://www.elecrow.com/hcsr505-mini-pir-motion-sensor-p-1382.html)(to give you a report if somebody is actually in your room)
- A [Relay](https://www.amazon.com/-/de/dp/B07BVXT1ZK/) to control lamps or other devices which have to be connected to high voltage(220V)
- A small [Display](https://www.amazon.com/Display-Module-ST7735-128x160-STM32/dp/B07BFV69DZ) to print important information
- A [Neopixel RGB-Ring/Block/Strip](https://www.adafruit.com/product/1463) to run effects or show a color

All these elements are controllable through a web-interface (Website with Websocket) to read the temperature/humidity, to control the Relay/the LED's or run an effect... You can also assign to the relay and to the Motion detector a name, which will be shown on the website accordingly. In order to ensure a smooth and user friendly experience, the website updates itself dynamically in the background, reading constantly the actual data from the sensors connected to the Board.

**The BackEnd is written entirely in C++. The Webinterface/Website is written in HTML, styled with CSS, the dynamic updates of the Website are written in JavaScript**

Here is a demonstration how the clients are syncronized with each other. If a parameter is changed at one client, the other client is updated directly!

[![Project Demo](https://img.youtube.com/vi/37696_5yYVo/0.jpg)](https://www.youtube.com/watch?v=37696_5yYVo)

## For Developers
Due to the limitations of the Board like **limited porcessing power and limited storage we dont have the ability to use multithreading** the functions or loops (which need to run continuosly in the background) are designed in a different way. For example, to start the effects we cant run in an own thread. We coded the functions of the effect (and also of the other periphery and objects which need an own loop) in such a way that it stores the previous color, changes the color when calling the function only one step and returns immediately. All these loop functions are called from the main loop.

### Used Languages
- C++ (Server BackEnd, peripheral control)
- HTML, CSS (for the Website)
- JavaScript (to update the Website dynamically)

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
	* SDA: D7
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

## Uploading the Sketch
**This are my settings and they should work properly**
- *Tools* > *Board* - select the NodeMCU 1.0 board
- *Tools* > *C++ Exceptions* - change it to **enable**
- *Tools* > *Flash Size* - select **4MB (FS:2MB OTA:~1019KB)**
- *Tools* > *Port* - connect your NodeMCU to your PC and select the port of your Board

Then upload the sketch by pressing **Ctrl+U** or clicking the arrow which points to the right at the left upper corner.
