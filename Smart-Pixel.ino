#include <Arduino.h>
#include <WebSocketsServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <exception>
#include <stdexcept>
#include <DHT.h>
#include <Adafruit_GFX.h>		// Include core graphics library
#include <Adafruit_ST7735.h>	// Include Adafruit_ST7735 library to drive the display

#define DEBUG

#include "lib/Filesystem.hpp"
#include "lib/config/ConfigFile.hpp"

#include "lib/Webserver.hpp"
#include "lib/Websocket/Websocket.hpp"

#include "lib/LED/RGB_LED.hpp"
#include "lib/LED/Effects.hpp"
#include "lib/Effect_Functions.hpp"
#include "lib/LED/RGB_Utils.hpp"

#include "lib/TouchSensor/TouchSensor.hpp"
#include "lib/PirSensor.hpp"
//#include "lib/Display.hpp"
#include "lib/Relay.hpp"

#include "lib/GlobalConstants.hpp"
#include "lib/Exception.hpp"

/**
 * Definiert ob man ein WiFi Access Point erstellen soll oder sich zu einem bestehende WiFi verbinden soll
 * true  = ein eignen WiFi Access Point erstellen
 * false = mit einem bestehenden WiFi verbinden
 */
bool WiFiAccessPointMode = true;
String Hostname; // Lokale Domain
String WiFiName;
String WiFiPassword;
/**
 * Maximale Anzahl an Clients, die sich mit dem WiFi verbinden koennen.
 * Ist nur wichtig, wenn ein WiFi-Access-Point erstellet wird(WiFiAccessPointMode = true).
 */
unsigned short MaxWiFiCon;

#include "lib/WiFiUtils.hpp"

/* Standard IP During programming WEB Server Mode
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
*/


Filesystem filesystem;
ConfigFile config(filesystem);


Adafruit_ST7735 display(TFT_CS, TFT_DC, TFT_RST);


String RGBColor;
unsigned short EffektSpeed = 10;
EffectGroup Effects;
RGB_LED RGB_LEDS(RGB_LED_NUMPIXELS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);


DHT dht(DHT_PIN, DHT_TYPE);

PirSensor Pir_Sensor(PIR_SENSOR_PIN);

Relay relay(RELAY_PIN, "LED");

TouchSensor Touch_Sensor(TOUCH_SENSOR_PIN);


//ESP8266WiFiClass WiFi;

//MDNSResponder MDNS;

Webserver webserver(filesystem, 80);

