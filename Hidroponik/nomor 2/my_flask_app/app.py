from flask import Flask, jsonify, render_template
import paho.mqtt.client as mqtt
import json
import threading

app = Flask(__name__)

# Variabel untuk menyimpan data terbaru
latest_data = {}

# Fungsi callback saat menerima pesan dari broker MQTT
def on_message(client, userdata, msg):
    global latest_data
    payload = msg.payload.decode()

    if msg.topic == "hidroponik/data":
        try:
            latest_data = json.loads(payload)
            print("Data received:", latest_data)
        except json.JSONDecodeError:
            print("Failed to parse JSON data:", payload)

# Konfigurasi MQTT Client
mqtt_client = mqtt.Client()
mqtt_client.on_message = on_message
mqtt_client.connect("broker.hivemq.com", 1883, 60)
mqtt_client.subscribe("hidroponik/data")

# Fungsi untuk menjalankan loop MQTT secara terpisah
def mqtt_loop():
    mqtt_client.loop_forever()

# Mulai loop MQTT di thread terpisah
mqtt_thread = threading.Thread(target=mqtt_loop)
mqtt_thread.start()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data', methods=['GET'])
def get_data():
    # Endpoint untuk mendapatkan data sensor dalam format JSON
    return jsonify(latest_data)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
