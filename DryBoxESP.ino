#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ESP8266httpUpdate.h"
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "DHT.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define DHTINTERNAL 2 // Pin D4 for external humidity sensor
#define DHTEXTERNAL 0 // Pin D3 for internal humitidy sensor
#define DHTTYPE DHT22 // Use DHT22 in the project


#define LOGO_HEIGHT   64 // Height of the logo
#define LOGO_WIDTH    128 // Width of the  logo
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// logo generated with LCD Assistant (https://randomnerdtutorials.com/guide-for-oled-display-with-arduino/)
static const unsigned char PROGMEM logo_bmp[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x07, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0F, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0F, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0F, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0F, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0F, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0F, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1F, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1F, 0xFF, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xE0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1F, 0xFF, 0x00, 0x01, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1F, 0xFF, 0x00, 0x01, 0x80, 0x18, 0x00, 0x00, 0x00, 0x30, 0x06, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1F, 0xFE, 0x00, 0x01, 0x80, 0x0C, 0x00, 0x00, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1F, 0xFE, 0x00, 0x01, 0x80, 0x04, 0x00, 0x00, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1F, 0xFE, 0x00, 0x01, 0x80, 0x02, 0x19, 0x88, 0x04, 0x30, 0x03, 0x00, 0xE0, 0x20, 0x10,
  0x00, 0x1F, 0xFF, 0xFF, 0xC1, 0x80, 0x02, 0x1B, 0x88, 0x04, 0x30, 0x02, 0x03, 0xF8, 0x10, 0x10,
  0x00, 0x3F, 0xFF, 0xFF, 0xC1, 0x80, 0x02, 0x1C, 0x08, 0x04, 0x30, 0x02, 0x04, 0x04, 0x10, 0x20,
  0x00, 0x3F, 0xFF, 0xFF, 0xC1, 0x80, 0x03, 0x18, 0x04, 0x08, 0x30, 0x06, 0x0C, 0x06, 0x08, 0x40,
  0x00, 0x3F, 0xFF, 0xFF, 0x81, 0x80, 0x03, 0x18, 0x04, 0x08, 0x3F, 0xFC, 0x08, 0x02, 0x04, 0x40,
  0x00, 0x3F, 0xFF, 0xFF, 0x81, 0x80, 0x03, 0x18, 0x04, 0x08, 0x3F, 0xFC, 0x08, 0x02, 0x04, 0x80,
  0x00, 0x3F, 0xFF, 0xFF, 0x01, 0x80, 0x03, 0x18, 0x02, 0x10, 0x30, 0x02, 0x18, 0x03, 0x03, 0x00,
  0x00, 0x3F, 0xFF, 0xFF, 0x01, 0x80, 0x02, 0x18, 0x02, 0x10, 0x30, 0x01, 0x18, 0x03, 0x03, 0x00,
  0x00, 0x3F, 0xFF, 0xFE, 0x01, 0x80, 0x02, 0x18, 0x02, 0x10, 0x30, 0x01, 0x18, 0x03, 0x03, 0x00,
  0x00, 0x3F, 0xFF, 0xFC, 0x01, 0x80, 0x02, 0x18, 0x01, 0x20, 0x30, 0x01, 0x88, 0x02, 0x04, 0x80,
  0x00, 0x00, 0x07, 0xFC, 0x01, 0x80, 0x04, 0x18, 0x01, 0x20, 0x30, 0x01, 0x08, 0x02, 0x04, 0x80,
  0x00, 0x00, 0x07, 0xF8, 0x01, 0x80, 0x0C, 0x18, 0x01, 0x20, 0x30, 0x01, 0x0C, 0x06, 0x08, 0x40,
  0x00, 0x00, 0x07, 0xF8, 0x01, 0x80, 0x18, 0x18, 0x00, 0xC0, 0x30, 0x02, 0x04, 0x04, 0x10, 0x20,
  0x00, 0x00, 0x07, 0xF0, 0x01, 0xFF, 0xE0, 0x18, 0x00, 0xC0, 0x3F, 0xFC, 0x03, 0xF8, 0x10, 0x20,
  0x00, 0x00, 0x07, 0xF0, 0x01, 0xFF, 0x00, 0x18, 0x00, 0xC0, 0x3F, 0xE0, 0x00, 0xC0, 0x20, 0x10,
  0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//Variables
int statusCode;
const char* ssid; // SSID, do not enter ip manualy, see README.MD to learn how to select the wifi network
const char* password; // Same as SSID
String ssidListString;
String htmlContent;
const char* version = "1.8"; // Increase the firmware version to ensure that the OTA works properly. (Don't forget to also update the firmware version in the server.
float internalHumidity = 0.0f; // default internal humidity is 0
float externalHumidity = 0.0f; // default external humidity is 0
char* serverName = "simonroy.pythonanywhere.com"; // You can either use the server here or create your own server from the source code on github
float humidityOffset = 0.0f; // The default offset is 0. You can use the "calibrate" button on the website to set an offset. The offset is not store in the eeprom.
bool otaUpdate = false;
int KeepAliveLed = 16; // Keep alive LED.
unsigned long currentMillis;
unsigned long startMillisMeasurement;
unsigned long startMillisUpdate;
unsigned long startMillisSendData;
const unsigned long periodMeasurement = 1000; // 1 second
const unsigned long periodUpdate = 1000 * 60; // 1 min
const unsigned long periodSendData = 1000 * 60 * 60; // 1 hour
boolean networkPortActivate = false; //Set this to true if you want to have the ability to push a new version over your own network (network port).


//Function Declaration
bool testWifi(void);
void launchWeb(void);
void createWebServer(void);
void setupAP(void);
void scanNetwork(void);
void sendHumidity(float internal, float external);
void drawHumidityScreen(void);
void drawSplashScreen(void);
void drawUpdateScreen(void);
void drawHeader(void);
void checkUpdate(void);
void calibrate(void);

// Establishing Local server at port 80
ESP8266WebServer server(80);

// Create humidity sensor object
DHT dhtInternal(DHTINTERNAL, DHTTYPE);
DHT dhtExternal(DHTEXTERNAL, DHTTYPE);

void setup() {
  pinMode(KeepAliveLed, OUTPUT);
  randomSeed(analogRead(0));
  WiFi.disconnect();

  Serial.begin(115200);
  delay(10);

  dhtInternal.begin();
  delay(10);

  dhtExternal.begin();
  delay(10);

  EEPROM.begin(512); //Initializing EEPROM
  delay(10);

  Serial.println("Booting ...");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay(); // Clear the display
  drawSplashScreen(); // Draw the logo

  // Read eeprom for ssid and password
  Serial.println("Reading EEPROM ssid");

  // the wifi ssid is on the first 32 bytes of the EEPROM
  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  // the wifi password is usually longer. The next 64 bytes are reserve for the wifi password.
  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }

  // Print SSID and wifi password (use if for debug if you aren't able to connect to wifi)
  /*
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  Serial.print("PASS: ");
  Serial.println(epass);
  */

  //Try to connect to wifi 20 times before giving up
  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi())
  {
    Serial.println("Succesfully Connected!!!");
    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_FS
        type = "filesystem";
      }

      // NOTE: if updating FS this would be the place to unmount FS using FS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });
    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    IPAddress dns(8, 8, 8, 8);
    checkUpdate();
    return;
  }
  else
  { 
    // Since the ESP wasn't able to connect to the wifi, create a web server and allow you to add a wifi network to the EEPROM
    Serial.println("Turning the HotSpot On");
    launchWeb();
    setupAP();// Setup accesspoint or HotSpot
    Serial.println();
    Serial.println("Waiting.");

    while ((WiFi.status() != WL_CONNECTED))
    {
      Serial.print(".");
      delay(100);
      server.handleClient();
    }
  }

  EEPROM.get(96, humidityOffset);
  Serial.println("humidity offset : " + (String) humidityOffset);

}

