// Purpose: Handle WiFi connection and WireGuard connection
#include <WireGuard-ESP32.h>

//--Custom libraries--//
#include "wifiHandler.h"
#include "displayHandler.h"
#include "takHandler.h"
#include "secrets.h"


// list of wifi networks to connect to in order of preference
RTC_DATA_ATTR const char *ssid[] = {"Torchwood", "imbabura"};
RTC_DATA_ATTR const char *password[] = {TORCHWOOD_PWD, IMBABURA_PWD};
RTC_DATA_ATTR const int num_networks = 2;
RTC_DATA_ATTR int network_index = 0;

// WireGuard configuration --- UPDATE this configuration from JSON
char private_key[] = WIREGUARD_PRIVATE_KEY;          // [Interface] PrivateKey
IPAddress local_ip(192, 168, 2, 5);                  // [Interface] Address
char public_key[] = WIREGUARD_PUBLIC_KEY;            // [Peer] PublicKey
char endpoint_address[] = WIREGUARD_ENDPOINT_ADRESS; // [Peer] Endpoint
int endpoint_port = WIREGUARD_ENDPOINT_PORT;         // [Peer] Endpoint

static constexpr const uint32_t UPDATE_INTERVAL_MS = 5000;
static WireGuard wg;

boolean Wificonnect = true;
int count;
bool isConnected = false;






void connectToKnownWIFI()
{
    clearAndResetCursor();
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("scan start");
    display->println("scan start");
    display->display();

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0)
    {
        Serial.println("no networks found");
    }
    else
    {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i)
        {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            delay(10);
        }

        
        isConnected = false;
        // check if one of the networks is in the  *ssid[] list of known networks
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < num_networks; j++)
            {

                if (WiFi.SSID(i) == ssid[j])
                {
                    Serial.println("Found a known network");
                    Serial.println(WiFi.SSID(i));
                    Serial.print("Connecting to WiFi");

                    isConnected = connectToWifiNetwork(ssid[j], password[j]);

                    // switch to station mode and connect to the network

                    if (!isConnected)
                    {
                        Serial.println("Connection Failed");
                        display->println("Connection Failed");
                        display->display();
                        delay(500);
                        //try again recursively
                        connectToKnownWIFI();
                        continue;
                    }
                    else
                    {
                        Serial.println("");
                        Serial.println("Connected to WiFi");
                        display->println("Connected to WiFi");
                        display->display();

                        //save the network index to RTC memory
                        network_index = i;
                        connectWireguard();
                        delay(500);
                        
                        break;
                    }
                    Serial.println("Exiting WiFi loop");
                }
            }

            if (isConnected)
            {
                sendChat_TAK("Lora Gateway", "Lora Sensor Gateway Online");
                break;
            }
        }

    }
    Serial.println("");

    // Wait a bit before scanning again
    delay(5000);
}

//Function to connect to a WiFi network
bool connectToWifiNetwork(const char *ssid, const char *password)
{
    Serial.println("Connecting to WiFi network: " + String(ssid));

    WiFi.begin(ssid, password);

    int count = 0;
    while (WiFi.status() != WL_CONNECTED)
    { // Wait for the WiFI connection completion

        delay(200);
        Serial.print(".");

        if (count > 10)
        {
            Serial.println("");
            Wificonnect = false;
            display->println("Connection Failed");
            display->display();
            Serial.println("No WiFi connection ");
            delay(500);
            return false;
        }
        count++;
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    display->println("WiFi connected");
    display->display();
    return true;
}



/*
 * Function to set the WiFi power saving mode to true
 * This will reduce the power consumption of the ESP32
 * The WiFi connection will be slower
 *
 */
void setWiFiPowerSavingMode()
{
    WiFi.setSleep(true);
}

void connectWireguard()
{
    Serial.println("Adjusting system time");
    display->println("Adjusting system time");
    display->display();
    configTime(9 * 60 * 60, 0, "0.de.pool.ntp.org", "time.google.com");
    delay(1000);
    clearAndResetCursor();
    Serial.println("Initializing WireGuard...");
    display->println("Initializing\nWireGuard...");

    display->display();
    wg.begin(
        local_ip,
        private_key,
        endpoint_address,
        public_key,
        endpoint_port);
    delay(1000);
    Serial.println("WireGuard initialized");
}


String getConnectedSSID()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return "";
    }else {return String(ssid[network_index]);}
}