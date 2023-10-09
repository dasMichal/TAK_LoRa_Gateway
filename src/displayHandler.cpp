#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// SCREEN
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//function to get the display object
Adafruit_SSD1306 getDisplay() {
  return display;
}



//Boot screen function to display the boot screen
void displayBootscreen() {
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
}


void clearAndResetCursor() {
  display.clearDisplay();
  display.setCursor(0, 0);
}




