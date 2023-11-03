#include <Arduino.h>
#include <M5Core2.h>
#include <Wire.h>

bool button_state = false;
void init_button();
void not_push_button();
void push_button();

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
}

void not_push_button() {
  M5.Axp.SetLDOEnable(3, 0);
  M5.Lcd.fillEllipse(160, 120, 90, 90, RED);
  M5.Lcd.setTextSize(14);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(80, 95);
  M5.Lcd.printf("PUSH");
}

#include "Adafruit_PWMServoDriver.h"
#include "MFRC522_I2C.h"

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
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire1);

#define SERVOMIN 102  // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 512  // This is the 'maximum' pulse length count (out of 4096)
#define USMIN \
  500  // This is the rounded 'minimum' microsecond length based on the minimum
       // pulse of 102
#define USMAX \
  5000  // This is the rounded 'maximum' microsecond length based on the maximum
        // pulse of 512
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz updates

#define svcKisoken "BABE"
#define UUIDSTATS "0000"
#define UUIDOrder "00DD"
#define UUIDCommand "CCDD"
#define UUIDA "AAAA"
#define UUIDB "BBBB"
#define UUIDC "CCCC"
#define UUIDD "DDDD"
#define UUIDE "EEEE"
#define UUIDCOUNT "CC00"
#define UUIDDISPOSE "DD00"

#define Catcher 0
#define Lifter 1
#define Sashi 2
#define CatcherExtend 70
#define LifterExtend 80
#define SashiExtend 150

MFRC522 mfrc522(0x28);  // Create MFRC522 instance.
void setServoPulse(uint8_t n, double pulse);
void servo_angle_write(uint8_t n, int Angle);
void ShowReaderDetails();

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *chrStatus;
BLECharacteristic *chrCommand;
BLECharacteristic *chrDispose;
BLECharacteristic *chrOrder;
BLECharacteristic *chrA;
BLECharacteristic *chrB;
BLECharacteristic *chrC;
BLECharacteristic *chrD;
BLECharacteristic *chrE;

class MyServerCallbacks : public BLEServerCallbacks {
 public:
  void onConnect(BLEServer *pServer) {
    Serial.println("クライアントが接続しました");
    NimBLEDevice::startAdvertising();
  }

  void onDisconnect(BLEServer *pServer) {
    Serial.println("クライアントが切断されました");
  }
};

void JumpArmSync(int X, int Y, int Z, int R);
void MoveArmSync(int X, int Y, int Z, int R);
void ArmOrientation(bool IsR);

void InitialPose();
void CheckOrder();
void PickSushi(int SushiNum);
void PickSara();
void PutSushiToSara();
void PutSushiToLane();
void CheckDispose();
void DisposeSushi();
void ResetAllServo() {
  servo_angle_write(Catcher, 0);
  servo_angle_write(Lifter, 0);
  servo_angle_write(Sashi, 0);
}

void ServoTest() {
  ResetAllServo();
  // Pick Sushi
  servo_angle_write(Sashi, 0);
  delay(500);
  servo_angle_write(Lifter, LifterExtend);
  delay(500);
  servo_angle_write(Catcher, CatcherExtend);
  delay(500);
  // Pick Sara
  servo_angle_write(Lifter, 0);
  delay(500);
  servo_angle_write(Sashi, SashiExtend);
  delay(500);
  // Put Sushi To Sara

  servo_angle_write(Catcher, 0);
  delay(500);

  // Put Sushi To Lane
  servo_angle_write(Sashi, 0);
  delay(2000);

  // Dispose Sushi
  servo_angle_write(Catcher, 0);
  servo_angle_write(Lifter, 0);
  delay(500);
  servo_angle_write(Sashi, SashiExtend);
  delay(500);
  servo_angle_write(Sashi, 0);
  delay(500);
}

void act(int SushiNum) {
  // int SushiNum = 3;
  InitialPose();
  PickSushi(SushiNum);
  PickSara();
  PutSushiToSara();
  PutSushiToLane();
  // DisposeSushi();
}

void Dispose() {
  InitialPose();
  DisposeSushi();
  InitialPose();
}
void InitialPose() {
  ArmOrientation(1);
  JumpArmSync(170, 0, 230, 0);  // 初期位置くん
  ResetAllServo();
}

