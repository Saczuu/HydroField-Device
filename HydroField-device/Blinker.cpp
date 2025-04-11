//
//  Blinker.cpp
//
//
//  Created by Piotr Sączewski on 10/04/2025.
//

#include "Blinker.h"
#include "BluetoothManager.h"
#include "MatrixGraphics.h"

bool isBlinking = false;
unsigned long blinkStartTime = 0;
unsigned long lastBlinkToggle = 0;
bool blinkState = false;

void startBlinkingAndPairing()
{
  isBlinking = true;
  blinkStartTime = millis();
  lastBlinkToggle = millis();
  blinkState = false;
  isBluetoothPairing = true;
  btStartTime = millis();
  Serial.println("Bluetooth pairing started...");
}

void handleBlinking()
{
  if (!isBlinking || isConnected)
    return;

  if (millis() - blinkStartTime >= 60000)
  {
    matrix.loadFrame(empty);
    isBlinking = false;
    return;
  }

  if (millis() - lastBlinkToggle >= 500)
  {
    blinkState = !blinkState;
    if (blinkState)
    {
      matrix.renderBitmap(frameLetterB, 8, 12);
    }
    else
    {
      matrix.loadFrame(empty);
    }
    lastBlinkToggle = millis();
  }
}