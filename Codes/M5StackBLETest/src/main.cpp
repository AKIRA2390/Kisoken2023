#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <M5Unified.h>

BLEServer* pServer = NULL;
BLEService* pService = NULL;
BLECharacteristic* pCharacteristic = NULL;

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  BLEDevice::init("MyDevice");
  pServer = BLEDevice::createServer();
  pService = pServer->createService(BLEUUID((uint16_t)0x180D));

  pCharacteristic = pService->createCharacteristic(
      BLEUUID((uint16_t)0x2A37),
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  pServer->getAdvertising()->start();
}

void loop() {
  // put your main code here, to run repeatedly:
  pCharacteristic->setValue("nya!");  // 送信するデータ
  pCharacteristic->notify();
  M5.update();
}
