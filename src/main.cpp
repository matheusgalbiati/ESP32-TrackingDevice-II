#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define RXD2 16
#define TXD2 17
HardwareSerial neogps(1);

const String DEVICE_ID = "1"; // Insira o ID do seu dispositivo

const char *ssid = "Matheus Galaxy S10";      // Insira o nome da sua rede WiFi
const char *password = "Matheus@10_Galbiati123"; // Insira a senha da sua rede WiFi

TinyGPSPlus gps;

void sendHTTPPost(String data);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // Conectar ao WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi!");
  digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED);
}

void loop() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(1000);
  }
  digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED);
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 10000;) {
    while (neogps.available()) {
      if (gps.encode(neogps.read())) {
        newData = true;
      }
    }
  }

  if (newData == true) {
    newData = false;
    String lat = String(gps.location.lat(), 6);
    String lng = String(gps.location.lng(), 6);
    String json = "{\"device_id\": \"" + DEVICE_ID + "\", \"latitude\": \"" + lat + "\", \"longitude\": \"" + lng + "\", \"last_track_time\": \"" + "2023-01-01 21:00:00" + "\"}";

    Serial.println(lat);
    Serial.println(lng);
    Serial.println(json);
    Serial.println("-----------------------");

    // Enviar dados por meio de uma requisição HTTP
    sendHTTPPost(json);
    Serial.println("-----------------------");
  } else {
    Serial.println("No Data");
  }
}

void sendHTTPPost(String data) {
  HTTPClient http;
  
  // Endereço do servidor para onde você enviará os dados
  String serverAddress = "https://g5wz2pwz9l.execute-api.us-east-1.amazonaws.com/dev/device_geolocation/update";
  
  // Adicione quaisquer parâmetros necessários ao URL
  // serverAddress += "?parametro1=valor1&parametro2=valor2";

  http.begin(serverAddress);
  http.addHeader("Content-Type", "application/json");

  // Enviar a requisição POST com os dados JSON
  int httpResponseCode = http.POST(data);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("HTTP Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
