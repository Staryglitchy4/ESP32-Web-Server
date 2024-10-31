#include <WiFi.h>               // Librería para gestionar Wi-Fi
#include <WiFiClient.h>         // Librería para cliente Wi-Fi
#include <WebServer.h>          // Librería para servidor web
#include <ESPmDNS.h>            // Librería para DNS en ESP32
#include <DHT.h>                // Librería para el sensor DHT

// Credenciales de la red Wi-Fi
const char *ssid = "Esp32-Tilico";
const char *password = "Tilico123456";

// Crear servidor web en el puerto 80
WebServer server(80);

// Configuración del sensor DHT11 en el pin D4
DHT dht(D4, DHT11);

// Función para manejar la ruta raíz "/"
void handleRoot() {
  // Enviar la página HTML que muestra los datos de temperatura y humedad
  server.send(200, "text/html",
              "<html>\
  <head>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 DHT Server</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center; }\
    h2 { font-size: 3.0rem; }\
    p { font-size: 3.0rem; }\
    .units { font-size: 1.2rem; }\
    .dht-labels { font-size: 1.5rem; vertical-align: middle; padding-bottom: 15px; }\
    </style>\
    <script>\
    function fetchData() {\
      var xhr = new XMLHttpRequest();\
      xhr.onreadystatechange = function() {\
        if (xhr.readyState == 4 && xhr.status == 200) {\
          var data = JSON.parse(xhr.responseText);\
          document.getElementById('temperature').innerHTML = data.temperature.toFixed(2);\
          document.getElementById('humidity').innerHTML = data.humidity.toFixed(2);\
        }\
      };\
      xhr.open('GET', '/data', true);\
      xhr.send();\
    }\
    setInterval(fetchData, 2000);\
    </script>\
  </head>\
  <body>\
    <h2>ESP32 DHT Server</h2>\
    <p>\
      <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
      <span class='dht-labels'>Temperature</span>\
      <span id='temperature'>--</span>\
      <sup class='units'>&deg;C</sup>\
    </p>\
    <p>\
      <i class='fas fa-tint' style='color:#00add6;'></i>\
      <span class='dht-labels'>Humidity</span>\
      <span id='humidity'>--</span>\
      <sup class='units'>&percnt;</sup>\
    </p>\
  </body>\
</html>");
}

// Función para manejar la ruta "/data" que devuelve los datos en formato JSON
void handleData() {
  String json = "{";
  json += "\"temperature\": " + String(readDHTTemperature(), 2) + ",";  // Añadir temperatura en JSON
  json += "\"humidity\": " + String(readDHTHumidity(), 2);               // Añadir humedad en JSON
  json += "}";
  server.send(200, "application/json", json);   // Enviar JSON con los datos de temperatura y humedad
}

// Función de configuración
void setup(void) {
  Serial.begin(115200);           // Iniciar monitor serie
  dht.begin();                    // Iniciar sensor DHT
  Serial.println("Configuring access point...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);     // Conectar a la red Wi-Fi
  Serial.println("");

  // Configurar punto de acceso
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Soft AP creation failed.");
    while (1) {}                   // Detener en caso de error
  }
  IPAddress myIP = WiFi.softAPIP(); // Obtener dirección IP del AP
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();                   // Iniciar servidor web

  Serial.println("Server started");

  // Iniciar mDNS para acceder por "esp32.local"
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  // Definir rutas para el servidor web
  server.on("/", handleRoot);       // Asignar función `handleRoot` a "/"
  server.on("/data", handleData);   // Asignar función `handleData` a "/data"

  server.begin();                   // Iniciar servidor HTTP
  Serial.println("HTTP server started");
}

// Función principal del bucle
void loop(void) {
  server.handleClient();            // Manejar solicitudes entrantes del cliente
  delay(2);                         // Pequeño retardo
}

// Función para leer la temperatura del sensor DHT
float readDHTTemperature() {
  float t = dht.readTemperature();  // Leer temperatura
  if (isnan(t)) {                   // Si falla la lectura
    Serial.println("Failed to read from DHT sensor!");
    return -1;                      // Devolver -1 en caso de error
  } else {
    return t;                       // Devolver temperatura
  }
}

// Función para leer la humedad del sensor DHT
float readDHTHumidity() {
  float h = dht.readHumidity();     // Leer humedad
  if (isnan(h)) {                   // Si falla la lectura
    Serial.println("Failed to read from DHT sensor!");
    return -1;                      // Devolver -1 en caso de error
  } else {
    return h;                       // Devolver humedad
  }
}