void loop() {

  // False by default for security reason
  if(networkPortActivate) {
    ArduinoOTA.handle();
  }

  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)

  // Do humidity measurement (1 sec)
  if (currentMillis - startMillisMeasurement >= periodMeasurement)  //test whether the period has elapsed (by default 1 seconds)
  {
    startMillisMeasurement = currentMillis;  // reset the start value to the actual time
    internalHumidity = dhtInternal.readHumidity(); 
    externalHumidity = dhtExternal.readHumidity();
    if (isnan(internalHumidity)) {
      internalHumidity = 0.0f;
    }
    if (internalHumidity >= 99.8f) {
      // if the humidity get too low, the return value will be 99.9.
      internalHumidity = 0.0f;
    }

    if (isnan(externalHumidity)) {
      externalHumidity = 0.0f;
    }
    internalHumidity += humidityOffset;
    Serial.println(internalHumidity);
    Serial.println(externalHumidity);

    // Redraw the screen
    drawHumidityScreen();
  }

  // check update (1 min)
  if (currentMillis - startMillisUpdate >= periodUpdate)  //test whether the period has elapsed (by default, 1h)
  {
    startMillisUpdate = currentMillis;  //IMPORTANT to save the start time of the current LED state.
    if ((WiFi.status() == WL_CONNECTED))
    {
      checkUpdate();
      Serial.println("checking update");
    }
  }

  // send data (1 h)
  if (currentMillis - startMillisSendData >= periodSendData)  //test whether the period has elapsed (by default, 1h)
  {
    startMillisSendData = currentMillis;  //IMPORTANT to save the start time of the current LED state.
    if ((WiFi.status() == WL_CONNECTED))
    {
      Serial.println("Sending data");
      sendHumidity(internalHumidity, externalHumidity);
    }
  }
}

