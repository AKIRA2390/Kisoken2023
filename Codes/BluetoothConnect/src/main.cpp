#include <Arduino.h>
#include <M5Core2.h>

bool button_state = false;
void init_button();
void not_push_button();
void push_button();
void act();

void init_button() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.fillEllipse(160, 120, 110, 110, LIGHTGREY);
  not_push_button();
}

void push_button() {
  M5.Axp.SetLDOEnable(3, 1);

  M5.Lcd.fillEllipse(160, 120, 90, 90, ORANGE);
  M5.Lcd.setTextSize(14);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setCursor(80, 95);
  M5.Lcd.printf("PUSH");
  act();
}

void not_push_button() {
  M5.Axp.SetLDOEnable(3, 0);
  M5.Lcd.fillEllipse(160, 120, 90, 90, RED);
  M5.Lcd.setTextSize(14);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(80, 95);
  M5.Lcd.printf("PUSH");
}

/*
    Based on Neil Kolban example for IDF:
   https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

/** NimBLE differences highlighted in comment blocks **/

/*******original********
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
***********************/

#include <NimBLEDevice.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define svcKisoken "BABE"
#define UUIDSTATS "0000"
#define UUIDCommand "CCDD"
#define UUIDA "AAAA"
#define UUIDB "BBBB"
#define UUIDC "CCCC"
#define UUIDD "DDDD"
#define UUIDCOUNT "CC00"
#define UUIDNotifyer "0000"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *chrStatus;
BLECharacteristic *chrCommand;
BLECharacteristic *chrA;
BLECharacteristic *chrB;
BLECharacteristic *chrC;
BLECharacteristic *chrD;

void act() {
  chrCommand->setValue("MOVJ");
  chrA->setValue(302);
  chrB->setValue(244);
  chrC->setValue(34);
  chrD->setValue(147);
  chrCommand->notify();

  delay(2000);

  chrCommand->setValue("MOVJ");
  chrA->setValue(208);
  chrB->setValue(208);
  chrC->setValue(34);
  chrD->setValue(84);
  chrCommand->notify();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  M5.begin();
  init_button();

  BLEDevice::init("KisokenC2023");
  pServer = BLEDevice::createServer();
  pService = pServer->createService(svcKisoken);
  chrStatus = pService->createCharacteristic(UUIDSTATS, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ);
  chrCommand = pService->createCharacteristic(
      UUIDCommand, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  chrA = pService->createCharacteristic(UUIDA, NIMBLE_PROPERTY::READ);
  chrB = pService->createCharacteristic(UUIDB, NIMBLE_PROPERTY::READ);
  chrC = pService->createCharacteristic(UUIDC, NIMBLE_PROPERTY::READ);
  chrD = pService->createCharacteristic(UUIDD, NIMBLE_PROPERTY::READ);

  chrStatus->setValue("YET");
  chrCommand->setValue("MOVJ");
  chrA->setValue(0);
  chrB->setValue(0);
  chrC->setValue(0);
  chrC->setValue(0);
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMaxPreferred(0x12);

  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  // put your main code here, to run repeatedly:
  TouchPoint_t pos = M5.Touch.getPressPoint();
  if (pos.x != -1) {
    if (!button_state) push_button();
    button_state = true;
  } else {
    if (button_state) not_push_button();
    button_state = false;
  }

  delay(10);
}



// nyaw















