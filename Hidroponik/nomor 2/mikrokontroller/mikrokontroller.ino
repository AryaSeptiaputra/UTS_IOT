#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Konfigurasi WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Konfigurasi MQTT
const char* mqtt_server = "broker.hivemq.com";
const char* topic_data = "hidroponik/data";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMillis = 0;
const long interval = 5000;

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("HidroponikClientESP")) {
      Serial.println("Connected to MQTT Broker");
    } else {
      delay(5000);
      Serial.print(".");
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

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
    dataPoint1["idx"] = 123; // Nilai dummy untuk index 1
    dataPoint1["Suhun"] = 27.5; // Suhu dummy
    dataPoint1["Kecerahan"] = 75; // Kecerahan dummy
    dataPoint1["Timestamp"] = millis();

    // Data dummy untuk indeks kedua
    JsonObject dataPoint2 = nilaiSuhuMaxHumidMax.createNestedObject();
    dataPoint2["idx"] = 456; // Nilai dummy untuk index 2
    dataPoint2["Suhun"] = 29.0; // Suhu dummy
    dataPoint2["Kecerahan"] = 65; // Kecerahan dummy
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

    // Kirim data JSON ke topik MQTT tunggal
    client.publish(topic_data, dataToSend);

    Serial.println("Data sent to MQTT Broker:");
    Serial.println(dataToSend);
  }
}
