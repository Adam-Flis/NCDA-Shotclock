const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>NCDA Digital Game Controller</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 0;
      background-color: #f5f5f5;
    }

    .navbar {
      background-color: #3f51b5;
      color: white;
      font-size: 20px;
      height: 50px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 0px 20px;
      box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.1);
    }

    .navbar button {
      font-weight: bold;
      font-size: 20px;
      cursor: pointer;
      background: none;
      border: none;
      padding: 0;
      color: white;
    }

    .navbar .home {
      font-size: 30px;
    }

    .section {
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 15px;
      margin-top: 20px;
      transition: all 0.3s ease;
    }

    .section label {
      font-size: 20px;
      font-weight: bold;
      margin-bottom: 10px;
    }

    .section div {
      display: flex;
      justify-content: center;
      gap: 20px;
    }

    .section button {
      background-color: #fff;
      border: none;
      color: black;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 20px;
      cursor: pointer;
      width: 200px;
      border-radius: 5px;
      box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.1);
      transition: all 0.3s ease;
    }

    .section button:hover {
      background-color: #f0f0f0;
    }

    .section button p {
      margin: 0;
      padding: 2px;
      line-height: 1;
    }

    table {
      display: table;
      text-align: center;
      font-size: 20px;
    }

    .number-input {
      display: flex;
      background-color: #fff;
      justify-content: center;
      align-items: center;
      border: none;
      border-radius: 5px;
      padding: 10px;
      box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.1);
      transition: all 0.3s ease;
    }

    .number-input:hover {
      background-color: #f0f0f0;
    }

    .number-input input {
      width: 50px;
      height: 30px;
      background: none;
      text-align: center;
      border: none;
      font-size: 20px;
      -moz-appearance: textfield;
      appearance: textfield;
      outline: none;
    }

    .number-input input::-webkit-inner-spin-button,
    .number-input input::-webkit-outer-spin-button {
      -webkit-appearance: none;
      margin: 0;
    }

    .number-input button {
      width: 30px;
      height: 30px;
      color: black;
      cursor: pointer;
      border: none;
      padding: 0px;
      font-size: 25px;
      display: flex;
      justify-content: center;
      align-items: center;
      background-color: #fff;
      transition: all 0.3s ease;
    }

    .number-input button:hover {
      background-color: #f0f0f0;
    }
    
  </style>
