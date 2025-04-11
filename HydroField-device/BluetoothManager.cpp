//
//  BluetoothManager.cpp
//
//
//  Created by Piotr Sączewski on 10/04/2025.
//

#include "BluetoothManager.h"
#include "Blinker.h"
#include "MatrixGraphics.h"

// Bluetooth setup
const char *deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214"; // The UUID for your custom service

// Create a BLE Service
BLEService myService(deviceServiceUuid);

// Create a BLE Characteristic (this can be for any data you want to exchange)
BLECharacteristic statusCharacteristic("19b10001-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite, 20);

bool isBluetoothPairing = false;
bool isConnected = false;
unsigned long btStartTime = 0;

void handleBluetooth()
{
    if (!isBluetoothPairing)
        return;
    BLE.advertise();
    Serial.println("Bluetooth is ready to pair!");

    if (millis() - btStartTime >= 60000)
    {
        Serial.println("Bluetooth pairing timeout.");
        BLE.stopAdvertise();
        isBluetoothPairing = false;
        return;
    }

    // Check if a device has connected
    BLEDevice central = BLE.central();
    if (central)
    {
        // A device has connected, stop advertising
        Serial.print("Device connected: ");
        Serial.println(central.address());
        Serial.print("Device Name: ");
        Serial.println(central.deviceName());

        // Stop advertising and blinking
        BLE.stopAdvertise();
        isBluetoothPairing = false;
        isBlinking = false;
        isConnected = true;

        // Keep showing the frameLetterB on the matrix
        matrix.renderBitmap(frameLetterB, 8, 12);

        // Wait until the device disconnects
        while (central.connected())
        {
            delay(1000); // Wait for the connection to stay alive
        }

        // Once disconnected, reset to initial state
        Serial.println("Device disconnected, restarting to initial state...");
        resetToInitialState();
    }
}

void setupBluetooth()
{
    // Initialize BLE
    if (!BLE.begin())
    {
        Serial.println("Starting Bluetooth® Low Energy module failed!");
        while (1)
            ; // If BLE init fails, stop here
    }
    // Set the Bluetooth device name
    BLE.setDeviceName("HydroField-BT-hfdb");
    BLE.setLocalName("HydroField-BT-hfdb"); // This is the name that will show up when advertising
    // Add the characteristic to the service
    myService.addCharacteristic(statusCharacteristic);
    // Add the service to the BLE stack
    BLE.addService(myService);
}

void resetToInitialState()
{
    // Reset all flags and stop showing the frameLetterB
    isConnected = false;
    isBluetoothPairing = false;
    isBlinking = false;
    matrix.loadFrame(empty); // Clear the display
    Serial.println("System is now ready for pairing again. Press the button to start.");
}