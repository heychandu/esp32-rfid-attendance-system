# 📡 ESP32 RFID Attendance System

> Developed by **Chandu R**

---

## 📌 Overview

This project is a **smart RFID-based attendance system** built using ESP32, developed progressively from a basic RFID reader into a **full cloud-connected attendance solution**.

Each version adds new features, improving functionality, user interaction, and real-world usability.

---

## 🚀 Features

* 📡 RFID Card Detection
* 📟 LCD Display Interface
* 🔊 Buzzer Feedback
* 💡 LED Indication
* 🔁 IN / OUT Attendance Logic
* 🧠 UID-based User Identification
* 🌐 Cloud Integration (Google Sheets)
* 🔘 Mode Switching (Attendance / Register)
* ⚙️ Modular & Scalable Design

---

## 🧠 System Workflow

👉 Scan Card → 🔍 Read UID → 👤 Identify User → 🌐 Send Data → 📊 Process Attendance → 📟 Display Result → 🔊 Feedback

---

## 🧩 Hardware Used

* ESP32
* MFRC522 RFID Module
* I2C LCD (16x2)
* Push Button
* Buzzer
* LED

---

## ⚙️ Technologies Used

* Embedded C/C++ (Arduino IDE)
* SPI Communication (RFID)
* I2C Communication (LCD)
* WiFi Communication (ESP32)
* Google Apps Script (Backend)
* Google Sheets (Cloud Database)

---

## 📁 Project Structure

```
rfid-attendance-system/
├── V1_basic_read/
├── V2_lcd_feedback/
├── V3_attendance_logic/
├── V4_cloud_system/
├── V5_smart_system/
```

---

## 🧪 Project Versions

### 🟢 V1 – Basic RFID Reader

* Reads RFID UID
* Displays output in Serial Monitor

---

### 🟡 V2 – LCD + Feedback

* LCD display
* Buzzer + LED indication
* Improved user interaction

---

### 🔵 V3 – Attendance Logic

* UID-based user identification
* IN / OUT attendance tracking
* Duplicate scan handling

---

### 🟣 V4 – Cloud Attendance System

* WiFi-based Google Sheets integration
* Real-time attendance logging
* Automatic Time In / Time Out
* Removed local data storage

---

### 🔴 V5 – Smart Attendance System

* Register mode for new cards
* Button-based mode switching
* LED + buzzer feedback system
* Clean UI (no flicker, no debug text)
* Optimized cloud communication
* Improved system usability

---

## 📸 Demo

*(Images / demo video will be added soon)*

---

## 🔮 Future Scope

* 📱 Mobile App Interface
* 📊 Web Dashboard for Attendance
* ☁️ Firebase / Real-time Database
* 🧠 Face Recognition Integration
* 🔐 Secure Authentication System

---

## 👨‍💻 Author

**Chandu R**
🔗 GitHub: [@heychandu](https://github.com/heychandu)
