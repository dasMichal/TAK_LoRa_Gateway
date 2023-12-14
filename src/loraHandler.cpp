#include <Arduino.h>



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