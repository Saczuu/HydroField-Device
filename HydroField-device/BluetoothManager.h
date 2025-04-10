//
//  BluetoothManager.hpp
//
//
//  Created by Piotr Sączewski on 10/04/2025.
//

#ifndef BluetoothManager_h
#define BluetoothManager_h

#include <ArduinoBLE.h>

void handleBluetooth();
void setupBluetooth();
void resetToInitialState();

extern bool isBluetoothPairing;
extern bool isConnected;
extern unsigned long btStartTime;

#endif /* BluetoothManager_hpp */
