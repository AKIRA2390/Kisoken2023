#include <Arduino.h>
#include <M5Core2.h>
#include <Wire.h>

#include "MFRC522_I2C.h"

MFRC522 mfrc522(0x28);  // Create MFRC522 instance.

#define SERVO_ADDR 0x53
void setup() {
  M5.Lcd.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("M5StackFire MFRC522");
  Serial.begin(115200);  // Initialize serial communications with the PC
  Wire.begin();          // Initialize I2C
  M5.begin();
  mfrc522.PCD_Init();   // Init MFRC522
  ShowReaderDetails();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, type, and data blocks..."));
  M5.Lcd.println("Scan PICC to see UID, type, and data blocks...");

  Wire.begin(21, 22, 100000UL);
}
// addr 0x01 mean control the number 1 servo by us
void Servo_write_us(uint8_t number, uint16_t us) {
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x00 | number);
  Wire.write(us & 0x00ff);
  Wire.write(us >> 8 & 0x00ff);
  Wire.endTransmission();
}

// addr 0x11 mean control the number 1 servo by angle
void Servo_write_angle(uint8_t number, uint8_t angle) {
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x10 | number);
  Wire.write(angle);
  Wire.endTransmission();
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
    Serial.println(F(
        "WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}

void loop() {
  static int count = 0;
  // Look for new cards, and select one if present
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(50);

    for (uint8_t i = 0; i < 12; i++) {
      Servo_write_us(i, 2300);
      // Servo_write_angle(i, 180);
    }
    return;
  }

  // Now a card is selected. The UID and SAK is in mfrc522.uid.

  // Dump UID
  Serial.print(F("Card UID:"));
  M5.Lcd.println(" ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    M5.Lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    M5.Lcd.print(mfrc522.uid.uidByte[i], HEX);
    count++;
    if (count >= 10) {
      M5.Lcd.clear();
    }
  }
  Serial.println();
  for (uint8_t i = 0; i < 12; i++) {
    Servo_write_us(i, 700);
    // Servo_write_angle(i, 0);
  }
}
