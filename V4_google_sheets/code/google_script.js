/**
 * 📡 RFID Attendance System - Google Apps Script (V4)
 * --------------------------------------------------
 * This script receives UID from ESP32 and updates attendance
 * in Google Sheets with Time In / Time Out logic.
 */

function doGet(e) {

  // ---------------- CONFIGURATION ----------------
  var sheet_id = "YOUR_SPREADSHEET_ID"; // 🔁 Replace with your Google Sheet ID
  var sheet_UD = "User_Data";           // Sheet containing registered users
  var sheet_AT = "Attendance";          // Sheet for attendance logs

  // Open spreadsheet and sheets
  var ss = SpreadsheetApp.openById(sheet_id);
  var userSheet = ss.getSheetByName(sheet_UD);
  var attSheet = ss.getSheetByName(sheet_AT);

  // Get UID from ESP32 request
  var uid = e.parameter.uid;

  // ---------------- DATE & TIME ----------------
  var now = new Date();

  // Format date and time
  var dateStr = Utilities.formatDate(now, "YOUR_TIMEZONE", "dd/MM/yyyy");
  var time = Utilities.formatDate(now, "YOUR_TIMEZONE", "HH:mm:ss");

  // Get all registered users
  var users = userSheet.getDataRange().getValues();

  // ---------------- FIND USER ----------------
  for (var i = 1; i < users.length; i++) {

    // Match UID from sheet with scanned UID
    if (users[i][1].toString().trim() == uid) {

      var name = users[i][0]; // Get user name
      var data = attSheet.getDataRange().getValues(); // Attendance data

      // ---------------- CHECK EXISTING ENTRY ----------------
      for (var j = 1; j < data.length; j++) {

        var rowUID = data[j][1].toString().trim();

        // Convert stored date into same format for comparison
        var rowDate = Utilities.formatDate(
          new Date(data[j][2]),
          "YOUR_TIMEZONE",
          "dd/MM/yyyy"
        );

        // Check if same user and same date
        if (rowUID == uid && rowDate == dateStr) {

          // -------- TIME OUT --------
          if (data[j][4] == "" || data[j][4] == null) {

            // Update Time Out column
            attSheet.getRange(j + 1, 5).setValue(time);

            return ContentService.createTextOutput(
              "OK,TO_Successful," + name + "," + time
            );
          }

          // -------- ALREADY DONE --------
          return ContentService.createTextOutput("OK,atcInf01");
        }
      }

      // -------- TIME IN (NEW ENTRY) --------
      attSheet.appendRow([name, uid, dateStr, time, ""]);

      return ContentService.createTextOutput(
        "OK,TI_Successful," + name + "," + time
      );
    }
  }

  // -------- INVALID CARD --------
  return ContentService.createTextOutput("OK,atcErr01");
}