</head>
<body>
  <div class="navbar">
    <button class="home" onclick="location.reload();">&#8962;</button>
    <span class="title">NCDA Digital Game Controller</span>
    <a href="/settings" class="link"><button class="settings-button">&#9965;</button></a>
  </div>

  <div class="section">
    <label>Shot Clockers:</label>
    <div>    
      <a href="/team1" class="link"><button id="team1-btn">
        <p id="team1-name">Team 1</p>
      </button></a>
      <a href="/team2" class="link">      
      <button id="team2-btn">
        <p id="team2-name">Team 2</p>
      </button></a> 
    </div>
  </div>

  <div class="section">
    <label>Referees:</label>
    <div>    
      <a href="/matchclock" class="link"><button id="ref-btn">
        <p>Match Clock</p>
      </button></a>      
    </div>
  </div>

  <div class="section">
    <label>Other:</label>
    <div>    
      <a href="/newgame" class="link"><button id="new-game-btn">
        <p>New Game</p>
      </button></a>     
    </div>
  </div>

  <div class="section">
    <label>Edit Shot Clock Timer:</label>
    <table>
      <tr>
        <td>Min:</td>
        <td>Max:</td>
      </tr>
      <tr>
        <td>
          <div class="number-input">
            <button onclick="decrement('min')" style="margin-right: -20px">&#706;</button>
            <input type="number" id="min" name="min" min="0" max="15" value="10">
            <button onclick="increment('min')" style="margin-left: -20px">&#707;</button>              
          </div>
        </td>
        <td>
          <div class="number-input">
            <button onclick="decrement('max')" style="margin-right: -20px">&#706;</button>
            <input type="number" id="max" name="max" min="0" max="99" value="15">
            <button onclick="increment('max')" style="margin-left: -20px">&#707;</button>  
          </div>
        </td>
      </tr>
    </table>
  </div>

  <script>

    // Function to run on page load
    document.addEventListener('DOMContentLoaded', function() {
      load();
    });

    function load() {
      source = new EventSource(`/events/index`);
      source.addEventListener('update', events);
      setTimeout(function() {  
        updateServer("undefined", "undefined");      
      }, 100);

      // Get all input elements
      var inputs = document.querySelectorAll('input');

      // Add event listeners to each input element
      inputs.forEach(function(input) {      
        // When the input receives focus, remove event listener
        input.addEventListener('focus', function() {
            source.removeEventListener('update', events);
        });

        // When the input loses focus, add event listener
        input.addEventListener('blur', function() {
            checkMinMax();
            source.addEventListener('update', events);
        });      
      });
    }

    function increment(inputId) {
      var input = document.getElementById(inputId);
      input.stepUp();
      checkMinMax();
    }

    function decrement(inputId) {
      var input = document.getElementById(inputId);
      input.stepDown();
      checkMinMax();
    }
    
    // Check min/max 
    function checkMinMax() {
      const minInput = document.getElementById('min');
      const maxInput = document.getElementById('max');   
      let min = parseInt(minInput.value);
      let max = parseInt(maxInput.value);
      if (min > max) {
        minInput.value = max;
        min = max;
      } 

      if (max > 99) {
        max = 99;
        maxInput.value = max;
      } else if (max < 0) {
        max = 0;
        maxInput.value = max;
      } 

      if (min > 99) {
        min = 99;
        minInput.value = min;
      } else if (min < 0) {
        min = 0;
        minInput.value = min;
      }
      minInput.setAttribute('max', max);
      maxInput.setAttribute('min', min);
      updateServer(min, max);
    }

    // Update the server when an action occurs 
    function updateServer(min, max) {
      var side = "index"     
      var xhr = new XMLHttpRequest();
      xhr.open("GET", `/updateServer?side=${side}&min=${min}&max=${max}`, true);    
      xhr.send();
    }

    // Add event listeners 
    function events(e) {
      var response = JSON.parse(e.data);

      // Update shot clockers buttons
      document.getElementById("team1-name").innerHTML = response.team1Name;
      document.getElementById("team2-name").innerHTML = response.team2Name;

      // Update shot clock timer
      document.getElementById("min").value = response.min;
      document.getElementById("max").value = response.max;      
    }

  </script>
</body>
</html>
)rawliteral";

