#include "displayHandler.h"

// Global variable for display
//Adafruit_SSD1306* 
Adafruit_SSD1306* display = getDisplay();

//Now set the display size 
//initialize the display object with the correct parameters and size 
Adafruit_SSD1306* getDisplay() {
  static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  return &display;
}


//Boot screen function to display the boot screen
void displayBootscreen() {
  Adafruit_SSD1306* display = getDisplay();
  display->clearDisplay();

  display->setTextSize(0.5);            // Normal 1:1 pixel scale
  display->setTextColor(SSD1306_WHITE); // Draw white text
  display->setCursor(20, 0);            // Start at top-left corner
  display->println("LoRa TAK Gateway");
  display->setCursor(30, 10); // Start at top-left corner
  display->println("Booting Up");
  display->display();
  delay(1000); // Pause for 2 seconds
  display->clearDisplay();
  display->setCursor(0, 0); // Start at top-left corner
  display->setTextSize(1);  // Normal 1:1 pixel scale
}


void clearAndResetCursor() {
  Adafruit_SSD1306* display = getDisplay();
  display->clearDisplay();
  display->setCursor(0, 0);
}

void drawLine(int y) {
  Adafruit_SSD1306* display = getDisplay();
  display->drawFastHLine(0, y, SCREEN_WIDTH, SSD1306_WHITE);
}

void drawStats(int cpuSleepState, int loraSignalStrength, int wifiStrength, String wifiSSID) {
  Adafruit_SSD1306* display = getDisplay();
  Serial.println("Displaying Stats");
  Serial.println(cpuSleepState);
  Serial.println(loraSignalStrength);
  Serial.println(wifiStrength);
  Serial.println(wifiSSID);


  clearAndResetCursor();
  // Clear the display buffer
  display->clearDisplay();

  // Display CPU Sleep State
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(0, 0);
  display->print("CPU Sleep State:");

  //If the CPU Sleep State is 0, then the CPU is awake and print awake
  if (cpuSleepState == 0) {
    display->println("Awake");
  }
  //If the CPU Sleep State is 1, then the CPU is light sleep and print asleep
  else if (cpuSleepState == 1) {
    display->println("Light");
  }
  //If the CPU Sleep State is 2, then the CPU is deep sleep and print asleep
  else if (cpuSleepState == 2) {
    display->println("Deep");
  }
  
  // Display LoRa Signal Strength
  //display->setCursor(0, 13);
  display->print("LoRa Signal:");
  display->println(loraSignalStrength);
  // Display WiFi Strength
  //display->setCursor(0, 30);
  display->print("WiFi Strength:");
  display->println(wifiStrength);
  //WifiSSID
  //display->setCursor(0, 45);
  display->print("WiFi SSID:");
  display->println(wifiSSID);

  // Display the buffer
  display->display();
}


