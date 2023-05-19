#pragma once

void print_wakeup_reason();
void onReceive(int packetSize);
void LED_alert(uint32_t color, int wait);
void connectHomeWIFI();
bool sendChat_TAK(String user_sender, String user_message);
void postPresence_TAK(String user_uid, String user_name, String user_lng, String user_lat);
void init_LoRa();
void connectToWifi();
void connectWireguard();
void setWiFiPowerSavingMode();