Websocket websocket(81);




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

	try {
		if(! filesystem.begin()) {
			throw Filesystem_error("Failed to start Filesystem");
		} else {
			Serial.println("Filesystem started");
		}

		config.setPath("/config.config");
		config.readConfigFile();
	}
	catch(Filesystem_error& fe) {
		Serial.println("Filesystem ERROR: ");
		Serial.println(fe.what());
		Serial.println("Aborting...");
		exit(-1);
	}
	catch(std::exception& exc) {
		Serial.println("ERROR: ");
		Serial.println(exc.what());
		Serial.println("Aborting...");
	}
	catch(...) {
		Serial.println("Unknown ERROR");
		Serial.println("Aborting...");
	}

	#ifdef DEBUG
		Serial.println(config.print());
	#endif // DEBUG
	Serial.println("Configs read");

	// Setting up Envirement variables
	try {
		WiFiName = config[WIFI][WIFI_NAME].get_Value();
		WiFiPassword = config[WIFI][WIFI_PASSWORD].get_Value();

		WiFiAccessPointMode = to_bool(config[WIFI][WIFI_ACCESSPOINT_MODE].get_Value());
		Hostname = config[SERVER][HOSTNAME].get_Value();
		MaxWiFiCon =  config[WIFI][MAX_CONNECTIONS].get_Value().toInt();

	}
	catch(Config_error& ce) {
		Serial.print("Config_ERROR: ");
		Serial.println(ce.what());
	}
	catch(Filesystem_error& fe) {
		Serial.print("Filesystem_ERROR: ");
		Serial.println(fe.what());
	}
	catch(std::exception& exc) {
		Serial.print("ERROR: ");
		Serial.println(exc.what());
	}


	initWifi();
	Serial.println("WiFi started");
	Serial.print("IP-Adresse: ");
	Serial.println(WiFi.localIP().toString());

	initDNS();
	Serial.println("DNS started");

	webserver.setWorkingDir("/www");
	webserver.begin();
	Serial.println("Web-Server started");

	websocket.set_seperator(':');

	// Actions the Websocket does if a new client connects to him
	websocket.set_onConnectHandler([&](uint8_t ClientNum){
		websocket.sendTXT(ClientNum, RGB_COLOR,					RGB_Utils::RGBColorToHex(RGB_LEDS.getPixelColor(0)));
		websocket.sendTXT(ClientNum, EFFECT_RUNNING,			to_string(RGB_LEDS.get_effectRunning()));
		websocket.sendTXT(ClientNum, EFFECT,					RGB_LEDS.get_effectRunning() == true ? RGB_LEDS.getActualEffekt().getName() : "Nothing");
		websocket.sendTXT(ClientNum, EFFECT_SPEED,				to_string(EffektSpeed));
		websocket.sendTXT(ClientNum, TEMPERATURE,				to_string(dht.readTemperature()));
		websocket.sendTXT(ClientNum, HUMIDITY, 					to_string(dht.readHumidity()));
		websocket.sendTXT(ClientNum, RELAY_NAME,				relay.getName());
		websocket.sendTXT(ClientNum, RELAY_STATUS,				to_string(relay.status()));
		websocket.sendTXT(ClientNum, PIR_SENSOR_ALERT_REPORTED,	to_string(Pir_Sensor.alertReported()));
		websocket.sendTXT(ClientNum, WIFI_ACCESSPOINT_MODE,		to_string(WiFiAccessPointMode));
		websocket.sendTXT(ClientNum, WIFI_NAME,					WiFiName);
		websocket.sendTXT(ClientNum, WIFI_PASSWORD,				WiFiPassword);
		websocket.sendTXT(ClientNum, HOSTNAME,					Hostname);
		websocket.sendTXT(ClientNum, MAX_CONNECTIONS,			to_string(MaxWiFiCon));
		// Send to all Clients
		websocket.broadcastTXT(NUM_OF_CONNECTED_CLIENTS,		to_string(static_cast<unsigned short>(websocket.connectedClients())));
	});
	// Actions the Websocket does if a  client disconnects
	websocket.set_onDisconnectHandler([&](uint8_t ClientNum){
		websocket.broadcastTXT(NUM_OF_CONNECTED_CLIENTS,		to_string(static_cast<unsigned short>(websocket.connectedClients())));
	});

	// Actions if the Websocket recievs Data(The right method is selected by the keyValue from the WebsocketAction)
	websocket.addAction(WebsocketAction(RGB_COLOR,							[&RGB_LEDS](String& arguments)				{ RGB_LEDS.fill(RGB_Utils::RGBHexToColor(arguments)); RGB_LEDS.show(); websocket.broadcastTXT(EFFECT_RUNNING, to_string(RGB_LEDS.get_effectRunning())); websocket.broadcastTXT(RGB_COLOR, arguments); }));
	websocket.addAction(WebsocketAction(EFFECT,								[&Effects, &RGB_LEDS](String& arguments)	{ RGB_LEDS.setActualEffekt(Effects[arguments]); websocket.broadcastTXT(EFFECT, arguments); }));
	websocket.addAction(WebsocketAction(EFFECT_RUNNING,						[&RGB_LEDS](String& arguments)				{ RGB_LEDS.set_effectRunning(to_bool(arguments)); websocket.broadcastTXT(EFFECT_RUNNING, to_string(RGB_LEDS.get_effectRunning())); }));
	websocket.addAction(WebsocketAction(EFFECT_SPEED,						[&EffektSpeed](String& arguments)			{ EffektSpeed = arguments.toInt(); websocket.broadcastTXT(EFFECT_SPEED, arguments); }));

	websocket.addAction(WebsocketAction(RELAY_STATUS,						[&relay](String& arguments)					{ relay.switchStatus(); websocket.broadcastTXT(RELAY_STATUS, to_string(relay.status())); }));
	websocket.addAction(WebsocketAction(RESET_PIR_SENSOR_ALERT_REPORTED,	[&Pir_Sensor](String& arguments)			{ Pir_Sensor.reset_reportedAlert(); websocket.broadcastTXT(PIR_SENSOR_ALERT_REPORTED, to_string(Pir_Sensor.alertReported())); }));
	websocket.addAction(WebsocketAction(REBOOT,								[](String& arguments)						{ ESP.restart(); websocket.broadcastTXT(CLIENT_ALERT, "Board restarted -- Need to reload site after restart!"); }));
	websocket.addAction(WebsocketAction(COMMAND,							[&config](String& arguments)				{
		if(arguments == WRITE_CONFIG) {
			config.writeConfigFile(); 
			websocket.broadcastTXT(CLIENT_ALERT, "Wrote Configs -- Reboot to take affect the changes!");
		}
	}));

	websocket.addAction(WebsocketAction(WIFI_NAME,							[&config](String& arguments)				{ config[WIFI][WIFI_NAME].set_Value(arguments);				WiFiName = arguments;							websocket.broadcastTXT(WIFI_NAME, arguments); }));
	websocket.addAction(WebsocketAction(WIFI_PASSWORD,						[&config](String& arguments)				{ config[WIFI][WIFI_PASSWORD].set_Value(arguments);			WiFiPassword = arguments;						websocket.broadcastTXT(WIFI_PASSWORD, arguments); }));
	websocket.addAction(WebsocketAction(MAX_CONNECTIONS,					[&config](String& arguments)				{ config[WIFI][MAX_CONNECTIONS].set_Value(arguments);		MaxWiFiCon = arguments.toInt();					websocket.broadcastTXT(MAX_CONNECTIONS, arguments); }));
	websocket.addAction(WebsocketAction(WIFI_ACCESSPOINT_MODE,				[&config](String& arguments)				{ config[WIFI][WIFI_ACCESSPOINT_MODE].set_Value(arguments);	WiFiAccessPointMode = arguments.toInt();		websocket.broadcastTXT(WIFI_ACCESSPOINT_MODE, arguments); }));
	websocket.addAction(WebsocketAction(HOSTNAME,							[&config](String& arguments)				{ config[SERVER][HOSTNAME].set_Value(arguments);			Hostname = arguments;							websocket.broadcastTXT(HOSTNAME, arguments); }));

	websocket.begin();
	Serial.println("Web-Sockets started");


	Serial.println("DHT started");
	dht.begin();
	Serial.print("Humidity: ");
	Serial.println(dht.readHumidity());
	Serial.print("Temp: ");
	Serial.println(dht.readTemperature());

	// Initialising the effectsGroup - until now only a example - need to rewrite the effects
	Effects.add(Effect(RAINBOW_SOFT_BLINK,	rainbow_soft_blink));
	Effects.add(Effect(COLOR_WIPE,			colorWipe));
	Effects.add(Effect(RAINBOW_CYCLE,		rainbowCycle));
	RGB_LEDS.setActualEffekt(Effects[RAINBOW_SOFT_BLINK]);


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
		display.println(websocket.connectedClients());
		timer = millis() + 1500;
	}

	/*
		Actions the Websocket does if something changed(on the Board) and the Websocket acts with the Clients
		without got previously data from the Client/s
	*/

	// Touch Sensor Actions
	if(Touch_Sensor.touched(4, TimeType::seconds)) {	// Reset - restor to default settings and then restart
		Serial.println("Reset...");
		

		config[WIFI][WIFI_NAME].set_Value("Smart-Pixel");
		config[WIFI][WIFI_PASSWORD].set_Value("12345678");
		config[WIFI][WIFI_ACCESSPOINT_MODE].set_Value("true");
		config[WIFI][MAX_CONNECTIONS].set_Value("1");
		config[SERVER][HOSTNAME].set_Value("smart-pixel");

		config.writeConfigFile();
		

		Serial.println("Restart...");
		ESP.restart();
	}
	else if(Touch_Sensor.touched(2, TimeType::seconds)) {	// Restart
		Serial.println("Restart...");
		ESP.restart();
	}
	else if(Touch_Sensor.touched(1, TimeType::seconds)) {
		relay.switchStatus();
		websocket.broadcastTXT(RELAY_STATUS, to_string(relay.status()));
		delay(500);
	}

	RGB_LEDS(EffektSpeed);
	Touch_Sensor.loop();
	websocket.loop();
	yield();
	webserver.handleClient();
	MDNS.update();
	yield();
	Pir_Sensor.check();
}
