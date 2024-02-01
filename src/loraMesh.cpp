// Variable list to store all known Lora Devices and their status (active or inactive)
// and also theri known devices (to be used for routing)

#include <map>
#include <vector>
#include <string>

// Define a struct for the device data
struct DeviceData {
    bool isActive; // Status of the device
    std::vector<std::string> knownDevices; // List of known devices
};

// Create a map to store the devices
std::map<std::string, DeviceData> devices;

// Define a struct for the message data
struct MessageData {
    std::string message; // The message
    std::string sender; // The sender
    std::string receiver; // The receiver
};

// Create a vector to store the messages
std::vector<MessageData> messages;

//function to add a device to the map
void addDevice(std::string deviceName, bool isActive, std::vector<std::string> knownDevices) {
    // Create a new device data struct
    DeviceData deviceData;
    deviceData.isActive = isActive;
    deviceData.knownDevices = knownDevices;

    // Add the device to the map
    devices[deviceName] = deviceData;
}   

//remove a device from the map
void removeDevice(std::string deviceName) {
    // Remove the device from the map
    devices.erase(deviceName);
}


//function to update a device in the map
void updateDevice(std::string deviceName, bool isActive, std::vector<std::string> knownDevices) {
    // Create a new device data struct
    DeviceData deviceData;
    deviceData.isActive = isActive;
    deviceData.knownDevices = knownDevices;

    // Update the device in the map
    devices[deviceName] = deviceData;
}

//function to add a message to the vector
void addMessage(std::string message, std::string sender, std::string receiver) {
    // Create a new message data struct
    MessageData messageData;
    messageData.message = message;
    messageData.sender = sender;
    messageData.receiver = receiver;

    // Add the message to the vector
    messages.push_back(messageData);
}

//function to get a device from the map
DeviceData getDevice(std::string deviceName) {
    // Get the device from the map
    return devices[deviceName];
}

