#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- PINS ----------------
#define SS_PIN 5
#define RST_PIN 4

#define GREEN_LED 26
#define BUZZER 25

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- KNOWN USERS ----------------
String userUIDs[] = {"12486A05", "146817CE"};
String userNames[] = {"Student", "Student2"};
bool attendanceStatus[] = {false, false};  // false = OUT, true = IN

int totalUsers = 2;

// ---------------- FUNCTIONS ----------------
String getUID() {
  String uid = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }

  uid.toUpperCase();
  return uid;
}

void successBeep() {
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
  digitalWrite(GREEN_LED, LOW);
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  SPI.begin(18, 19, 23, 5);
  mfrc522.PCD_Init();

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.print("Tap your card");
}

// ---------------- LOOP ----------------
void loop() {

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  String uid = getUID();
  Serial.println(uid);

  lcd.clear();

  bool found = false;

  for (int i = 0; i < totalUsers; i++) {
    if (uid == userUIDs[i]) {

      found = true;

      if (!attendanceStatus[i]) {
        lcd.print(userNames[i]);
        lcd.setCursor(0, 1);
        lcd.print("IN Marked");
        attendanceStatus[i] = true;
      } else {
        lcd.print(userNames[i]);
        lcd.setCursor(0, 1);
        lcd.print("OUT Marked");
        attendanceStatus[i] = false;
      }

      successBeep();
      break;
    }
  }

  if (!found) {
    lcd.print("Invalid Card");
  }

  delay(2000);

  lcd.clear();
  lcd.print("Tap your card");
}