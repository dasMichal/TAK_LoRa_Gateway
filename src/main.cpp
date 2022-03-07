#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WireGuard-ESP32.h>
#include <ArduinoJson.h>
#include <string.h>
#include "secrets.h"
#include "prototypes.h"

// SCREEN
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED_PIN 26
#define LED_COUNT 5                                                // How many NeoPixels are attached to the Arduino?
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // Declare our NeoPixel strip object:
#define LORA_SPREADING_FACTOR 12

#define SERIAL_BAUD 19200

// WireGuard configuration --- UPDATE this configuration from JSON
char private_key[] = WIREGUARD_PRIVATE_KEY;          // [Interface] PrivateKey
IPAddress local_ip(192, 168, 2, 5);                  // [Interface] Address
char public_key[] = WIREGUARD_PUBLIC_KEY;            // [Peer] PublicKey
char endpoint_address[] = WIREGUARD_ENDPOINT_ADRESS; // [Peer] Endpoint
int endpoint_port = WIREGUARD_ENDPOINT_PORT;         // [Peer] Endpoint

static constexpr const uint32_t UPDATE_INTERVAL_MS = 5000;
static WireGuard wg;

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

  display.clearDisplay();

  display.setTextSize(0.5);            // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(20, 0);            // Start at top-left corner
  display.println("LoRa TAK Gateway");
  display.setCursor(30, 10); // Start at top-left corner
  display.println("Booting Up");
  display.display();
  delay(1000); // Pause for 2 seconds
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner
  display.setTextSize(1);  // Normal 1:1 pixel scale

  test();

  connectHomeWIFI();

  count = 0;
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner

  init_LoRa();

  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.clear();
  strip.show(); // Turn OFF all pixels ASAP

  // register the receive callback
  LoRa.onReceive(onReceive);

  display.clearDisplay();  // Clear the display  buffer
  display.setCursor(0, 0); // Start at top-left corner
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
    // print_wakeup_reason();
    Serial.println("STATE HIGH");

    if (msgtype == 0x3) // Motion Sensor 1
    {
      sendChat_TAK("Motion Sensor 1", "Motion Detected");
      // LED_alert(strip.Color(127, 0, 0), 500);
      display.clearDisplay();
      display.setCursor(0, 0); // Start at top-left corner
      display.println("Motion Sensor 1");
      display.println("Tripped");
      display.display();
    }
    else if (msgtype == 0x01) // GPS Transmitter 1
    {

      if (GPS_State == 0) // No GPS FIX
      {
        display.clearDisplay();
        display.setCursor(0, 0); // Start at top-left corner
        display.println("GPS 1");
        display.println("No Fix");
        display.display();
      }
      else if (GPS_State == 1)
      {
        display.clearDisplay();
        display.setCursor(0, 0); // Start at top-left corner
        display.println("Michal LoRa Tracker");
        display.print("Latitude ");
        display.println(LatString);
        display.print("Longitude ");
        display.println(LngString);
        display.display();

        postPresence_TAK("41df070c-2de7-11ec-86f8-b827eb0147b9", "Michal LoRa Tracker", LngString, LatString);
        yield();

        // postPresence_TAK("07bd7b3e-2dfc-11ec-b3e3-b827eb0147b9", "Michał LoRa Tracker", LngString, LatString);
        // postPresence_TAK(UID, "Deine Mudda", LngString, LatString);
      }

      // sendChat_TAK("GPS Transmitter 1", "Send Signal");
    }

    delay(1000);
    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    display.display();
    state = LOW;
    // esp_deep_sleep_start();
    // esp_light_sleep_start();
  }
  else
  {
    // Serial.println("STATE LOW");
    // Go to sleep now
    // Serial.println("Going to light-sleep now");
    // delay(2000);
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
  default:
    // Statement(s)
    break; // Wird nicht benötigt, wenn Statement(s) vorhanden sind
  }
}

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

void init_LoRa()
{
  display.println("Setting LoRa Pins:");
  display.display();
  Serial.println(" ");
  Serial.println("Setting LoRa Pins");

  // LoRa.setPins(15, 5, 4); //NODE MCU
  LoRa.setPins(5, 32, 33); // NODE MCU32
  // LoRa.setPins(4, 2, 3); //NANO
  // LoRa.setPins(ss, reset, dio0);

  if (!LoRa.begin(433E6))
  {
    Serial.println("LoRa init failed. Check your connections.");
    sendChat_TAK("Lora Gateway", "Starting LoRa failed!");
    display.clearDisplay();
    display.println("LoRa init failed.");
    display.display();
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

void connectHomeWIFI()
{

  WiFi.begin("imbabura", IMBABURA_PWD); // WiFi connection

  display.println("Connecting to WiFi");
  display.println("Imbabura");
  display.display();
  delay(100);

  Serial.print("Trying to connecting to WiFi imbabura");

  while (WiFi.status() != WL_CONNECTED)
  { // Wait for the WiFI connection completion

    delay(200);
    Serial.print(".");

    if (count > 10)
    {
      Wificonnect = false;
      display.println("Connection Failed");
      display.display();
      Serial.println("No WiFi connection to imbabura");
      delay(500);
      break;
    }
    count++;
  }

  if (!Wificonnect)
  {
    count = 0;
    Wificonnect = true;
    WiFi.begin("Torchwood", TORCHWOOD_PWD); // WiFi connection

    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    display.setTextSize(1);  // Normal 1:1 pixel scale
    display.println("Connecting to WiFi");
    display.println("Torchwood");
    delay(200);

    Serial.print("Trying to connecting to WiFi Torchwood");
    display.display();

    while (WiFi.status() != WL_CONNECTED)
    { // Wait for the WiFI connection completion

      delay(200);
      Serial.print(".");

      if (count > 10)
      {
        Wificonnect = false;
        display.println("Connection Failed");
        display.display();
        Serial.println("No WiFi connection to Torchwood");
        delay(500);
        break;
      }
      count++;
    }
    Serial.print("\n");
  }

  if (Wificonnect)
  {
    display.println("WiFi Connected");
    Serial.println("Connected to WiFi");
    display.display();
    Serial.println("Adjusting system time");
    display.println("Adjusting system time");
    display.display();
    configTime(9 * 60 * 60, 0, "0.de.pool.ntp.org", "time.google.com");
    delay(1000);
    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    Serial.println("Initializing WireGuard...");
    display.println("Initializing\nWireGuard...");

    display.display();
    wg.begin(
        local_ip,
        private_key,
        endpoint_address,
        public_key,
        endpoint_port);
    delay(1000);
    sendChat_TAK("Lora Gateway", "Lora Sensor Gateway Online");
  }
}

void sendChat_TAK(String user_sender, String user_message)
{

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
  }
  else
  {

    Serial.println("Error in WiFi connection");
  }
}

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

void test()
{

  Serial.print("***TEST START");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
  {
    Serial.println("no networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(5000);
}