/**
 * ============================================================
 * 📡 RFID Attendance System (V5 - Google Apps Script Backend)
 * ============================================================
 * Sheet Structure:
 * 
 * 📄 User_Data   → Name | UID
 * 📄 Attendance  → Name | UID | Date | Time In | Time Out
 *
 * Modes:
 * - sts=reg → Register Mode (Add new UID)
 * - sts=atc → Attendance Mode (IN / OUT tracking)
 *
 * ------------------------------------------------------------
 * ⚠️ Before Deployment:
 * - Replace YOUR_SPREADSHEET_ID
 * - Deploy as Web App (Anyone access)
 * ============================================================
 */

function doGet(e) {

  // ---------------- VALIDATION ----------------
  if (!e.parameter) {
    return ContentService.createTextOutput("ERROR");
  }

  // ---------------- CONFIG ----------------
  var SHEET_ID   = "YOUR_SPREADSHEET_ID";
  var USER_SHEET = "User_Data";
  var ATT_SHEET  = "Attendance";

  var ss = SpreadsheetApp.openById(SHEET_ID);
  var userSheet = ss.getSheetByName(USER_SHEET);
  var attSheet  = ss.getSheetByName(ATT_SHEET);

  // ---------------- PARAMETERS ----------------
  var mode = e.parameter.sts;   // "reg" or "atc"
  var uid  = e.parameter.uid;

  // ---------------- DATE & TIME ----------------
  var now  = new Date();
  var DATE = Utilities.formatDate(now, "Asia/Kolkata", "dd/MM/yyyy");
  var TIME = Utilities.formatDate(now, "Asia/Kolkata", "HH:mm:ss");

  var users = userSheet.getDataRange().getValues();

  // ============================================================
  // ===================== REGISTER MODE =========================
  // ============================================================
  if (mode === "reg") {

    // 🔍 Check if UID already exists
    for (var i = 1; i < users.length; i++) {
      if (users[i][1] == uid) {
        return ContentService.createTextOutput("regErr01");
      }
    }

    // ➕ Add new UID (Name can be edited later)
    userSheet.appendRow(["New User", uid]);

    return ContentService.createTextOutput("R_Successful");
  }

  // ============================================================
  // ==================== ATTENDANCE MODE ========================
  // ============================================================
  if (mode === "atc") {

    var name = "";

    // 🔍 Find user by UID
    for (var i = 1; i < users.length; i++) {
      if (users[i][1] == uid) {
        name = users[i][0];
        break;
      }
    }

    // ❌ Invalid card
    if (name === "") {
      return ContentService.createTextOutput("atcErr01");
    }

    var data = attSheet.getDataRange().getDisplayValues();

    // 🔁 Check existing attendance for today
    for (var i = 1; i < data.length; i++) {

      if (data[i][1] == uid && data[i][2] == DATE) {

        // 🟢 TIME OUT
        if (data[i][4] == "") {
          attSheet.getRange(i + 1, 5).setValue(TIME);

          return ContentService.createTextOutput(
            "TO_Successful," + name + "," + TIME
          );
        }

        // ⚠️ Already completed
        return ContentService.createTextOutput("atcInf01");
      }
    }

    // 🟢 TIME IN (new entry)
    attSheet.insertRows(2);
    attSheet.getRange("A2").setValue(name);
    attSheet.getRange("B2").setValue(uid);
    attSheet.getRange("C2").setValue(DATE);
    attSheet.getRange("D2").setValue(TIME);

    return ContentService.createTextOutput(
      "TI_Successful," + name + "," + TIME
    );
  }
}