const char shotclock_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title id="title">Team _: Shot Clock</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 0;
      background-color: #f5f5f5;
    }

    .navbar {
      background-color: #3f51b5;
      color: white;
      font-size: 20px;
      height: 50px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 0px 20px;
      box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.1);
    }

    .navbar button {
      font-weight: bold;
      font-size: 20px;
      cursor: pointer;
      background: none;
      border: none;
      padding: 0;
      color: white;
    }

    .content {
      text-align: center;
      margin-top: 20px;
    }

    .settings-overlay {
      position: fixed;
      top: 0;
      right: 0;
      bottom: 0;
      left: 100%;
      background-color: rgba(0, 0, 0, 0.5);
      z-index: 999;
      transition: left 0.3s ease-in-out;
    }

    .settings-overlay.open {
      left: 0;
    }

    .settings-page {
      position: absolute;
      top: 0;
      right: 0;
      bottom: 0;
      width: 300px;
      background-color: #f5f5f5;
      padding: 0px;
      transition: transform 0.3s ease-in-out;
      transform: translateX(100%);
      box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.1);
    }

    .settings-overlay.open .settings-page {
      transform: translateX(0);
    }

    .settings-content label,
    .settings-content input {
      margin-bottom: 10px;
      margin-left: 5px;
    }    

    .container {
      display: flex;
      flex-direction: column;
      height: calc(100vh - 50px);
      background-color: #f5f5f5;
    }

    .top-half {
      display: flex;
      flex-basis: 50%;
    }

    .button {
        flex-grow: 1;
        display: block;
        align-items: center;
        justify-content: center;
        box-sizing: border-box;
        border: 1px solid #ddd;
        background-color: #fff;
        font-size: 50px;
        cursor: pointer;
        transition: all 0.3s ease;
        box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.1);
    }

    .button.left {
      justify-content: flex-start;
      text-align: center;
    }

    .button.right {
      justify-content: flex-end;
    }

    .bottom-half {
      display: flex;
      flex-basis: 50%;
    }

    .button.big {
      flex-grow: 1;
    }

    @media (hover: hover) {
      /* Only apply hover effect on devices that support hover */
      .button:hover{
          background-color: #f0f0f0;
      }      
    }

    .middle-container {
        position: absolute;
        top: 52%;
        left: 49%;
        transform: translate(-50%, -50%);
        width: 150px;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        padding: 5px;
    }   
      
    .timer {
        width: 150px;
        height: 150px;
        background-color: #fff;
        border: 1px solid #ddd;
        border-radius: 5px;
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        display: flex;
        align-items: center;
        justify-content: center;
        /*animation: timerInOut 2s ease-in-out infinite;*/
    }

    .paused {
      animation-play-state: paused;
    }

    .timer p {
        margin: 0;      
        display: flex;
        align-items: center;
        justify-content: center;
        font-size: 130px;
        color: #ff0000;
    }  
    
    .switch {
      position: relative; 
      display: inline-block; 
      width: 60px; 
      height: 34px;
      margin-top: 5px;
    } 

    .switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }

    .slider {
      position: absolute;
      cursor: pointer;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      transition: .4s;
      border-radius: 34px;
    }

    .slider:before {
      position: absolute;
      content: "";
      height: 26px;
      width: 26px;
      left: 4px;
      bottom: 4px;
      background-color: white;
      transition: .4s;
      border-radius: 50%;
    }

    input:checked + .slider {
      background-color: #3f51b5;
    }

    input:focus + .slider {
      box-shadow: 0 0 1px #3f51b5;
    }

    input:checked + .slider:before {
      transform: translateX(26px);
    }

    .slider:after {
      color: white;
      display: block;
      position: absolute;
      transform: translate(-50%, -50%);
      top: 50%;
      left: 50%;
      font-size: 10px;
      font-family: Verdana, sans-serif;
    }

    /* Add new animation keyframes */
    @keyframes timerInOut {
      0% {
        transform: scale(1);
        opacity: 1;
      }
      50% {
        transform: scale(0.8);
        opacity: 0.8;
      }
      100% {
        transform: scale(1);
        opacity: 1;
      }
    }
      
  </style>
