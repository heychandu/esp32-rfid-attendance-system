/**
 * 📡 RFID Attendance System (V4 - Cloud)
 * --------------------------------------
 * ESP32 reads RFID card UID and sends it to Google Sheets.
 * Displays Name + Time (IN/OUT) on 16x2 LCD.
 */

#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

// ---------------- WIFI CONFIG ----------------
const char* ssid = "YOUR_WIFI_SSID";         // 🔁 Replace with your WiFi name
const char* password = "YOUR_WIFI_PASSWORD"; // 🔁 Replace with your WiFi password

// 🔁 Replace with your Google Apps Script Web App URL
String Web_App_URL = "YOUR_WEB_APP_URL";

// ---------------- RFID CONFIG ----------------
#define SS_PIN 5
#define RST_PIN 4

MFRC522 mfrc522(SS_PIN, RST_PIN);

// ---------------- LCD CONFIG ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- FUNCTION: GET UID ----------------
String getUID() {
  String uid = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }

  uid.toUpperCase();
  return uid;
}

// ---------------- FUNCTION: SEND REQUEST ----------------
String sendRequest(String uid) {

  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    lcd.print("WiFi Error");
    delay(1500);
    return "";
  }

  HTTPClient http;

  // Build request URL
  String url = Web_App_URL + "?sts=atc&uid=" + uid;

  http.begin(url);

  // 🔥 Required for Google redirect
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  int httpCode = http.GET();

  Serial.print("HTTP Code: ");
  Serial.println(httpCode);

  String payload = "";

  if (httpCode > 0) {
    payload = http.getString();
  }

  http.end();
  return payload;
}

// ---------------- SETUP ----------------
void setup() {

  Serial.begin(115200);

  // Initialize SPI and RFID
  SPI.begin(18, 19, 23, 5);
  mfrc522.PCD_Init();

  // Initialize LCD
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  lcd.print("Connecting WiFi");

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  lcd.clear();
  lcd.print("Ready");
  delay(1000);

  lcd.clear();
  lcd.print("Tap Card");
}

// ---------------- LOOP ----------------
void loop() {

  // Wait for RFID card
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  String uid = getUID();
  Serial.println("UID: " + uid);

  lcd.clear();
  lcd.print("Processing...");

  // Send UID to server
  String response = sendRequest(uid);
  response.trim(); // Remove unwanted characters

  Serial.println("RAW RESPONSE:");
  Serial.println("[" + response + "]");

  lcd.clear();

  // ---------------- RESPONSE HANDLING ----------------
  if (response.startsWith("OK")) {

    // -------- TIME IN --------
    if (response.indexOf("TI_Successful") >= 0) {

      int p1 = response.indexOf(",");
      int p2 = response.indexOf(",", p1 + 1);
      int p3 = response.indexOf(",", p2 + 1);

      String name = response.substring(p2 + 1, p3);
      String time = response.substring(p3 + 1);

      lcd.setCursor(0, 0);
      lcd.print(name.substring(0, 16));

      lcd.setCursor(0, 1);
      lcd.print("IN: ");
      lcd.print(time);
    }

    // -------- TIME OUT --------
    else if (response.indexOf("TO_Successful") >= 0) {

      int p1 = response.indexOf(",");
      int p2 = response.indexOf(",", p1 + 1);
      int p3 = response.indexOf(",", p2 + 1);

      String name = response.substring(p2 + 1, p3);
      String time = response.substring(p3 + 1);

      lcd.setCursor(0, 0);
      lcd.print(name.substring(0, 16));

      lcd.setCursor(0, 1);
      lcd.print("OUT: ");
      lcd.print(time);
    }

    // -------- ALREADY DONE --------
    else if (response.indexOf("atcInf01") >= 0) {
      lcd.setCursor(0, 0);
      lcd.print("Already Done");
    }

    // -------- INVALID CARD --------
    else if (response.indexOf("atcErr01") >= 0) {
      lcd.setCursor(0, 0);
      lcd.print("Invalid Card");
      Serial.println("Add UID: " + uid);
    }

  } else {
    lcd.clear();
    lcd.print("Server Error");
  }

  delay(3000);

  lcd.clear();
  lcd.print("Tap Card");
}