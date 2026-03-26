#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- PIN CONFIG ----------------
#define SS_PIN 5
#define RST_PIN 4   // Final system pin

#define GREEN_LED 26
#define BUZZER    25

#define SDA_PIN 21
#define SCL_PIN 22

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);

  // -------- SPI (RFID) --------
  SPI.begin(18, 19, 23, 5);
  mfrc522.PCD_Init();

  // -------- I2C (LCD) --------
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  // -------- OUTPUT PINS --------
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // -------- START MESSAGE --------
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
}

void loop() {

  // -------- CHECK FOR CARD --------
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.println("Card Detected");

  // -------- LCD DISPLAY --------
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card Detected");

  // -------- SUCCESS FEEDBACK --------
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(200);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BUZZER, LOW);

  delay(1000);

  // -------- RESET SCREEN --------
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
}