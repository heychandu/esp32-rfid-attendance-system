#include <SPI.h>        // Library for SPI communication
#include <MFRC522.h>    // Library for RFID module

// ---------------- PIN DEFINITIONS ----------------
#define SS_PIN 5        // SDA / SS pin connected to GPIO 5
#define RST_PIN 22      // Reset pin connected to GPIO 22

// Create RFID object
MFRC522 mfrc522(SS_PIN, RST_PIN);

// ---------------- SETUP FUNCTION ----------------
void setup() {
  Serial.begin(115200);   // Start Serial communication (for output display)

  // Initialize SPI communication
  // Format: SPI.begin(SCK, MISO, MOSI, SS)
  SPI.begin(18, 19, 23, 5);

  // Initialize RFID module
  mfrc522.PCD_Init();

  // Print message to Serial Monitor
  Serial.println("Place your RFID card...");
}

// ---------------- LOOP FUNCTION ----------------
void loop() {

  // Step 1: Check if a new RFID card is present
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;   // If no card detected, exit loop and check again
  }

  // Step 2: Try to read the card's UID
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;   // If reading fails, exit loop
  }

  // Step 3: Print UID to Serial Monitor
  Serial.print("UID: ");

  // UID is stored as multiple bytes → loop through each byte
  for (byte i = 0; i < mfrc522.uid.size; i++) {

    // Print each byte in HEX format
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");  // Add space for readability
  }

  Serial.println();  // Move to next line

  // Step 4: Small delay to avoid multiple readings of same card
  delay(800);
}