/**
 * Functions used for saving WiFi credentials and to connect to it which you do not need to change
 * @return bool reprensenting if the wifi is working or not
 */
bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for WiFi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connection timed out, opening AP or Hotspot");
  return false;
}

/**
 * Function that launch a web server and AP to allow you to enter the wifi credential
 */
void launchWeb()
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  
  // Start the server
  server.begin();
  Serial.println("Server started");
}

/**
 * Function that allow you to setup the access point
 */
void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  scanNetwork();
}

/**
 * Function that scan the network to find each wifi available and the associate power (db)
 */
void scanNetwork() {
  int n = WiFi.scanNetworks();
  Serial.println("scan completed");
  if (n == 0)
    Serial.println("No WiFi Networks found");
  else
  {
    Serial.print(n);
    Serial.println(" Networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  ssidListString = "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    ssidListString += "<li>";
    ssidListString += "<button onclick='openModal(\"";
    ssidListString += WiFi.SSID(i);
    ssidListString += "\")'>Choose</button>";
    ssidListString += WiFi.SSID(i);
    ssidListString += " (";
    ssidListString += WiFi.RSSI(i);
    ssidListString += " dbm)";
    ssidListString += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    ssidListString += "</li>";
  }
  ssidListString += "</ol>";
  delay(100);
  WiFi.softAP("DryBox looking for wifi", "");
  Serial.println("Initializing_Wifi_accesspoint");
  launchWeb();
  Serial.println("over");
}

/**
 * Function that create the web page used to enter the wifi credential.
 */
void createWebServer()
{
  {
    server.on("/", []() {
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      htmlContent = "<!DOCTYPE HTML>\r\n<html><h1>Welcome to DryBox interface connector ! </h1>";
      htmlContent += "<p> You can choose a wifi in the list here.</p>";
      // modal
      htmlContent += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      htmlContent += ipStr;
      htmlContent += ssidListString;
      htmlContent += "<div id='myModal' class='modal'><div class='modal-content'><span class='close'>&times;</span><p><form method='get' action='setting'><label>SSID: </label><input id='test' name='ssid' length=32><input name='pass' length=64><input type='submit'></form></p></div></div>";
      htmlContent += "</html>";

      // css to make the modal work
      htmlContent += "<style>.modal{display: none; position: fixed; z-index: 1; left: 0; top: 0; width: 100%; height: 100%; overflow: auto; background-color: rgb(0,0,0); background-color: rgba(0,0,0,0.4);}.modal-content{background-color: #fefefe; margin: 15% auto; padding: 20px; border: 1px solid #888; width: 80%;}.close{color: #aaa; float: right; font-size: 28px; font-weight: bold;}.close:hover,.close:focus{color: black; text-decoration: none; cursor: pointer;}</style>";

      // javascript to make the modal work
      htmlContent += "<script>var modal=document.getElementById('myModal');var btn=document.getElementById('myBtn');var span=document.getElementsByClassName('close')[0];function openModal(parameter){modal.style.display='block'; document.getElementById('test').value=parameter}span.onclick=function(){modal.style.display='none';}; window.onclick=function(event){if (event.target==modal){modal.style.display='none';}}</script>";

      server.send(200, "text/html", htmlContent);
    });
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      scanNetwork();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

      server.sendHeader("Location", String("/"), true);
      server.send ( 302, "text/plain", "");

    });

    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96 + sizeof(float); ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");

        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();

        htmlContent = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.reset();
      } else {
        htmlContent = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", htmlContent);

    });
  }
}

