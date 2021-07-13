#pragma once



// Config-Keywords
#define WIFI_NAME				"WiFiName"
#define WIFI_PASSWORD			"WiFiPassword"
#define MAX_CONNECTIONS			"MaxConnections"
#define WIFI_ACCESSPOINT_MODE	"WiFiAccessPointMode"
#define HOSTNAME				"Hostname"

// Websocket Acions
#define RGB_COLOR					"RGB-Color"
#define EFFECT						"Effect"
#define EFFECT_RUNNING				"EffektRunning"
#define EFFECT_SPEED				"EffectSpeed"
#define HUMIDITY					"Humidity"
#define TEMPERATURE					"Temperature"
#define RELAY_STATUS				"RelayStatus"
#define RELAY_NAME					"RelayName"
#define WRITE_CONFIG				"write-config"
#define PIR_SENSOR_ACTIVE_REPORT	"PirSensorActiveReport"
// Reboot the board
#define REBOOT						"reboot"
// Send Command to client to show him a message(pop up window)
#define CLIENT_ALERT				"ClientAlert"
#define NUM_OF_CONNECTED_CLIENTS	"NumOfConnectedClients"

// conifg-Groups
#define WIFI	"WiFi"
#define SERVER	"Server"

// Effects
#define COLOR_WIPE			"ColorWipe"
#define RAINBOW_SOFT_BLINK	"rainbowSoftBlink"
#define RAINBOW_CYCLE		"RainbowCycle"


// Display
/*
Pinouts:
VCC			3.3V
GND			GND
CS			D8
Reset		3.3v
A0 or D/C	D4
SDA (MOSI)	D7 
SCK			D5 
LED			3.3V
*/

#define TFT_CS		D8	//15	// D8
#define TFT_RST		0	// goes to ground
#define TFT_DC		D4	//2	// DC = A0 - D4
// Display

// RGB-LED
#define RGB_LED_NUMPIXELS 16
#define RGB_LED_PIN D2
// RGB-LED

// DHT-Temperature-Humidity-Sensor
#define DHT_PIN D0
#define DHT_TYPE DHT11
// DHT-Temperature-Humidity-Sensor

// Pir-Sensor
#define PIR_SENSOR_PIN D1
// Pir-Sensor

// Relay
#define RELAY_PIN D3
// Relay
