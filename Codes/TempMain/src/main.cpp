#include <Arduino.h>
#include <M5Core2.h>

#include <Wire.h>

#include "Adafruit_PWMServoDriver.h"
#include "MFRC522_I2C.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire1);

#define SERVOMIN 102  // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 512  // This is the 'maximum' pulse length count (out of 4096)
#define USMIN \
  500  // This is the rounded 'minimum' microsecond length based on the minimum
       // pulse of 102
#define USMAX \
  2500  // This is the rounded 'maximum' microsecond length based on the maximum
        // pulse of 512
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz updates

// 0x28 is i2c address on SDA. Check your address with i2cscanner if not match.
MFRC522 mfrc522(0x28);  // Create MFRC522 instance.
void setServoPulse();
void servo_angle_write();
void ShowReaderDetails();

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
  button_state = true;
  act();
  M5.Axp.SetLDOEnable(3, 1);

  M5.Lcd.fillEllipse(160, 120, 90, 90, ORANGE);
  M5.Lcd.setTextSize(14);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setCursor(80, 95);
  M5.Lcd.printf("PUSH");
}

void not_push_button() {
  button_state = false;

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
#define UUIDCommand "CCDD"
#define UUIDA "AAAA"
#define UUIDB "BBBB"
#define UUIDC "CCCC"
#define UUIDD "DDDD"
#define UUIDCOUNT "CC00"
#define UUIDNotifyer "0000"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *chrCommand;
BLECharacteristic *chrA;
BLECharacteristic *chrB;
BLECharacteristic *chrC;
BLECharacteristic *chrD;


void act(){

  chrCommand->setValue("MOVJ");
  chrA->setValue(0);
  chrB->setValue(0);
  chrC->setValue(0);
  chrD->setValue(0);
  chrCommand->notify();

  delay(2000);


}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  M5.begin(true, true, true, false, kMBusModeInput);
  Wire1.begin(21, 22);

  pwm.begin();
  pwm.setPWMFreq(50);
  M5.Lcd.setCursor(115, 0, 4);
  M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Lcd.print("Servo2");
  mfrc522.PCD_Init();   // Init MFRC522
  ShowReaderDetails();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println("Scan PICC to see UID, type, and data blocks...");
  M5.Lcd.println("Scan PICC to see UID, type, and data blocks...");

  init_button();

  BLEDevice::init("KisokenC2023");
  pServer = BLEDevice::createServer();
  pService = pServer->createService(svcKisoken);
  chrCommand =
      pService->createCharacteristic(UUIDCommand, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  chrA = pService->createCharacteristic(UUIDA, NIMBLE_PROPERTY::READ);
  chrB = pService->createCharacteristic(UUIDB, NIMBLE_PROPERTY::READ);
  chrC = pService->createCharacteristic(UUIDC, NIMBLE_PROPERTY::READ);
  chrD = pService->createCharacteristic(UUIDD, NIMBLE_PROPERTY::READ);

  chrCommand->setValue("MOVJ");
  chrA->setValue(0);
  chrB->setValue(0);
  chrC->setValue(0);
  chrC->setValue(0);
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(
      0x06);  
  pAdvertising->setMaxPreferred(0x12);

  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  // put your main code here, to run repeatedly:
  TouchPoint_t pos = M5.Touch.getPressPoint();
  if (pos.x != -1) {
    if (!button_state) push_button();
  } else {
    if (button_state) not_push_button();    
  }

  delay(10);
}


void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  pulselength = 1000000;  // 1,000,000 us per second
  pulselength /= 50;      // 50 Hz
  Serial.print(pulselength);
  Serial.println(" us per period");
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength);
  Serial.println(" us per bit");
  pulse *= 1000;
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

void servo_angle_write(uint8_t n, int Angle) {
  double pulse = Angle;
  pulse = pulse / 90 + 0.5;
  setServoPulse(n, pulse);
}


void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(
        F("WARNING: Communication failure, is the MFRC522 properly connec"));
  }
}
  












// nyaw