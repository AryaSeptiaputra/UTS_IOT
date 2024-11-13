from flask import Flask, request, jsonify, render_template

app = Flask(__name__)

# Variabel untuk menyimpan data terbaru
latest_data = {}

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data', methods=['GET'])
def get_data():
    # Endpoint untuk mendapatkan data sensor dalam format JSON
    return jsonify(latest_data)

@app.route('/data', methods=['POST'])
def post_data():
    global latest_data
    # Ambil data dari body request
    data = request.get_json()
    if data:
        latest_data = data
        print("Data received:", latest_data)
        return jsonify({"status": "success", "message": "Data received"}), 200
    else:
        return jsonify({"status": "error", "message": "Invalid data format"}), 400

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
