#include <Arduino.h>
#include <ArduinoJson.h>



//Custom libraries
#include "loraHandler.h"
#include "takHandler.h"
#include "displayHandler.h"


#define LORA_SPREADING_FACTOR 12
#define SERIAL_BAUD 19200
#define LORA_FREQUENCY 433E6

/*
  Function to initialize the LoRa Module and display the status on the OLED

*/
void init_LoRa()
{
  clearAndResetCursor();
  display->println("Setting LoRa Pins:");
  display->display();
  Serial.println(" ");
  Serial.println("Setting LoRa Pins");

  // LoRa.setPins(15, 5, 4); //NODE MCU
  LoRa.setPins(5, 32, 33); // NODE MCU32
  // LoRa.setPins(4, 2, 3); //NANO
  // LoRa.setPins(ss, reset, dio0);

  if (!LoRa.begin(LORA_FREQUENCY))
  {
    Serial.println("LoRa init failed. Check your connections.");
    //sendChat_TAK("Lora Gateway", "Starting LoRa failed!");
    display->clearDisplay();
    display->println("LoRa init failed.");
    display->display();
    delay(5000);
    init_LoRa();
    while (1)
      ;
  }
  else
  {

    // put the radio into receive mode
    display->println("LoRa init sucessfull");
    display->display();
    Serial.println("LoRa init sucessfull");
    LoRa.receive();
    LoRa.enableCrc();
    LoRa.setSpreadingFactor(LORA_SPREADING_FACTOR);
  }
}

/*
  Function to send a message via LoRa

  @param message The message to send
  @param receiver The receiver of the message
*/  
void sendLoRaMessage(String message, String receiver)
{
  // Create a JSON object
  StaticJsonDocument<200> doc;

  // Add the message and receiver to the JSON object
  doc["message"] = message;
  doc["receiver"] = receiver;

  // Serialize the JSON object to a string
  String jsonString;
  serializeJson(doc, jsonString);

  // Send the message via LoRa
  LoRa.beginPacket();
  LoRa.print(jsonString);
  LoRa.endPacket();
}

/*
  Function to send a heartbeat signal via LoRa

  @param uuid The UUID of the device
  @param devices The list of devices
*/

void sendHeartbeatSignal(String uuid, std::map<std::string, DeviceData> devices)
{
  // Create a JSON object
  StaticJsonDocument<200> doc;

  // Add the UUID and device list to the JSON object
  doc["uuid"] = uuid;

  // Create a JSON array for the device list
  JsonArray deviceList = doc.createNestedArray("devices");

  // Iterate over the devices and add them to the array
  for (const auto& device : devices)
  {
    JsonObject deviceObj = deviceList.createNestedObject();
    deviceObj["deviceName"] = device.first;
    deviceObj["deviceData"] = device.second;
  }

  // Serialize the JSON object to a string
  String jsonString;
  serializeJson(doc, jsonString);

  // Send the heartbeat signal via LoRa
  LoRa.beginPacket();
  LoRa.print(jsonString);
  LoRa.endPacket();
}