</head>
<body>
  <div class="navbar">
    <a href="/" class="link"><button class="back-button">&#8592;</button></a>
    <span class="title" id="navbar-title">Team _: Shot Clock</span>
    <button class="settings-button" onclick="openSettings()">&#9965;</button>
  </div>
  <div class="container">

    <div class="top-half">
      <button class="button left" id="undo" onclick="updateServer(this)">UNDO</button>
      <button class="button right" id="pause" onclick="updateServer(this)">PAUSED</button>
    </div>

    <div class="bottom-half">
      <button class="button big" id="reset" onclick="updateServer(this)">RESET</button>
    </div>   

    <div class="middle-container">
      <div class="timer">
        <p id="timer-text">10</p>
      </div>
      <div style="display: flex; flex-direction: column; align-items: center;">
          <div style="display: flex; align-items: center;">
            <span style="margin-right: 7px; margin-top: 7px; font-size: 20px" id="min-length">10</span>
            <label class="switch">
              <input type="checkbox" id="duration" onchange="updateServer(this)">
              <span class="slider"></span>
            </label>
            <span style="margin-left: 7px; margin-top: 7px; font-size: 20px" id="max-length">15</span>
          </div>
        </div>
    </div>       
  </div>

  <div class="settings-overlay">
    <div class="settings-page" id="settings">
      <div class="navbar" style="margin-bottom: 20px">
        <span>Settings</span>
        <button class="close-button" onclick="closeSettings()">&#88;</button>
      </div>
      <div class="settings-content">
        <label for="name">Name:</label>
        <input type="text" id="name" placeholder="Enter name"><br>
  
        <label for="timer">Timer:</label>
        <input type="number" id="timer" min="0" value="10"><br>
  
        <label for="players">Players:</label>
        <input type="number" id="players" min="0" value="0"><br>
  
        <label for="timeouts">Timeouts:</label>
        <input type="number" id="timeouts" min="0" value="0"><br>
  
        <label for="points">Points:</label>
        <input type="number" id="points" min="0" value="0"><br>
  
        <label for="color">Color:</label>
        <input type="color" id="color" value="#ff0000"><br>
  
        <div class="slider-container" style="display: flex; align-items: center;">
          <label for="direction">Count Direction:</label>
          <div style="display: flex; align-items: center; transform: scale(0.8);">
            <span style="margin-right: 7px; margin-top: 7px; font-size: 20px" id="down">Dw</span>
            <label class="switch">
              <input type="checkbox" id="direction" onchange="updateServer(this)">
              <span class="slider"></span>
            </label>
            <span style="margin-left: 7px; margin-top: 7px; font-size: 20px" id="up">Up</span>
          </div>
          </div>
        </div>
      </div>
    </div>
  </div>

  <script>  
    var side;
    var source;
    function capitalize(str) {
      return str.charAt(0).toUpperCase() + str.slice(1).toLowerCase();
    }

    // Function to run on page load
    document.addEventListener('DOMContentLoaded', function() {
      load();
    });
    
    function load() {
      const path = window.location.pathname;
      side = path.substring(path.lastIndexOf('/') + 1).toLowerCase().replace(/\s+/g, '');
      source = new EventSource(`/events/${side}`);
      source.addEventListener('update', events);
      setTimeout(function() {  
        updateServer("load");      
      }, 100);

      // Get all input elements
      var inputs = document.querySelectorAll('input');

      // Add event listeners to each input element
      inputs.forEach(function(input) {
        if (input.type != "checkbox") {       
          // When the input receives focus, remove event listener
          input.addEventListener('focus', function() {
              source.removeEventListener('update', events);
          });

          // When the input loses focus, add event listener
          input.addEventListener('blur', function() {
              updateServer(input);
              source.addEventListener('update', events);
          });
        } 
      });
    }                  

    // Settings page functions
    function openSettings() {
      document.querySelector('.settings-overlay').classList.add('open');
    }
    function closeSettings() {
      document.querySelector('.settings-overlay').classList.remove('open');
    }

    const settingsOverlay = document.querySelector('.settings-overlay');
    settingsOverlay.addEventListener('click', (event) => {
      if (event.target === settingsOverlay) {
        closeSettings();
      }
    });

    // Update the server when an action occurs 
    function updateServer(element) { 
      var str = element.id;
      if (str === undefined) {
        str = element;
      }
      var value = element.value;
      if (str == "color") {
        value = value.replace("#", "");
        // Convert the hex color code to RGB
        var r = parseInt(value.substring(0, 2), 16);
        var g = parseInt(value.substring(2, 4), 16);
        var b = parseInt(value.substring(4, 6), 16);
        value = `r=${r}g=${g}b=${b}`;
      }
      var xhr = new XMLHttpRequest();
      xhr.open("GET", `/updateServer?side=${side}&type=${str}&value=${value}`, true);    
      xhr.send();
    }

    // Add event listeners 
    function events(e) {
        var response = JSON.parse(e.data);

        // Update title
        document.getElementById("title").innerHTML = response.title;
        document.getElementById("navbar-title").innerHTML = response.title;   
        
        // Update name
        document.getElementById("name").value = response.name;
        
        // Update pause
        var paused = !response.paused;
        var text = paused ? 'PAUSE' : 'RESUME';
        //if (paused) {
        //  timer.classList.add('paused');
        //} else {
        //  timer.classList.remove('paused');
        //}
        document.getElementById("pause").innerHTML = text;               

        // Update timer
        document.getElementById("timer-text").innerHTML = response.timer;
        document.getElementById("timer").value = response.timer;

        // Update min length
        document.getElementById("min-length").innerHTML = response.minLength;

        // Update max length
        document.getElementById("max-length").innerHTML = response.maxLength; 

        // Update length
        document.getElementById("duration").checked = !response.duration;        
        
        // Update points
        document.getElementById("points").value = response.points;
        
        // Update players
        document.getElementById("players").value = response.players;

        // Update timeouts
        document.getElementById("timeouts").value = response.timeouts;

        // Update color
        document.getElementById("timer-text").style.color = response.color;
        document.getElementById("color").value = response.color;

        // Update direction
        document.getElementById("direction").checked = !response.direction;

      };  

  </script>
</body>
</html>
)rawliteral";