#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <exception>
#include <stdexcept>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>
//#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>		// Include core graphics library
#include <Adafruit_ST7735.h>	// Include Adafruit_ST7735 library to drive the display

#define DEBUG

#include "lib/Filesystem.hpp"
#include "lib/config/ConfigFile.hpp"

/**
 * Definiert ob man ein WiFi Access Point erstellen soll oder sich zu einem bestehende WiFi verbinden soll
 * true  = ein eignen WiFi Access Point erstellen
 * false = mit einem bestehenden WiFi verbinden
 */
bool WiFiAccessPointMode = true;
String Hostname; // Lokale Domain
String WiFiName;
String WiFiPassword;

/* Standard IP During programming WEB Server Mode */
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

/**
 * Maximale Anzahl an Clients, die sich mit dem WiFi verbinden koennen.
 * Ist nur wichtig, wenn ein WiFi-Access-Point erstellet wird(WiFiAccessPointMode = true).
 */
unsigned short MaxWiFiCon;

ESP8266WebServer webserver(WiFi.localIP(), 80);
// https://github.com/Links2004/arduinoWebSockets
WebSocketsServer WebSocket(81);
#define DYNAMIC_WEBSITE_UPDATE_INTERVAL 5000 // 1s = 1000(ms)

//ESP8266WiFiClass WiFi;

//MDNSResponder MDNS;

#define TFT_CS		15	// D8
#define TFT_RST		0	// goes to ground
#define TFT_DC		2	// DC = A0 - D4

Adafruit_ST7735 display(TFT_CS, TFT_DC, TFT_RST);

#define RGB_LED_NUMPIXELS 16
#define RGB_LED_PIN D2
Adafruit_NeoPixel RGB_LEDS(RGB_LED_NUMPIXELS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);
String RGBColor;

#define DHT_PIN D0
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

#include "lib/PirSensor.hpp"
#include "lib/Relay.hpp"
Relay relay;
PirSensor Pir_Sensor(D1);

Filesystem filesystem;
ConfigFile config(filesystem);

#include "lib/Exception.hpp"
#include "lib/WiFiUtils.hpp"
#include "lib/PirSensor.hpp"
#include "lib/TouchSensor.hpp"
#include "lib/RGBRing.hpp"
#include "lib/Display.hpp"

