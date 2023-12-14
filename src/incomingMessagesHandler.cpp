#include <Arduino.h>
//--Custom libraries--//
#include "displayHandler.h"




// Function that handles incoming LoRa messages
void processIncomingMessage(int msgType)
{

    if (msgType == 0x3) // Motion Sensor 1
    {
        //sendChat_TAK("Motion Sensor 1", "Motion Detected");
        Serial.println("Motion Sensor 1 Tripped");
        // LED_alert(strip.Color(127, 0, 0), 500);
        clearAndResetCursor();
        display->println("Motion Sensor 1");
        display->println("Tripped");
        display->display();
    }
    else if (msgType == 0x01) // GPS Transmitter 1
    {

      //postPresence_TAK("41df070c-2de7-11ec-86f8-b827eb0147b9", "Michal LoRa Tracker", LngString, LatString);
      
      // postPresence_TAK("07bd7b3e-2dfc-11ec-b3e3-b827eb0147b9", "Michał LoRa Tracker", LngString, LatString);
      // postPresence_TAK(UID, "Deine Mudda", LngString, LatString);
      

        // sendChat_TAK("GPS Transmitter 1", "Send Signal");
    }
}



