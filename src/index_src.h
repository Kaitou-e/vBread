
#pragma once

// note R"KEYWORD( html page code )KEYWORD"; 

const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Hello Kitty Form</title>
  <style>
    body {
      background: linear-gradient(135deg, #ffd6e7, #fff0f6);
      color: #5a2a3a;
      font-family: Arial, sans-serif;
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      margin: 0;
      padding: 20px;
      box-sizing: border-box;
    }

    .form-container {
      background: linear-gradient(180deg, #fff7fb, #ffe4f0);
      padding: 22px 30px;
      border-radius: 18px;
      box-shadow: 0 8px 30px rgba(255, 105, 180, 0.25);
      width: min(950px, 100%);
      border: 2px solid #ffb6d5;
    }

    .form-header {
      margin-bottom: 14px;
      font-size: 18px;
      text-align: center;
      color: #d94f8a;
      font-weight: bold;
    }

    .status {
      margin-bottom: 12px;
      text-align: center;
      font-size: 14px;
      color: #c05a86;
      min-height: 1.2em;
      font-weight: bold;
    }

    .editor-layout {
      display: flex;
      gap: 20px;
      align-items: stretch;
    }

    .editor-column {
      flex: 1;
      min-width: 0;
    }

    textarea {
      width: 100%;
      height: 30em;
      resize: vertical;
      padding: 12px;
      border: 2px solid #ffb6d5;
      border-radius: 14px;
      background-color: #fffafd;
      color: #5a2a3a;
      font-size: 14px;
      box-sizing: border-box;
      font-family: monospace;
      outline: none;
    }

    textarea:focus {
      border-color: #ff6fae;
      box-shadow: 0 0 0 3px rgba(255, 111, 174, 0.2);
    }

    .calculator {
      width: 260px;
      flex: 0 0 260px;
      background: #fffafd;
      border: 2px solid #ffb6d5;
      border-radius: 14px;
      padding: 14px;
      box-sizing: border-box;
    }

    .calculator-title {
      text-align: center;
      color: #d94f8a;
      font-weight: bold;
      margin-bottom: 10px;
    }

    .key-display {
      background: #ffe4f0;
      border: 2px solid #ffb6d5;
      border-radius: 10px;
      padding: 10px;
      margin-bottom: 14px;
      text-align: center;
      color: #8d315a;
    }

    .key-display .value {
      display: block;
      font-family: monospace;
      font-size: 26px;
      font-weight: bold;
      color: #d94f8a;
      margin-top: 4px;
    }

    .button-grid {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 10px;
    }

    .key-button {
      width: auto;
      aspect-ratio: 1;
      padding: 4px;
      border: 2px solid #ffb6d5;
      border-radius: 12px;
      background: linear-gradient(180deg, #ffd6e7, #ffb6d5);
      color: #8d315a;
      font-size: 14px;
      cursor: pointer;
      box-shadow: 0 3px #d94f8a;
      font-weight: bold;
    }

    .key-button:hover {
      background: linear-gradient(180deg, #ffe6f0, #ffc6df);
    }

    .key-button.active {
      background: linear-gradient(180deg, #ff77b5, #f14b96);
      border-color: #c92f70;
      color: white;
      box-shadow: 0 1px #a9235d;
      transform: translateY(2px);
    }

    .calculator-actions {
      display: flex;
      gap: 10px;
      margin-top: 14px;
    }

    .calculator-actions button {
      width: auto;
      flex: 1;
      padding: 9px;
      font-size: 13px;
      box-shadow: 0 3px #d94f8a;
    }

    .submit-button {
      width: 100%;
      padding: 12px;
      border: none;
      border-radius: 14px;
      background: linear-gradient(180deg, #ff8fc1, #ff5fa2);
      color: #ffffff;
      font-size: 16px;
      cursor: pointer;
      box-shadow: 0 4px #d94f8a;
      transition: all 0.1s ease-in-out;
      font-weight: bold;
      margin-top: 20px;
    }

    .submit-button:hover {
      background: linear-gradient(180deg, #ff9fcb, #ff6bb0);
    }

    .submit-button:active,
    .calculator-actions button:active {
      transform: translateY(2px);
      box-shadow: 0 2px #d94f8a;
    }

    @media (max-width: 750px) {
      .editor-layout {
        flex-direction: column;
      }

      .calculator {
        width: 100%;
        flex-basis: auto;
      }
    }
  </style>
</head>

<body onload="process(); loadNames();">
  <div class="form-container">
    <div class="form-header">Edit names map JSON</div>
    <div class="status" id="status"></div>

    <div class="editor-layout">
      <div class="editor-column">
        <textarea id="textInput" rows="30" cols="50"
          placeholder='Example: { "1": "Angry", "3": "Combo hotkey" }'></textarea>
      </div>

      <div class="calculator">
        <div class="calculator-title">Button Key Calculator</div>

        <div class="key-display">
          JSON key for selected buttons:
          <span class="value" id="keyValue">0</span>
        </div>

        <div class="button-grid">
          <button type="button" class="key-button" data-bit="0" onclick="toggleKey(this)">1<br>bit 0</button>
          <button type="button" class="key-button" data-bit="1" onclick="toggleKey(this)">2<br>bit 1</button>
          <button type="button" class="key-button" data-bit="2" onclick="toggleKey(this)">4<br>bit 2</button>

          <button type="button" class="key-button" data-bit="3" onclick="toggleKey(this)">8<br>bit 3</button>
          <button type="button" class="key-button" data-bit="4" onclick="toggleKey(this)">16<br>bit 4</button>
          <button type="button" class="key-button" data-bit="5" onclick="toggleKey(this)">32<br>bit 5</button>

          <button type="button" class="key-button" data-bit="6" onclick="toggleKey(this)">64<br>bit 6</button>
          <button type="button" class="key-button" data-bit="7" onclick="toggleKey(this)">128<br>bit 7</button>
          <button type="button" class="key-button" data-bit="8" onclick="toggleKey(this)">256<br>bit 8</button>
        </div>

        <div class="calculator-actions">
          <button type="button" onclick="copyKey()">Copy key</button>
          <button type="button" onclick="clearKeys()">Clear</button>
        </div>
      </div>
    </div>

    <button type="button" class="submit-button" id="submitBtn" onclick="submitForm()">Submit</button>
  </div>

  <script type="text/javascript">
    var xmlHttp = new XMLHttpRequest();
    var selectedButtonMask = 0;

    function setStatus(msg, good) {
      const el = document.getElementById("status");
      el.textContent = msg;
      el.style.color = good ? "#d94f8a" : "#ff4f87";
    }

    function updateKeyDisplay() {
      document.getElementById("keyValue").textContent = selectedButtonMask;
    }

    function toggleKey(button) {
      const bit = Number(button.dataset.bit);
      const mask = 1 << bit;

      selectedButtonMask ^= mask;
      button.classList.toggle("active");
      updateKeyDisplay();
    }

    function clearKeys() {
      selectedButtonMask = 0;
      document.querySelectorAll(".key-button.active").forEach(function (button) {
        button.classList.remove("active");
      });
      updateKeyDisplay();
    }

    function copyKey() {
      const key = String(selectedButtonMask);

      if (navigator.clipboard && navigator.clipboard.writeText) {
        navigator.clipboard.writeText(key)
          .then(function () {
            setStatus("Copied JSON key: " + key, true);
          })
          .catch(function () {
            setStatus("Key is: " + key, true);
          });
      } else {
        setStatus("Key is: " + key, true);
      }
    }

    function response() {
      if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
        console.log("Response from server:", xmlHttp.responseXML);
      }
    }

    function loadNames() {
      var xhttp = new XMLHttpRequest();

      xhttp.onreadystatechange = function () {
        if (xhttp.readyState == 4) {
          if (xhttp.status == 200) {
            document.getElementById("textInput").value = xhttp.responseText;
            setStatus("Loaded names from server", true);
          } else {
            setStatus("Failed to load names", false);
            console.log("Failed to load names, status:", xhttp.status);
          }
        }
      };

      xhttp.open("GET", "getnames", true);
      xhttp.send();
    }

    function submitForm() {
      var xhttp = new XMLHttpRequest();
      var textValue = encodeURIComponent(document.getElementById("textInput").value);

      xhttp.onreadystatechange = function () {
        if (xhttp.readyState == 4) {
          if (xhttp.status == 200 && xhttp.responseText.trim() == "OK") {
            setStatus("Names updated successfully", true);
          } else {
            setStatus("Update failed: " + xhttp.responseText, false);
          }
        }
      };

      xhttp.open("PUT", "submit?VALUE=" + textValue, true);
      xhttp.send();
    }

    function process() {
      if (xmlHttp.readyState == 0 || xmlHttp.readyState == 4) {
        xmlHttp.open("PUT", "xml", true);
        xmlHttp.onreadystatechange = response;
        xmlHttp.send(null);
      }

      setTimeout(process, 200);
    }
  </script>
</body>

</html>
)=====";