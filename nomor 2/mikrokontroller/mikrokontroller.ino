#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Konfigurasi WiFi
const char* ssid = "Arya's A55";
const char* password = "87654321";

// URL API
const char* serverUrl = "http://192.168.81.40:5000/data";

unsigned long lastMillis = 0;
const long interval = 10000;

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
}

void loop() {
  unsigned long currentMillis = millis();

  // Kirim data dummy setiap 5 detik
  if (currentMillis - lastMillis >= interval) {
    lastMillis = currentMillis;

    // Membuat objek JSON dengan data dummy
    StaticJsonDocument<512> jsonData;
    jsonData["Suhumax"] = 36.0;
    jsonData["Suhumin"] = 23.0;
    jsonData["Suhurata"] = 28.35;

    // Buat array "nilai_suhu_max_humid_max" dengan 2 indeks
    JsonArray nilaiSuhuMaxHumidMax = jsonData.createNestedArray("nilai_suhu_max_humid_max");

    // Data dummy untuk indeks pertama
    JsonObject dataPoint1 = nilaiSuhuMaxHumidMax.createNestedObject();
    dataPoint1["idx"] = 123;
    dataPoint1["Suhun"] = 27.5;
    dataPoint1["humid"] = 30;
    dataPoint1["Kecerahan"] = 75;
    dataPoint1["Timestamp"] = millis();

    // Data dummy untuk indeks kedua
    JsonObject dataPoint2 = nilaiSuhuMaxHumidMax.createNestedObject();
    dataPoint2["idx"] = 456;
    dataPoint2["Suhun"] = 29.0;
    dataPoint1["humid"] = 31;
    dataPoint2["Kecerahan"] = 65;
    dataPoint2["Timestamp"] = millis();

    // Buat array "month_year_max" dengan 2 indeks
    JsonArray monthYearMax = jsonData.createNestedArray("month_year_max");

    JsonObject monthYear1 = monthYearMax.createNestedObject();
    monthYear1["month_year"] = "2024-11";

    JsonObject monthYear2 = monthYearMax.createNestedObject();
    monthYear2["month_year"] = "2024-12";

    // Konversi objek JSON menjadi string
    char dataToSend[512];
    serializeJson(jsonData, dataToSend);

    // Kirim data JSON menggunakan HTTP POST
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient wifiClient;
      HTTPClient http;
      http.begin(wifiClient, serverUrl);
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(dataToSend);

      if (httpResponseCode > 0) {
        Serial.print("Data sent, response code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("Error sending data, code: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    } else {
      Serial.println("WiFi not connected");
    }
  }
}
