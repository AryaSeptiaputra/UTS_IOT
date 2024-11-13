#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN 25
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Pin perangkat
#define LED_HIJAU_PIN 32
#define LED_MERAH_PIN 12
#define LED_KUNING_PIN 27
#define BUZZER_PIN 26
#define RELAY_PIN 33

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
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Inisialisasi pin perangkat
  pinMode(LED_HIJAU_PIN, OUTPUT);
  pinMode(LED_MERAH_PIN, OUTPUT);
  pinMode(LED_KUNING_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Pastikan semua perangkat dalam keadaan mati
  digitalWrite(LED_HIJAU_PIN, LOW);
  digitalWrite(LED_MERAH_PIN, LOW);
  digitalWrite(LED_KUNING_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();

  // Hanya membaca sensor jika sudah 5 detik berlalu
  if (currentMillis - lastMillis >= interval) {
    lastMillis = currentMillis;

    // Baca suhu dan kelembapan dari sensor
    float suhu = dht.readTemperature();
    float kelembapan = dht.readHumidity();

    // Periksa apakah pembacaan sensor valid
    if (isnan(suhu) || isnan(kelembapan)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Membuat objek JSON untuk menggabungkan data sesuai format yang diinginkan
    StaticJsonDocument<512> jsonData;
    jsonData["Suhumax"] = 36.0;
    jsonData["Suhumin"] = 23.0;
    jsonData["Suhurata"] = 28.35;

    // Buat array "nilai_suhu_max_humid_max"
    JsonArray nilaiSuhuMaxHumidMax = jsonData.createNestedArray("nilai_suhu_max_humid_max");
    JsonObject dataPoint = nilaiSuhuMaxHumidMax.createNestedObject();
    dataPoint["idx"] = random(100, 1000);
    dataPoint["Suhun"] = suhu;
    dataPoint["Kecerahan"] = random(0, 100);
    dataPoint["Timestamp"] = millis();

    // Buat array "month_year_max"
    JsonArray monthYearMax = jsonData.createNestedArray("month_year_max");
    JsonObject monthYear = monthYearMax.createNestedObject();
    monthYear["month_year"] = "2024-11";

    // Konversi objek JSON menjadi string
    char dataToSend[512];
    serializeJson(jsonData, dataToSend);

    // Kirim data JSON ke topik MQTT tunggal
    client.publish(topic_data, dataToSend);

    Serial.println("Data sent to MQTT Broker:");
    Serial.println(dataToSend);

    // Logika if-else untuk kontrol perangkat berdasarkan suhu
    if (suhu > 35) {
      digitalWrite(LED_MERAH_PIN, HIGH);
      digitalWrite(LED_KUNING_PIN, LOW);
      digitalWrite(LED_HIJAU_PIN, LOW);
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(RELAY_PIN, HIGH);
    } else if (suhu >= 30 && suhu <= 35) {
      digitalWrite(LED_MERAH_PIN, LOW);
      digitalWrite(LED_KUNING_PIN, HIGH);
      digitalWrite(LED_HIJAU_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(RELAY_PIN, HIGH);
    } else {
      digitalWrite(LED_MERAH_PIN, LOW);
      digitalWrite(LED_KUNING_PIN, LOW);
      digitalWrite(LED_HIJAU_PIN, HIGH);
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(RELAY_PIN, LOW);
    }
  }  
}