void setup() {
	Serial.begin(9600);
	Serial.println("Serial started");

	display.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
	Serial.println("Display started");
	
	display.fillScreen(ST7735_BLACK);
	display.setRotation(1);
	display.setTextWrap(true);
	const char* name = "Smart-Pixel";
	display.setTextSize(2);
	display.setCursor( (display.width() - strlen(name)*12) / 2, display.height() / 2 - 16/2);
	for(unsigned short i = 0; i < strlen(name); i++) {
		display.setTextColor(display.color565(i%2 != 0 && i %3 != 0 ? 255 : 0, i%2 == 0 ? 255 : 0, i%3 == 0 ? 255 : 0));
		display.print(name[i]);
		delay(70);
	}
	delay(1000);
	//display.fillScreen(INITR_BLACKTAB);

	unsigned short abstand_an_den_raendern = 10;

	display.drawFastHLine(abstand_an_den_raendern, 100, display.width() - abstand_an_den_raendern * 2, display.color565(255, 0, 0));
	display.drawFastHLine(abstand_an_den_raendern, 105, display.width() - abstand_an_den_raendern * 2, display.color565(255, 0, 0));
	display.drawFastVLine(abstand_an_den_raendern, 100, 6, display.color565(255, 0, 0));
	display.drawFastVLine(display.width() - 1 - abstand_an_den_raendern, 100, 6, display.color565(255, 0, 0));

	for(unsigned int i = 0; i < 127; i++) {
			display.drawFastHLine(12, 102, 10 + i, display.color565(255, 0, 0));
			display.drawFastHLine(12, 103, 10 + i, display.color565(255, 0, 0));
		delay(10);
	}

	config.setPath("/config.config");
	config.readConfigFile();
	Serial.println("Configs read");

	// Setting up Envirement variables
	try {
		WiFiName = config["WiFi"]["WiFiName"];
		WiFiPassword = config["WiFi"]["WiFiPassword"];

		String tmp = config["WiFi"]["WiFiAccessPointMode"];
		if(tmp == "true" || tmp == "1") {
			WiFiAccessPointMode == true;
		} else if(tmp == "false" || tmp == "0") {
			WiFiAccessPointMode == false;
		}
		Hostname = config["Server"]["Hostname"];

	}
	catch(config_error& ce) {
		Serial.print("Config_ERROR: ");
		Serial.println(ce.what());
	}
	catch(filesystem_error& fe) {
		Serial.print("Filesystem_ERROR: ");
		Serial.println(fe.what());
	}
	catch(std::exception& exc) {
		Serial.print("Exception: ");
		Serial.println(exc.what());
	}

	#ifdef DEBUG
		Serial.println(config.print());
	#endif // DEBUG


	initWifi();
	Serial.println("WiFi started");
	Serial.print("IP-Adresse: ");
	Serial.println(WiFi.localIP().toString());

	initDNS();
	Serial.println("DNS started");

	initWebServer();
	Serial.println("Web-Server started");

	initWebSockets();
	Serial.println("Web-Sockets started");

	RGB_LEDS.begin();
	Serial.println("RGB-LEDS started");

	for(int i = 0; i < RGB_LEDS.numPixels(); i++) {
		RGB_LEDS.setPixelColor(i, Adafruit_NeoPixel::Color(255, 0, 0));
	}
	RGB_LEDS.show();
	delay(500);
	for(int i = 0; i < RGB_LEDS.numPixels(); i++) {
		RGB_LEDS.setPixelColor(i, Adafruit_NeoPixel::Color(0, 255, 0));
	}
	RGB_LEDS.show();
	delay(500);
	for(int i = 0; i < RGB_LEDS.numPixels(); i++) {
		RGB_LEDS.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 255));
	}
	RGB_LEDS.show();
	delay(500);
	for(int i = 0; i < RGB_LEDS.numPixels(); i++) {
		RGB_LEDS.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
	}
	RGB_LEDS.show();

	Serial.println("DHT started");
	dht.begin();
	Serial.print("Humidity: ");
	Serial.println(dht.readHumidity());
	Serial.print("Temp: ");
	Serial.println(dht.readTemperature());

	EffektContainer.push_back(Effekt("Blink", rainbow_soft_blink));
	EffektContainer.push_back(Effekt("RainbowCycle", rainbowCycle));
	EffektContainer.push_back(Effekt("ColorWipe", colorWipe));	
	EffektContainer.push_back(Effekt("Nothing", Nothing));
	for(unsigned short i = 0; i < EffektContainer.size(); i++) {
		if(EffektContainer[i].getName() == "Nothing") {
			aktueller_Effekt = &EffektContainer[i];
			break;
		}
	}

	relay.setPin(D3);
	relay.setName("LED");

}

void loop() {
	/*
	display.fillScreen(ST7735_BLACK);
	display.setCursor(0,0);
	display.print(dht.readTemperature());
	delay(1000);
	*/
	static unsigned long timer = 0;
	if(timer < millis()) {
		display.setTextWrap(false);
		display.fillScreen(ST7735_BLACK);
		display.setCursor(0,0 );
		display.setTextSize(2);
		display.setTextColor(ST7735_YELLOW);
		display.println("WiFi-Settings");

		display.drawFastHLine(0, display.getCursorY() + 3, display.width(), ST7735_WHITE);

		display.setTextSize(1);
		display.println();

		display.setTextColor(ST77XX_CYAN);
		display.println("Name: ");
		display.setTextColor(ST7735_WHITE);
		display.println(WiFiName);

		display.println();

		display.setTextColor(ST77XX_CYAN);
		display.println("Passwort: ");
		display.setTextColor(ST7735_WHITE);
		display.println(WiFiPassword);

		display.println();

		display.setTextColor(ST77XX_CYAN);
		display.println("Hostname: ");
		display.setTextColor(ST7735_WHITE);
		display.println(Hostname + ".local");

		display.println();

		display.setTextColor(ST77XX_CYAN);
		display.println("Lokale IP: ");
		display.setTextColor(ST7735_WHITE);
		display.println(WiFi.localIP().toString());

		display.println();

		display.setTextColor(ST77XX_CYAN);
		display.print("Verbundene Clients: ");
		display.setTextColor(ST7735_WHITE);
		display.println(WebSocket.connectedClients());
		timer = millis() + 1500;
	}

	run_Effekt();
	WebSocket.loop();
	yield();
	webserver.handleClient();
	MDNS.update();
	yield();
	Pir_Sensor.check();
}
