//
//  ButtonHandler.cpp
//
//
//  Created by Piotr Sączewski on 10/04/2025.
//

#include "ButtonHandler.h"
#include "Blinker.h"
#include "BluetoothManager.h"

const int buttonPin = 2;
unsigned long lastButtonPressTime = 0;
const unsigned long debounceDelay = 300; // 300 ms debounce delay
bool lastButtonState = HIGH;
bool buttonPressed = false;

void checkButton()
{
  bool currentButtonState = digitalRead(buttonPin);
  // Check if the button has been pressed and debounce
  if (currentButtonState == LOW && lastButtonState == HIGH && millis() - lastButtonPressTime > debounceDelay)
  {
    buttonPressed = true;
    lastButtonPressTime = millis(); // record the time of the press
  }
  else
  {
    buttonPressed = false;
  }

  if (buttonPressed && !isBlinking && !isBluetoothPairing && !isConnected)
  {
    startBlinkingAndPairing();
  }

  lastButtonState = currentButtonState;
}