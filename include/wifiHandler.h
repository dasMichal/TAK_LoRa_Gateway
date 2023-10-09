#pragma once

//void connectToWifi();
void connectToKnownWIFI();
bool connectToWifiNetwork(const char *ssid, const char *password);
void connectWireguard();
void setWiFiPowerSavingMode();