void CheckDispose() {
  int OrderVal = chrOrder->getValue();
  Serial.print("ifDispose");
  Serial.println(OrderVal);
  if (OrderVal != -1) {
    act(OrderVal) chrDispose->setValue(-1);
  }
}
void PickSushi(int SushiNum) {
  ArmOrientation(1);
  // JumpArmSync(int(-39 + 52.5 * SushiNum), 290, 52, 90);
  JumpArmSync(int(-39 + 52.5 * SushiNum), 300, 52, 90);
  // JumpArmSync(39, 290, 52, 90);
  // M5.Lcd.fillScreen(RED);
  servo_angle_write(Sashi, 0);
  delay(500);
  servo_angle_write(Lifter, LifterExtend);
  delay(500);
  servo_angle_write(Catcher, CatcherExtend);
  delay(500);
  // servo_angle_write()
}
void PickSara() {
  servo_angle_write(Lifter, 0);
  delay(500);
  ArmOrientation(0);
  JumpArmSync(219, 309, 209, 100);  // 皿サーバー
  ArmOrientation(0);
  MoveArmSync(219, -203, 209, 100);  // 皿サーバー
  ArmOrientation(0);
  JumpArmSync(206, -203, 58, -180);  // 皿サーバー
  servo_angle_write(Sashi, SashiExtend);
  delay(500);
}
void PutSushiToSara() {
  servo_angle_write(Catcher, 0);
  delay(500);
}
void PutSushiToLane() {
  ArmOrientation(1);
  JumpArmSync(370, -72, 122, -270);
  servo_angle_write(Sashi, 0);
  delay(500);
  ArmOrientation(1);
  MoveArmSync(370, -72, 230, -270);
  ArmOrientation(1);
  JumpArmSync(170, 0, 230, -270);  // 初期位置くん
}
void CheckDispose() {
  String ifDispose = String(chrDispose->getValue());
  Serial.print("ifDispose");
  Serial.println(ifDispose);
  if (ifDispose == "DISPOSE") {
    Dispose();
    chrDispose->setValue("NO");
  }
}

void DisposeSushi() {
  servo_angle_write(Catcher, 0);
  servo_angle_write(Lifter, 0);
  delay(500);
  ArmOrientation(1);
  MoveArmSync(368, 11, 173, 90);
  servo_angle_write(Sashi, SashiExtend);
  delay(500);
  ArmOrientation(0);
  JumpArmSync(127, -346, 166, -90);
  servo_angle_write(Sashi, 0);
  delay(500);
  InitialPose();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  M5.begin();

  M5.Lcd.begin();
  init_button();

  Wire.begin();
  M5.begin();
  // M5.begin(true, true, true, false, kMBusModeInput);
  // Wire1.begin(21, 22);

  pwm.begin();
  pwm.setPWMFreq(50);

  BLEDevice::init("KisokenC2023");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  pService = pServer->createService(svcKisoken);
  chrStatus = pService->createCharacteristic(
      UUIDSTATS, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ);
  chrCommand = pService->createCharacteristic(
      UUIDCommand, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  chrDispose =
      pService->createCharacteristic(UUIDDISPOSE, NIMBLE_PROPERTY::WRITE);
  chrOrder = pService->createCharacteristic(UUIDOrder, NIMBLE_PROPERTY::WRITE);
  chrA = pService->createCharacteristic(UUIDA, NIMBLE_PROPERTY::READ);
  chrB = pService->createCharacteristic(UUIDB, NIMBLE_PROPERTY::READ);
  chrC = pService->createCharacteristic(UUIDC, NIMBLE_PROPERTY::READ);
  chrD = pService->createCharacteristic(UUIDD, NIMBLE_PROPERTY::READ);
  chrE = pService->createCharacteristic(UUIDE, NIMBLE_PROPERTY::READ);

  chrStatus->setValue("YET");
  chrCommand->setValue("Ping");
  chrDispose->setValue("NO");
  chrDispose->setValue(0);
  chrA->setValue(0);
  chrB->setValue(0);
  chrC->setValue(0);
  chrD->setValue(0);
  chrE->setValue(0);
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(svcKisoken);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMaxPreferred(0x12);

  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");

  ResetAllServo();
}

void loop() {
  // put your main code here, to run repeatedly:
  TouchPoint_t pos = M5.Touch.getPressPoint();
  if (pos.x != -1) {
    if (!button_state) {
      push_button();
      // act();
      ServoTest();
    }
    button_state = true;
  } else {
    if (button_state) {
      not_push_button();
      delay(500);
      InitialPose();
    }
    button_state = false;
  }
  CheckDispose();
  // Now a card is selected. The UID and SAK is in mfrc522.uid.

  // Dump UID
  Serial.println();

  // for (int i = 0; i < 16; i++) {
  // setServoPulse(i, 2.5);
  // }
  delay(500);
  // delay(10);
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

void JumpArmSync(int X, int Y, int Z, int R) {
  Serial.print("Jump Arm:");
  Serial.print(X);
  Serial.print(", ");
  Serial.print(Y);
  Serial.print(", ");
  Serial.print(Z);
  Serial.print(", ");
  Serial.print(R);

  chrCommand->setValue("JumpTo");
  chrA->setValue(X);
  chrB->setValue(Y);
  chrC->setValue(Z);
  chrD->setValue(R);
  delay(10);
  chrCommand->notify();

  String Stats = "YET";
  while (Stats == "YET") {
    Stats = String(chrStatus->getValue());
    Serial.print("Waiting!! Status: ");
    Serial.println(Stats);
    delay(100);
  }
  chrStatus->setValue("YET");
}

void MoveArmSync(int X, int Y, int Z, int R) {
  Serial.print("Move Arm:");
  Serial.print(X);
  Serial.print(", ");
  Serial.print(Y);
  Serial.print(", ");
  Serial.print(Z);
  Serial.print(", ");
  Serial.print(R);

  chrCommand->setValue("GoTo");
  chrA->setValue(X);
  chrB->setValue(Y);
  chrC->setValue(Z);
  chrD->setValue(R);
  delay(10);
  chrCommand->notify();

  String Stats = "YET";
  while (Stats == "YET") {
    Stats = String(chrStatus->getValue());
    Serial.print("Waiting!! Status: ");
    Serial.println(Sta