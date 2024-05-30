
const char *html_page = "<!DOCTYPE html>"
"<html>"
"<head>"
"    <meta charset=\"utf-8\">"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">"
""
"    <link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap@4.0.0/dist/css/bootstrap.min.css\" integrity=\"sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm\" crossorigin=\"anonymous\">"
""
"    <link rel=\"stylesheet\" type=\"test/css\" href=\"{% static 'app/static/main.css' %}\">"
""
"    <style> textarea {"
"        width: 150px;"
"        height: 25px;"
"    } </style>"
"    <style>header{"
"      width: 100%;"
"      height: 5vh;"
"    }"
"    .container{"
"      max-width: 150rem;"
"      width: 90%;"
"      margin: 10 auto;"
"    }"
"    .container {"
"      display: grid; "
"      align-items: flex-start; "
"      grid-template-columns: 1fr 1fr 3fr;; "
"    }"
"    "
"    .text, .chart {"
"      flex: 1;"
"    }"
"    .chart {"
"      width: 110%; "
"      height: auto; "
"      margin-right: 0; "
"      margin-left: 0; "
"      display: grid;"
"      grid-column: 3; "
"      visibility: visible;"
"    }"
"    @media (min-width: 1600px) {"
"      .chart {"
"        width: 100%; "
"      }"
"    }"
""
"    @media (min-width: 1750px) {"
"      .chart {"
"        width: 117%;"
"      }"
"    }"
"  </style>"
""
"    <title>Dynamic Line Chart with Limited Data Points</title>"
"    <script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.js\"></script>"
""
""
"    <title>WebUI</title>"
"</head>"
"<body>"
"    <header class=\"site-header\">"
"        <nav class=\"navbar navbar-expand-md navbar-dark bg-steel fixed-top\">"
"          <div class=\"container\">"
"            <a style=\"color: black;\" class=\"navbar-brand mr-4\" href=\".\"><font size=\"6\">Web User Interface</font></a>"
"            <button class=\"navbar-toggler\" type=\"button\" data-toggle=\"collapse\" data-target=\"#navbarToggle\" aria-controls=\"navbarToggle\" aria-expanded=\"false\" aria-label=\"Toggle navigation\">"
"            <span class=\"navbar-toggler-icon\"></span>"
"            </button>"
"            <div class=\"collapse navbar-collapse\" id=\"navbarToggle\">"
"              <div class=\"navbar-nav mr-auto\">"
"                <a style=\"color: black;\" class=\"nav-item nav-link\" href=\".\"><font size=\"6\">Homepage</font></a>"
"                <a style=\"color: black;\" class=\"nav-item nav-link\" href=\"./documentation\"><font size=\"6\">Documentation</font></a>"
"              </div>"
"              <div class=\"navbar-nav\">"
"              </div>"
"            </div>"
"          </div>"
"        </nav>"
"    </header>"
"    <div class=\"container\">"
"      <div class=\"text\">"
"        <br><br><br>"
"        <h1>ESP32  Control</h1>"
"        <p id=\"ledState\">LED is OFF</p> "
"        <button onclick=\"toggleLED();\">Toggle LED</button>"
""
"        <br><br><br>"
"        <a id=\"moisture_value_label\"><font size=\"5px\"> Moisture Value: </font></a>"
"        <p id=\"moisture_value\"><font size=\"4px\"> 1.00 </font></p>"
"        <a id=\"moisture_threshold_label\"><font size=\"5px\"> Moisture Threshold: </font></a>"
"        <p id=\"moisture_threshold\"><font size=\"4px\"> 0.00 </font></p>"
""
"        <a id=\"solar_value_label\"><font size=\"5px\"> Solar Value: </font></a>"
"        <p id=\"solar_value\"><font size=\"4px\"> 0.00 </font></p>"
"        <a id=\"solar_threshold_label\"><font size=\"5px\"> Solar Threshold: </font></a>"
"        <p id=\"solar_threshold\"><font size=\"4px\"> 0.00 </font></p>"
""
"        <br>"
"        <p id=\"pumpLabel\">Turn ON/OFF the water pump</p> "
"        <button id=\"pumpState\" onclick=\"switchPump();\">TURN ON</button>"
""
"        <script>"
"        function toggleLED() {"
"          var xhr = new XMLHttpRequest();"
"          xhr.onreadystatechange = function() {"
"            if (xhr.readyState == 4 && xhr.status == 200) {"
"              document.getElementById('ledState').innerHTML = xhr.responseText;"
"            }"
"          };"
"          xhr.open('GET', '/toggle', true);"
"          xhr.send();"
"        }"
"        function switchPump() {"
"          var xhr = new XMLHttpRequest();"
"          xhr.onreadystatechange = function() {"
"            if (xhr.readyState == 4 && xhr.status == 200) {"
"              document.getElementById('pumpState').innerHTML = xhr.responseText;"
"            }"
"          };"
"          xhr.open('GET', '/pump', true);"
"          xhr.send();"
"        }"
"        function find_and_send(id, path){"
"          var xhr = new XMLHttpRequest();"
"          xhr.onreadystatechange = function() {"
"            if (xhr.readyState == 4 && xhr.status == 200) {"
"              document.getElementById(id).innerHTML = xhr.responseText;"
"            }"
"          };"
"          xhr.open('GET', path, true);"
"          xhr.send();"
"        }"
"        window.onload = function() {"
"          find_and_send(id='moisture_threshold', path='/def_moist');"
"          find_and_send(id='solar_threshold', path='/def_solar');"
"          find_and_send(id='moisture_value', path='/moist_val');"
"          find_and_send(id='solar_value', path='/solar_val');"
"        };"
"        function updateVariables(){"
"          find_and_send(id='moisture_threshold', path='/def_moist');"
"          find_and_send(id='solar_threshold', path='/def_solar');"
"          find_and_send(id='moisture_value', path='/moist_val');"
"          find_and_send(id='solar_value', path='/solar_val');"
"          "
"        };"
"        setInterval(updateVariables, 1000);"
"        </script>"
"      </div>"
"      <div class=\"chart\">"
"        <br><br><br>"
"        <canvas id=\"myLimitedLineChart\" width=\"400\" height=\"200\"></canvas>"
"      </div>"
"    </div>"
"    <script>"
"      var ctx = document.getElementById('myLimitedLineChart').getContext('2d');"
"      var maxDataPoints = 30;"
"      var dataPoints = [];"
"      var timeLabels = [];"
"  "
"      function updateChartData() {"
"          var valueElement = document.getElementById('moisture_value');"
"          var value = parseInt(valueElement.textContent.replace('Value: ', ''));"
"          var currentTime = new Date().toLocaleTimeString();"
"  "
"          dataPoints.push(value);"
"          timeLabels.push(currentTime);"
"  "
"          if (dataPoints.length > maxDataPoints) {"
"              dataPoints.shift();"
"              timeLabels.shift();"
"          }"
"  "
"          myLimitedLineChart.data.datasets[0].data = dataPoints;"
"          myLimitedLineChart.data.labels = timeLabels;"
"          myLimitedLineChart.update();"
"      }"
"  "
"      var myLimitedLineChart = new Chart(ctx, {"
"          type: 'line',"
"          data: {"
"              labels: timeLabels,"
"              datasets: [{"
"                  label: 'Dynamic Value',"
"                  data: dataPoints,"
"                  fill: true,"
"                  borderColor: 'rgb(75, 192, 192)',"
"                  tension: 0.1"
"              }]"
"          },"
"          options: {"
"              scales: {"
"                  y: {"
"                      beginAtZero: true"
"                  }"
"              }"
"          }"
"      });"
"  "
"      setInterval(updateChartData, 1000);"
"  </script>"
"    <script src=\"https://code.jquery.com/jquery-3.2.1.slim.min.js\" integrity=\"sha384-KJ3o2DKtIkvYIK3UENzmM7KCkRr/rE9/Qpg6aAZGJwFDMVNA/GpGFF93hXpG5KkN\" crossorigin=\"anonymous\"></script>"
"    <script src=\"https://cdn.jsdelivr.net/npm/popper.js@1.12.9/dist/umd/popper.min.js\" integrity=\"sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q\" crossorigin=\"anonymous\"></script>"
"    <script src=\"https://cdn.jsdelivr.net/npm/bootstrap@4.0.0/dist/js/bootstrap.min.js\" integrity=\"sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl\" crossorigin=\"anonymous\"></script>"
"</body>"
"</html>";