/**
 * Function that send the humidity to the server
 * @param internal : float reprenting the internal humidity
 * @param external : float reprenting the external humidity
 */
void sendHumidity(float internal, float external) {
  WiFiClient client;
  HTTPClient http;

  String serverPath = "http://" + String(serverName) +  "/api/publish_humidity/" + String(internal, 3) + "/" + String(external, 3) + "/";
  Serial.println("url : " + serverPath);
  http.begin(client, serverPath.c_str());
  Serial.println("Mac adresse (for the server) : " + WiFi.macAddress());
  http.addHeader("X-Esp8266-Mac", WiFi.macAddress());

  // Send HTTP POST request
  int httpResponseCode = http.GET();
  Serial.println(httpResponseCode);

  // The web server use response code to limite the amount of data on the network.
  if (httpResponseCode == 205) {
    calibrate();
  }
}
/**
 * Function that take the difference between the two sensor and adjust their values to be equal.
 */
void calibrate(void) {
  internalHumidity = dhtInternal.readHumidity();
  externalHumidity = dhtExternal.readHumidity();
  if (isnan(internalHumidity)) {
    internalHumidity = 0;
  }

  if (isnan(externalHumidity)) {
    externalHumidity = 0;
  }
  humidityOffset = externalHumidity - internalHumidity;
  Serial.print("humidity offset  : " + String(humidityOffset));

  Serial.println("writing calibration :");
  EEPROM.put(humidityOffset, 96);
}

/**
 * Function that draw the informations on the screen
 */
void drawHumidityScreen(void) {
  drawHeader();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.write("In:  ");
  display.print(internalHumidity, 1);
  display.write("%\nOut: ");
  display.print(externalHumidity, 1);
  display.write("%");
  display.display();
}

/**
 * Function that draw the splash screen (while waiting for wifi)
 */
void drawSplashScreen(void) {
  drawHeader();
  display.clearDisplay();
  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
}

/**
 * Function that draw the update screen (might not be called since the reboot is usualy done before)
 */
void drawUpdateScreen(void) {
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.write("Updating.. \n\n");
  display.setTextSize(1);
  display.write("Do not turn off");
  display.display();
}

/**
 * Function that draw the header of the display (show ota state and version)
 */
void drawHeader(void) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);     // Start at top-left corner
  display.setTextColor(WHITE); // Draw white text
  //const char[] headerText = "Wifi : On    V" + version + "\n\n";
  display.write("OTA : ");
  if (otaUpdate) {
    display.write("on ");
  } else {
    display.write("off");
  }
  display.write("    V");
  display.write(version);
  display.write("\n\n");
}

/**
 * Function that check if there is a new version of the code on the server.
 */
void checkUpdate(void) {
  WiFiClient client;
  t_httpUpdate_return ret = ESPhttpUpdate.update(client, serverName, 80, "/binary/", version);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.println("[update] Update failed.");
      otaUpdate = false;
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("[update] No update.");
      otaUpdate = true;
      break;
    case HTTP_UPDATE_OK:
      drawUpdateScreen(); // may not be called since we reboot the ESP
      otaUpdate = true;
      Serial.println("[update] Update ok.");
      break;
  }
}
