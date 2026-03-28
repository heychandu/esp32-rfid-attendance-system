/**
 * ============================================================
 * 📡 RFID Attendance System (V5 - ESP32 + Google Sheets)
 * ============================================================
 */

#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

// ---------------- PIN CONFIG ----------------
#define SS_PIN 5
#define RST_PIN 4
#define BTN_PIN 15

#define GREEN_LED 26
#define RED_LED 27
#define BUZZER 25

// ---------------- WIFI CONFIG ----------------
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ---------------- GOOGLE SCRIPT ----------------
String WEB_APP_URL = "YOUR_WEB_APP_URL";

// ---------------- OBJECTS ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);

// ---------------- VARIABLES ----------------
String mode = "atc";   // "atc" = Attendance, "reg" = Register
String uidStr = "";

// ============================================================
// 🔔 FEEDBACK FUNCTIONS
// ============================================================
void successBeep() {
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
  digitalWrite(GREEN_LED, LOW);
}

void errorBeep() {
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(400);
  digitalWrite(BUZZER, LOW);
  digitalWrite(RED_LED, LOW);
}

// ============================================================
// 🔍 READ RFID UID
// ============================================================
String getUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  return uid;
}

// ============================================================
// 🌐 SEND DATA TO GOOGLE SHEETS
// ============================================================
String sendRequest(String uid) {

  if (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    lcd.print("WiFi Error");
    errorBeep();
    delay(1000);
    return "";
  }

  HTTPClient http;

  String url = WEB_APP_URL + "?sts=" + mode + "&uid=" + uid;

  http.begin(url);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  int httpCode = http.GET();
  String payload = "";

  if (httpCode > 0) payload = http.getString();

  http.end();
  return payload;
}

// ============================================================
// ⚙️ SETUP
// ============================================================
void setup() {

  Serial.begin(115200);
  delay(1000);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  SPI.begin();
  rfid.PCD_Init();

  // -------- WIFI CONNECT --------
  lcd.print("Connecting WiFi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    retry++;
  }

  lcd.clear();

  if (WiFi.status() == WL_CONNECTED) {
    lcd.print("WiFi Connected");
  } else {
    lcd.print("WiFi Failed");
  }

  delay(1500);

  lcd.clear();
  lcd.print("Ready to Use");
}

// ============================================================
// 🔄 LOOP
// ============================================================
void loop() {

  // -------- BUTTON: MODE SWITCH --------
  if (digitalRead(BTN_PIN) == LOW) {
    delay(300);

    mode = (mode == "atc") ? "reg" : "atc";

    lcd.clear();
    lcd.print(mode == "atc" ? "Attendance Mode" : "Register Mode");
    delay(1200);
  }

  // -------- RFID DETECTION --------
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  uidStr = getUID();

  lcd.clear();
  lcd.print("Processing...");

  String response = sendRequest(uidStr);
  response.trim();

  Serial.println(response);

  // ================= REGISTER MODE =================
  if (mode == "reg") {

    lcd.clear();

    if (response.indexOf("R_Successful") >= 0) {
      lcd.print("Registered");
      successBeep();
    }
    else if (response.indexOf("regErr01") >= 0) {
      lcd.print("Already Exists");
      errorBeep();
    }

    delay(2000);

    lcd.clear();
    lcd.print("Register Mode");
    return;
  }

  // ================= ATTENDANCE MODE =================
  if (response.indexOf("TI_Successful") >= 0 ||
      response.indexOf("TO_Successful") >= 0) {

    int p1 = response.indexOf(",");
    int p2 = response.indexOf(",", p1 + 1);

    String status = response.substring(0, p1);
    String name   = response.substring(p1 + 1, p2);
    String time   = response.substring(p2 + 1);

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(name.substring(0, 16));

    lcd.setCursor(0, 1);

    if (status == "TI_Successful") {
      lcd.print("IN: ");
      lcd.print(time);
      successBeep();
    } else {
      lcd.print("OUT: ");
      lcd.print(time);
      successBeep();
    }
  }
  else if (response.indexOf("atcInf01") >= 0) {

    lcd.clear();
    lcd.print("Already Done");
    errorBeep();
  }
  else if (response.indexOf("atcErr01") >= 0) {

    lcd.clear();
    lcd.print("Invalid Card");
    errorBeep();
  }
  else {

    lcd.clear();
    lcd.print("Server Error");
    errorBeep();
  }

  delay(2500);

  lcd.clear();
  lcd.print("Tap your card");
}