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

void drawStats(int cpuSleepState, int loraSignalStrength, int wifiStrength) {
  Adafruit_SSD1306* display = getDisplay();
  Serial.println("Displaying Stats");
  Serial.println(cpuSleepState);
  Serial.println(loraSignalStrength);
  Serial.println(wifiStrength);


  clearAndResetCursor();
  // Clear the display buffer
  display->clearDisplay();

  // Display CPU Sleep State
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(0, 0);
  display->print("CPU Sleep State:");
  display->println(cpuSleepState);
  drawLine(12);

  // Display LoRa Signal Strength
  display->setCursor(0, 18);
  display->print("LoRa Signal:");
  display->println(loraSignalStrength);
  drawLine(47);
  display->setCursor(0, 55);

  // Display WiFi Strength
  display->setCursor(0, 75);
  display->println("WiFi Strength:");
  drawLine(90);
  display->setCursor(20, 95);
  display->print("   ");
  display->println(wifiStrength);

  // Display the buffer
  display->display();
}


