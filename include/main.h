#pragma once



void print_wakeup_reason();
void onReceive(int packetSize);

bool sendChat_TAK(String user_sender, String user_message);
void postPresence_TAK(String user_uid, String user_name, String user_lng, String user_lat);
void init_LoRa();