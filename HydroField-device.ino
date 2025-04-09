#include "Arduino_LED_Matrix.h"
#include <ArduinoBLE.h>

// Bluetooth setup
const char *deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";  // The UUID for your custom service

// Create a BLE Service
BLEService myService(deviceServiceUuid);

// Create a BLE Characteristic (this can be for any data you want to exchange)
BLECharacteristic statusCharacteristic("19b10001-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite, 20);

// Arduino Matrix
ArduinoLEDMatrix matrix;

// Pinout
const int buttonPin = 2;

// Helpers variables
bool isBlinking = false;
unsigned long blinkStartTime = 0;
unsigned long lastBlinkToggle = 0;
bool blinkState = false;

bool isBluetoothPairing = false;
unsigned long btStartTime = 0;
bool isConnected = false;  // Flag to track if a device is connected

byte frameLetterB[8][12] = {
  { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 }
};

const uint32_t empty[] = { 0x0, 0x0, 0x0 };

// Variables for debouncing
unsigned long lastButtonPressTime = 0;
const unsigned long debounceDelay = 300; // 300 ms debounce delay
bool lastButtonState = HIGH;
bool buttonPressed = false;

void startBlinkingAndPairing() {
  isBlinking = true;
  blinkStartTime = millis();
  lastBlinkToggle = millis();
  blinkState = false;

  isBluetoothPairing = true;
  btStartTime = millis();

  Serial.println("Bluetooth pairing started...");
}

void handleBlinking() {
  if (!isBlinking || isConnected) return;

  if (millis() - blinkStartTime >= 60000) {
    matrix.loadFrame(empty);
    isBlinking = false;
    return;
  }

  if (millis() - lastBlinkToggle >= 500) {
    blinkState = !blinkState;
    if (blinkState) {
      matrix.renderBitmap(frameLetterB, 8, 12);
    } else {
      matrix.loadFrame(empty);
    }
    lastBlinkToggle = millis();
  }
}

void handleBluetooth() {
  if (!isBluetoothPairing) return;
  BLE.advertise();
  Serial.println("Bluetooth is ready to pair!");

  if (millis() - btStartTime >= 60000) {
    Serial.println("Bluetooth pairing timeout.");
    BLE.stopAdvertise();
    isBluetoothPairing = false;
    return;
  }

  // Check if a device has connected
  BLEDevice central = BLE.central();
  if (central) {
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
    while (central.connected()) {
      delay(1000);  // Wait for the connection to stay alive
    }

    // Once disconnected, reset to initial state
    Serial.println("Device disconnected, restarting to initial state...");
    resetToInitialState();
  }
}

void resetToInitialState() {
  // Reset all flags and stop showing the frameLetterB
  isConnected = false;
  isBluetoothPairing = false;
  isBlinking = false;
  matrix.loadFrame(empty);  // Clear the display
  Serial.println("System is now ready for pairing again. Press the button to start.");
}

void checkButton() {
  bool currentButtonState = digitalRead(buttonPin);

  // Check if the button has been pressed and debounce
  if (currentButtonState == LOW && lastButtonState == HIGH && millis() - lastButtonPressTime > debounceDelay) {
    buttonPressed = true;
    lastButtonPressTime = millis(); // record the time of the press
  } else {
    buttonPressed = false;
  }

  if (buttonPressed && !isBlinking && !isBluetoothPairing && !isConnected) {
    startBlinkingAndPairing();
  }

  lastButtonState = currentButtonState;
}

void setup() {
  // ---- Pinout ----
  pinMode(buttonPin, INPUT_PULLUP);
  // ---- Matrix ----
  matrix.begin();
  // ---- Serial ----
  Serial.begin(9600);

  // ---- Bluetooth ----
  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Starting Bluetooth® Low Energy module failed!");
    while (1); // If BLE init fails, stop here
  }

  // Set the Bluetooth device name
  BLE.setDeviceName("HydroField-BT-hfdb");
  BLE.setLocalName("HydroField-BT-hfdb");   // This is the name that will show up when advertising

  // Add the characteristic to the service
  myService.addCharacteristic(statusCharacteristic);

  // Add the service to the BLE stack
  BLE.addService(myService);

  //----------------------------------------
  Serial.println("Initial setup - Complete.");
}

void loop() {
  checkButton();
  handleBlinking();
  handleBluetooth();
}