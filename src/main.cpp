#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <ArduinoJson.h>
#include <string.h>


//--Custom libraries--//
#include "main.h"
#include "secrets.h"
#include "config.h"
#include "displayHandler.h"
#include "wifiHandler.h"
#include "incomingMessagesHandler.h"
#include "loraHandler.h"


//Adafruit_SSD1306 display = getDisplay();

// #define LED_PIN 26
// #define LED_COUNT 5                                                // How many NeoPixels are attached to the Arduino?
// Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // Declare our NeoPixel strip object:
//#define LORA_SPREADING_FACTOR 12
//#define LORA_FREQUENCY 433E6

#define SERIAL_BAUD 19200

String recived;
int rssi;
int msgtype;
volatile byte state = LOW;





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
  //esp_sleep_enable_ulp_wakeup();
  esp_sleep_enable_wifi_wakeup();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display->begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  displayBootscreen();
  display->clearDisplay();
  display->setCursor(0, 0); // Start at top-left corner

  WiFi.mode(WIFI_STA);
  connectToKnownWIFI();
  setWiFiPowerSavingMode();

  display->clearDisplay();
  display->setCursor(0, 0); // Start at top-left corner

  init_LoRa();

  // register the receive callback
  LoRa.onReceive(onReceive);

  clearAndResetCursor();
  Serial.println("Setup Finished");
  display->println("Setup Finished");
  display->display();

  LoRa.receive(); // put the radio into receive mode
  drawStats(0,LoRa.packetRssi(),WiFi.RSSI());
  //esp_sleep_enable_wifi_wakeup();

  
}

void loop()
{
  //display->clearDisplay();
  //display->display();

  if (state == HIGH)
  {
    print_wakeup_reason();
    Serial.println("STATE HIGH");

    // check wifi connection and reconnect if necessary
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi Disconnected after Wakeup");
      clearAndResetCursor();
      display->println("WiFi Disconnected");
      display->display();
      //connectToWifi();
      connectToKnownWIFI();
    }
    else
    {
      Serial.println("WiFi Connected after Wakeup");
      clearAndResetCursor();
      display->println("WiFi Connected");
      display->display();
    }

    processIncomingMessage(msgtype);

    delay(1000);
    clearAndResetCursor();
    display->display();
    state = LOW;
    Serial.println("Setting State LOW");
    delay(500);
    //drawStats(1,LoRa.packetRssi(),WiFi.RSSI());
    //esp_deep_sleep_start();

    esp_sleep_enable_wifi_wakeup();
    esp_light_sleep_start();
    
  
  }
  else
  {
    Serial.println("STATE LOW");
    //  Go to sleep now
    Serial.println("Going to light-sleep now");
    delay(500);

    drawStats(1,LoRa.packetRssi(),WiFi.RSSI());
    //esp_deep_sleep_start();
    esp_sleep_enable_wifi_wakeup();
    esp_light_sleep_start();
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
    //TODO: GPS as Protobuf but have to update the GPS Tracker first to send Protobuf instead of String

    break;
  case 0x3:
    
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



