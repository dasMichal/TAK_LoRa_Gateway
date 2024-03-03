#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// SCREEN
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)


//extern Adafruit_SSD1306* display;
extern Adafruit_SSD1306* display;

Adafruit_SSD1306* getDisplay();

void displayHandler(void);
void displayBootscreen();
void clearAndResetCursor();
void drawLine(int y);
void drawStats(int cpuSleepState, int loraSignalStrength, int wifiStrength, String wifiSSID);