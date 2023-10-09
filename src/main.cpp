#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WireGuard-ESP32.h>
#include <ArduinoJson.h>
#include <string.h>

//--Custom libraries--//
#include "secrets.h"
#include "prototypes.h"
#include "config.h"
#include "displayHandler.h"
#include "wifiHandler.h"
#include "incomingMessagesHandler.h"


Adafruit_SSD1306 display = getDisplay();

// #define LED_PIN 26
// #define LED_COUNT 5                                                // How many NeoPixels are attached to the Arduino?
// Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // Declare our NeoPixel strip object:
#define LORA_SPREADING_FACTOR 12
#define SERIAL_BAUD 19200
#define LORA_FREQUENCY 433E6



// list of wifi networks to connect to in order of preference
RTC_DATA_ATTR const char *ssid[] = {"Torchwood", "imbabura"};
RTC_DATA_ATTR const char *password[] = {TORCHWOOD_PWD, IMBABURA_PWD};
RTC_DATA_ATTR const int num_networks = 2;

String recived;
int rssi;
int msgtype;
volatile byte state = LOW;
int count;
boolean Wificonnect = true;
byte GPS_State;

String LatString = "";
String LngString = "";
String SpeedString = "";
String AttitudeString = "";
String UID = "";

//------------------

RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }
}

void setup()
{

  Serial.begin(SERIAL_BAUD);
  delay(1000);

  // Increment boot number and print it every reboot
  //++bootCount;
  // Serial.println("Boot number: " + String(bootCount));

  // print_wakeup_reason();
  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);
  gpio_wakeup_enable(GPIO_NUM_33, GPIO_INTR_HIGH_LEVEL);
  esp_sleep_enable_gpio_wakeup();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  displayBootscreen();

  WiFi.mode(WIFI_STA);
  connectToKnownWIFI();
  setWiFiPowerSavingMode();

  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner

  init_LoRa();

  // register the receive callback
  LoRa.onReceive(onReceive);

  clearAndResetCursor();
  Serial.println("Setup Finished");
  display.println("Setup Finished");
  display.display();

  LoRa.receive(); // put the radio into receive mode
}

void loop()
{
  display.clearDisplay();
  display.display();

  if (state == HIGH)
  {
    print_wakeup_reason();
    Serial.println("STATE HIGH");

    // check wifi connection and reconnect if necessary
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi Disconnected after Wakeup");
      clearAndResetCursor();
      display.println("WiFi Disconnected");
      display.display();
      connectToWifi();
    }
    else
    {
      Serial.println("WiFi Connected after Wakeup");
      clearAndResetCursor();
      display.println("WiFi Connected");
      display.display();
    }

    processIncomingMessage(msgtype);

    delay(1000);
    clearAndResetCursor();
    display.display();
    state = LOW;
    Serial.println("Setting State LOW");
    delay(500);
    // esp_deep_sleep_start();
    // esp_light_sleep_start();
  }
  else
  {
    // Serial.println("STATE LOW");
    //  Go to sleep now
    // Serial.println("Going to light-sleep now");
    // delay(500);
    // esp_deep_sleep_start();
    // esp_light_sleep_start();
  }
}

void onReceive(int packetSize)
{
  // received a packet
  Serial.println("Received packet");

  msgtype = LoRa.read(); // recipient address

  switch (msgtype)
  {
  case 0x01:
    int counter;

    GPS_State = LoRa.read();

    if (GPS_State == 0) // No GPS FIX
    {
      Serial.print("No GPS FIX from Tracker: ");
      Serial.println(msgtype, HEX);
      counter = LoRa.read();

      Serial.print("Data: ");
      Serial.println(GPS_State);
      Serial.println(counter);
    }
    else if (GPS_State == 1) // GPS FIX
    {
      Serial.print("GPS fix from Tracker: ");
      Serial.println(msgtype, HEX);

      // Reciving the GPS Data
      // UID = LoRa.readStringUntil('\n');
      LatString = LoRa.readStringUntil('\n');
      LngString = LoRa.readStringUntil('\n');
      // SpeedString = LoRa.readStringUntil('\n');
      // AttitudeString = LoRa.readStringUntil('\n');

      Serial.println(LatString);
      Serial.println(LngString);
      // Serial.println(SpeedString);
      // Serial.println(AttitudeString);
    }

    state = HIGH;
    break;
  case 0x3:
    // Serial.println("Data for me :)");
    Serial.print("Data: ");
    // read packet
    for (int i = 0; i < packetSize; i++)
    {
      Serial.print((char)LoRa.read());
    }
    Serial.println(" ");
    state = HIGH;
    break;
  default:
    // Statement(s)
    break; // Wird nicht benötigt, wenn Statement(s) vorhanden sind
  }
}

