#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <string.h>


#include "secrets.h"
#include "config.h"
#include "takHandler.h"



String UID = "";


/*
  * Send Chat to TAK Server using HTTP POST 

*/
bool sendChat_TAK(String user_sender, String user_message)
{
  Serial.println("Sending Chat to TAK");
  if (WiFi.status() == WL_CONNECTED)
  { // Check WiFi connection status

    HTTPClient http; // Declare object of class HTTPClient

    http.begin("http://192.168.178.130:19023/ManageChat/postChatToAll"); // Specify request destination
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
    //connectToWifi();
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