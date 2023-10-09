#include <Arduino.h>
#include <Adafruit_SSD1306.h>

//--Custom libraries--//
#include "displayHandler.h"

Adafruit_SSD1306 display = getDisplay();



// Function that handles incoming LoRa messages
void processIncomingMessage(int msgType)
{

    if (msgType == 0x3) // Motion Sensor 1
    {
        //sendChat_TAK("Motion Sensor 1", "Motion Detected");
        Serial.println("Motion Sensor 1 Tripped");
        // LED_alert(strip.Color(127, 0, 0), 500);
        clearAndResetCursor();
        display.println("Motion Sensor 1");
        display.println("Tripped");
        display.display();
    }
    else if (msgType == 0x01) // GPS Transmitter 1
    {

        if (GPS_State == 0) // No GPS FIX
        {
            clearAndResetCursor();
            display.println("GPS 1");
            display.println("No Fix");
            display.display();
        }
        else if (GPS_State == 1)
        {
            clearAndResetCursor();
            display.println("Michal LoRa Tracker");
            display.print("Latitude ");
            display.println(LatString);
            display.print("Longitude ");
            display.println(LngString);
            display.display();

            //postPresence_TAK("41df070c-2de7-11ec-86f8-b827eb0147b9", "Michal LoRa Tracker", LngString, LatString);
            yield();

            // postPresence_TAK("07bd7b3e-2dfc-11ec-b3e3-b827eb0147b9", "Michał LoRa Tracker", LngString, LatString);
            // postPresence_TAK(UID, "Deine Mudda", LngString, LatString);
        }

        // sendChat_TAK("GPS Transmitter 1", "Send Signal");
    }
}



void onReceiveLoRaMessage(int packetSize)
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