/*
void LED_alert(uint32_t color, int wait)
{
  for (int a = 0; a < 3; a++)
  { // Repeat 3 times...

    // Serial.println(a);
    strip.clear();
    strip.show();

    for (int i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, color);
      strip.show(); // Update strip with new contents
      delay(100);
    }

    strip.show(); // Update strip with new contents
    delay(500);   // Pause for a moment
    // Serial.println("Finito");
  }

  strip.clear();
  strip.show();
}
*/

/*
  Function to initialize the LoRa Module and display the status on the OLED

*/
void init_LoRa()
{
  clearAndResetCursor();
  display.println("Setting LoRa Pins:");
  display.display();
  Serial.println(" ");
  Serial.println("Setting LoRa Pins");

  // LoRa.setPins(15, 5, 4); //NODE MCU
  LoRa.setPins(5, 32, 33); // NODE MCU32
  // LoRa.setPins(4, 2, 3); //NANO
  // LoRa.setPins(ss, reset, dio0);

  if (!LoRa.begin(LORA_FREQUENCY))
  {
    Serial.println("LoRa init failed. Check your connections.");
    sendChat_TAK("Lora Gateway", "Starting LoRa failed!");
    display.clearDisplay();
    display.println("LoRa init failed.");
    display.display();
    delay(5000);
    init_LoRa();
    while (1)
      ;
  }
  else
  {

    // put the radio into receive mode
    display.println("LoRa init sucessfull");
    display.display();
    Serial.println("LoRa init sucessfull");
    LoRa.receive();
    LoRa.enableCrc();
    LoRa.setSpreadingFactor(LORA_SPREADING_FACTOR);
  }
}

/*
  * Send Chat to TAK Server using HTTP POST

*/
bool sendChat_TAK(String user_sender, String user_message)
{
  Serial.println("Sending Chat to TAK");
  if (WiFi.status() == WL_CONNECTED)
  { // Check WiFi connection status

    HTTPClient http; // Declare object of class HTTPClient

    http.begin("http://192.168.178.42:19023/ManageChat/postChatToAll"); // Specify request destination
                                                                        // http.addHeader("Content-Type", "application/x-www-form-urlencoded", false, true);

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", APIKEY);
    // int httpResponseCode = http.POST("{\"message\":\"Movement\",\"sender\":\"Motion Sensor\"}");

    int httpResponseCode = http.POST("{\"message\":\"" + user_message + "\",\"sender\": \"" + user_sender + "\" }");

    // int httpResponseCode = http.GET();
    // String payload = http.getString();

    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.print(httpResponseCode);
      Serial.print(" ");
      String payload = http.getString();
      Serial.println(payload);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    return true;
  }
  else
  {
    // connectToWifi();
    Serial.println("Error in WiFi connection");
    return false;
  }
}

/*
 * Send Presence to TAK Server
 */
void postPresence_TAK(String user_uid, String user_name, String user_lng, String user_lat)
{

  if (WiFi.status() == WL_CONNECTED)
  { // Check WiFi connection status

    HTTPClient http; // Declare object of class HTTPClient

    http.begin("http://192.168.178.42:19023/ManagePresence/postPresence"); // Specify request destination
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", APIKEY);

    user_lat.trim();
    user_lng.trim();
    // user_uid.trim();
    int httpResponseCode;

    DynamicJsonDocument doc(2048);
    doc["uid"] = user_uid;
    doc["how"] = "nonCoT";
    doc["name"] = user_name;
    doc["longitude"] = user_lng;
    doc["latitude"] = user_lat;
    doc["role"] = "Team Member";
    doc["team"] = "Red";

    // Serialize JSON document
    String json;
    serializeJson(doc, json);

    // Serial.println(json);

    // Send the request
    httpResponseCode = http.POST(json);

    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.print(httpResponseCode);
      Serial.print(" ");
      String payload = http.getString();
      Serial.println(payload);
      UID = payload;
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else
  {

    Serial.println("Error in WiFi connection");
  }
}

