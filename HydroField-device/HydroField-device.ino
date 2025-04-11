#include "Arduino_LED_Matrix.h"
#include <ArduinoBLE.h>

#include "MatrixGraphics.h"
#include "ButtonHandler.h"
#include "BluetoothManager.h"
#include "Blinker.h"

void setup()
{
  // ---- Pinout ----
  pinMode(buttonPin, INPUT_PULLUP);
  // ---- Matrix ----
  matrix.begin();
  // ---- Serial ----
  Serial.begin(9600);
  // ---- Bluetooth ----
  setupBluetooth();
  //----------------------------------------
  Serial.println("Initial setup - Complete.");
}

void loop()
{
  checkButton();
  handleBlinking();
  handleBluetooth();
}
