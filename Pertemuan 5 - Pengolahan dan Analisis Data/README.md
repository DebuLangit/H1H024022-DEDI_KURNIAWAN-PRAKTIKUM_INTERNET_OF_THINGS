# Modul Praktikum 5: Pengolahan dan Analisis Data
> Filtering, Analisis dan Visualisasi Data

## Percobaan 5A: Pengolahan Data Sensor pada ESP32 (Moving Average dan Statistik)

### 1. Detail Percobaan

Percobaan 5A berfokus pada pemahaman dan implementasi pengolahan data sensor secara langsung di sisi perangkat (edge processing). Pada percobaan ini, mikrokontroler menggunakan filter moving average untuk menghaluskan data pembacaan serta mencatat nilai suhu minimum dan maksimum selama program dijalankan.

### 2. Penjelasan Kode dan Fungsi (Function)

```cpp
#include <Arduino.h>

const int N = 5;            // Jumlah sampel moving average
float buffer[N];           // Menyimpan N pembacaan terakhir
int indeks = 0;            // Posisi penulisan berikutnya pada buffer
int jumlah = 0;            // Jumlah sampel yang sudah terisi

float suhuMin = 1000.0;
float suhuMax = -1000.0;

// Mock function pengganti sensor DHT22
float bacaSuhuMock() {
    static unsigned long jumlahBaca = 0;
    jumlahBaca++;

    // Suhu dasar berubah secara bertahap
    float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);

    // Noise kecil untuk mensimulasikan fluktuasi sensor
    float noise = random(-5, 6) * 0.1;

    float suhu = suhuDasar + noise;

    // Lonjakan suhu setiap 25 pembacaan untuk simulasi anomali
    if (jumlahBaca % 25 == 0) {
        suhu += 8.0;
    }

    return suhu;
}

float hitungMovingAverage(float nilaiBaru) {
    buffer[indeks] = nilaiBaru;
    indeks = (indeks + 1) % N;
    if (jumlah < N) jumlah++;

    float total = 0;
    for (int i = 0; i < jumlah; i++) {
        total += buffer[i];
    }
    return total / jumlah;
}

void setup() {
    Serial.begin(115200);
}

void loop() {
    // Membaca data dari fungsi mock
    float suhu = bacaSuhuMock();

    if (isnan(suhu)) {
        Serial.println("Pembacaan gagal, data diabaikan");
    } else {
        float suhuFilter = hitungMovingAverage(suhu);

        if (suhu < suhuMin) suhuMin = suhu;
        if (suhu > suhuMax) suhuMax = suhu;

        Serial.print("Mentah: ");
        Serial.print(suhu);
        Serial.print(" | Filter: ");
        Serial.print(suhuFilter);
        Serial.print(" | Min: ");
        Serial.print(suhuMin);
        Serial.print(" | Max: ");
        Serial.println(suhuMax);
    }

    delay(2000);
}
```

### 3. Penjelasan Percabangan dan Kondisional

- `if (jumlahBaca % 25 == 0)`: Mengevaluasi jumlah pembacaan. Setiap kelipatan 25 pembacaan, program menambahkan nilai 8.0 pada suhu untuk mensimulasikan terjadinya lonjakan/anomali data.

- `if (jumlah < N)`: Mengecek apakah jumlah sampel (buffer) yang terisi masih kurang dari nilai N (5). Jika ya, variabel `jumlah` diinkrementasi agar pembagian rata-rata hanya dilakukan pada jumlah data aktual yang telah masuk.

- `if (isnan(suhu))`: Memeriksa validitas data hasil pembacaan suhu. Jika Not a Number (NaN), program mencetak pesan gagal dan memblokir eksekusi logika selanjutnya.

- `if (suhu < suhuMin)` dan `if (suhu > suhuMax)`: Membandingkan data pembacaan terbaru dengan riwayat data ekstrem. Jika suhu saat ini lebih rendah dari suhu minimum yang tersimpan, atau lebih tinggi dari suhu maksimum, nilai variabel diperbarui dengan suhu terbaru.


### 4. Library yang Digunakan

-   `Arduino.h`: Library standar pustaka inti Arduino untuk menyediakan fungsi-fungsi dasar seperti `Serial`, matematika `sin()`, `random()`, dan `delay()`.

### 5. Modifikasi Program untuk Menghitung Simpangan Baku (Standard Deviation)

```cpp
#include <Arduino.h>
#include <math.h> // Tambahkan library math untuk fungsi sqrt()

const int N = 5;            // Jumlah sampel moving average
float buffer[N];           // Menyimpan N pembacaan terakhir
int indeks = 0;            // Posisi penulisan berikutnya pada buffer
int jumlah = 0;            // Jumlah sampel yang sudah terisi

float suhuMin = 1000.0;
float suhuMax = -1000.0;

// Mock function pengganti sensor DHT22
float bacaSuhuMock() {
    static unsigned long jumlahBaca = 0;
    jumlahBaca++;

    // Suhu dasar berubah secara bertahap
    float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);

    // Noise kecil untuk mensimulasikan fluktuasi sensor
    float noise = random(-5, 6) * 0.1;

    float suhu = suhuDasar + noise;

    // Lonjakan suhu setiap 25 pembacaan untuk simulasi anomali
    if (jumlahBaca % 25 == 0) {
        suhu += 8.0;
    }

    return suhu;
}

float rataRataSaatIni = 0; // Tambahkan variabel global untuk menyimpan rata-rata

float hitungMovingAverage(float nilaiBaru) {
    buffer[indeks] = nilaiBaru;
    indeks = (indeks + 1) % N;
    if (jumlah < N) jumlah++;

    float total = 0;
    for (int i = 0; i < jumlah; i++) {
        total += buffer[i];
    }
    
    rataRataSaatIni = total / jumlah; // Simpan nilai rata-rata ke variabel global
    return rataRataSaatIni;
}

// FUNGSI BARU: Menghitung simpangan baku dari data di dalam buffer
float hitungSimpanganBaku() {
    if (jumlah == 0) return 0; // Mencegah error jika buffer kosong
    
    float jumlahVarians = 0; // Variabel untuk menyimpan total selisih kuadrat
    for (int i = 0; i < jumlah; i++) {
        // (x - mean)^2
        jumlahVarians += pow(buffer[i] - rataRataSaatIni, 2); 
    }
    
    // Simpangan baku = akar kuadrat dari (jumlahVarians / jumlah data)
    return sqrt(jumlahVarians / jumlah); 
}

void setup() {
    Serial.begin(115200);
}

void loop() {
    float suhu = bacaSuhuMock();

    if (isnan(suhu)) {
        Serial.println("Pembacaan gagal, data diabaikan");
    } else {
        float suhuFilter = hitungMovingAverage(suhu);
        float simpanganBaku = hitungSimpanganBaku(); // Memanggil fungsi hitung simpangan baku

        if (suhu < suhuMin) suhuMin = suhu;
        if (suhu > suhuMax) suhuMax = suhu;

        Serial.print("Mentah: ");
        Serial.print(suhu);
        Serial.print(" | Filter: ");
        Serial.print(suhuFilter);
        Serial.print(" | StdDev: ");         // Menampilkan label simpangan baku
        Serial.print(simpanganBaku);         // Menampilkan nilai simpangan baku
        Serial.print(" | Min: ");
        Serial.print(suhuMin);
        Serial.print(" | Max: ");
        Serial.println(suhuMax);
    }
    delay(2000);
}
```


## Percobaan 5B: Akuisisi, Analisis, dan Visualisasi Data

### 1. Detail Percobaan

Percobaan 5B mendemonstrasikan tahapan pengolahan data pada sisi komputer (server/PC). Praktikum ini meliputi pengambilan data yang dipublikasikan mikrokontroler lewat MQTT, menyimpan data tersebut dalam file CSV, menghitung statistik deskriptif dan deteksi anomali menggunakan perhitungan z-score, dan terakhir memvisualisasikan hasilnya ke dalam bentuk grafik.


### 2. Penjelasan (`percobaan5b.ino`)

```cpp
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "MamaLime";
const char* password = "sayaakanlawan";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

// Topic MQTT
const char* topicPerintah = "KelompokDediPyka/perintah";
const char* topicData = "KelompokDediPyka/data"; // Topic untuk kirim data suhu

const int ledPin = 4; // GPIO4 (D2 pada NodeMCU)

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastSend = 0; // Timer untuk interval pengiriman data

// Mock function pengganti sensor DHT22
float bacaSuhuMock() {
    static unsigned long jumlahBaca = 0;
    jumlahBaca++;

    // Suhu dasar berubah secara bertahap
    float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);

    // Noise kecil untuk mensimulasikan fluktuasi sensor
    float noise = random(-5, 6) * 0.1;

    float suhu = suhuDasar + noise;

    // Lonjakan suhu setiap 25 pembacaan untuk simulasi anomali
    if (jumlahBaca % 25 == 0) {
        suhu += 8.0;
    }

    return suhu;
}

// Fungsi callback dipanggil otomatis setiap ada pesan perintah masuk
void callback(char* topic, byte* payload, unsigned int length) {
    String pesan;
    for (unsigned int i = 0; i < length; i++) {
        pesan += (char)payload[i];
    }
    Serial.print("Pesan diterima [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(pesan);

    // Deserialisasi data JSON yang diterima
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, pesan);

    if (error) {
        Serial.print("Gagal parsing JSON: ");
        Serial.println(error.c_str());
        return;
    }

    const char* perintah = doc["perintah"];
    if (perintah != NULL) {
        if (String(perintah) == "ON") {
            digitalWrite(ledPin, HIGH);
            Serial.println("Aktuator: ON");
        } else if (String(perintah) == "OFF") {
            digitalWrite(ledPin, LOW);
            Serial.println("Aktuator: OFF");
        }
    }
}

void hubungkanWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Menghubungkan ke WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() {
    while (!client.connected()) {
        Serial.print("Menghubungkan ke broker MQTT...");
        String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
        if (client.connect(clientId.c_str())) {
            Serial.println("berhasil terhubung!");
            client.subscribe(topicPerintah); // Subscribe ke topic perintah
            Serial.print("Subscribe ke topic: ");
            Serial.println(topicPerintah);
        } else {
            Serial.print("gagal, rc=");
            Serial.print(client.state());
            Serial.println(" coba lagi dalam 2 detik");
            delay(2000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    
    hubungkanWiFi();
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        hubungkanMQTT();
    }
    client.loop(); // Wajib dipanggil terus-menerus agar pesan callback dapat diterima

    // Kirim data suhu mock setiap 2 detik (2000 ms)
    unsigned long now = millis();
    if (now - lastSend > 2000) {
        lastSend = now;

        float suhu = bacaSuhuMock();

        // Buat payload JSON
        JsonDocument doc;
        doc["suhu"] = suhu;

        char buffer[128];
        serializeJson(doc, buffer);

        // Publish ke topic data
        client.publish(topicData, buffer);
        Serial.print("Publish data suhu: ");
        Serial.println(buffer);
    }
}
```


### 3. Skrip Python (modul5_analisis_data.py)

Skrip Python ini berfungsi sebagai penerima (subscriber) di sisi PC yang mengolah data yang diterbitkan oleh ESP8266 di atas.
```py
import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "KelompokDediPyka/data"  # Samakan dengan topicData pada ESP32
JUMLAH_SAMPEL = 30  # Jumlah data yang dikumpulkan

data = []

def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker, kode:", reason_code)
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        suhu = float(payload["suhu"])
    except (ValueError, KeyError):
        print("Data tidak valid, diabaikan")
        return

    data.append({"waktu": time.strftime("%Y-%m-%d %H:%M:%S"), "suhu": suhu})
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}")

    if len(data) >= JUMLAH_SAMPEL:
        client.disconnect()

# Tahap 1: Akuisisi data dari MQTT
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT)
client.loop_forever()

# Tahap 2: Simpan ke CSV
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("Data tersimpan di data_suhu.csv")

# Tahap 3: Analisis statistik dan deteksi anomali (z-score)
rata2 = df["suhu"].mean()
std = df["suhu"].std()

if std == 0:
    df["zscore"] = 0.0
else:
    df["zscore"] = (df["suhu"] - rata2) / std

df["anomali"] = df["zscore"].abs() > 2

print(f"Rata-rata   : {rata2:.2f}")
print(f"Std deviasi : {std:.2f}")
print(f"Minimum     : {df['suhu'].min():.2f}")
print(f"Maksimum    : {df['suhu'].max():.2f}")
print(f"Jumlah anomali: {df['anomali'].sum()}")

# Tahap 4: Visualisasi
plt.figure(figsize=(10, 4))
plt.plot(df.index, df["suhu"], marker="o", label="Suhu")
anom = df[df["anomali"]]
plt.scatter(anom.index, anom["suhu"], color="red", s=100, label="Anomali", zorder=3)
plt.axhline(rata2, color="gray", linestyle="--", label="Rata-rata")
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Data Suhu dan Deteksi Anomali")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu.png")
plt.show()
```

- Tahap 1 (Akuisisi Data MQTT): Membuat objek klien, berlangganan ke topicData, dan mengambil muatan JSON menggunakan loop hingga jumlah sampel yang dibutuhkan terpenuhi (30 data).
- Tahap 2 (Simpan ke CSV): Memanfaatkan pustaka `pandas` untuk mengubah daftar data list ke format tabular (DataFrame) dan menyimpannya ke `data_suhu.csv`.
- Tahap 3 (Analisis & Z-Score): Menghitung mean, simpangan baku, nilai minimum, dan maksimum. Digunakan rumus $z = (x - rata-rata) / std$ untuk mendeteksi anomali. Nilai anomali ditandai menjadi `True` jika jarak $\vert{}z\vert{} > 2$.
- Tahap 4 (Visualisasi): Menggunakan pustaka `matplotlib` untuk menampilkan plot grafis yang menyoroti data suhu biasa dengan garis, dan data anomali dengan titik (scatter) berwarna merah.

### 4. Penjelasan Percabangan dan Kondisional

- `if (error)` pada program `.ino`: Mengevaluasi apakah proses parsing dokumen JSON gagal dilakukan. Jika iya, program mencetak string error dari lib ArduinoJson dan dihentikan.

- `if (perintah != NULL)` pada program `.ino`: Memastikan atribut perintah ada dalam key JSON yang diterima. Selanjutnya menggunakan `if (String(perintah) == "ON")` dan `else if` untuk menyalakan/mematikan lampu LED.

- `if (now - lastSend > 2000)` pada `.ino`: Menggunakan millis() untuk pengaturan penjadwalan non-blocking tiap 2 detik tanpa harus menghentikan (delay) penerimaan MQTT.

- `try - except (ValueError, KeyError)` pada skrip Python: Menangani potensi error pada saat parsing string JSON atau mengonversi data ke tipe `float`, sehingga program Python tidak hancur (crash) apabila pesan string rusak.

- `if len(data) >= JUMLAH_SAMPEL` pada skrip Python: Memutuskan sambungan ke MQTT secara paksa begitu jumlah array data telah mencapai ambang batas yang ditentukan (30 buah).

- `if std == 0` pada skrip Python: Mencegah ZeroDivisionError secara kondisional apabila deviasi/simpangan baku dari data ternyata bernilai statis (0), dengan memberikan status zscore nilai 0.0 statis.

### 5. Library yang Digunakan

- `ESP8266WiFi.h`: Untuk menyambungkan hardware ke access point jaringan.
- `PubSubClient.h`: Menginisialisasi publisher/subscriber komunikasi broker MQTT.
- `ArduinoJson.h`: Mengubah (serialize) dan membaca (deserialize) pesan string format JSON pada ESP8266.
- `paho-mqtt`: Pustaka pada Python untuk membuat klien yang bisa berlangganan (subscribe) topik dari broker MQTT.
- `pandas`: Pustaka Python untuk memproses data tabular, list array, menjadi bentuk DataFrame, menghitung data statistik, serta operasi eksport ke format CSV.
- `matplotlib`: Pustaka pada Python yang ditujukan untuk membuat grafik/visualisasi dari data angka DataFrame secara dua dimensi.
- `json` dan `time`: Pustaka standar Python bawaan untuk membaca JSON dari MQTT dan merekam timestamp saat penerimaan data.

### 6. Modifikasi program agar menggunakan metode moving average (jendela 5 data) pada data hasil CSV sebelum dihitung z-score-nya, kemudian bandingkan jumlah anomali yang terdeteksi

```py
import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "KelompokDediPyka/data"  # Samakan dengan topicData pada ESP32
JUMLAH_SAMPEL = 30  # Jumlah data yang dikumpulkan

data = []

def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker, kode:", reason_code)
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        suhu = float(payload["suhu"])
    except (ValueError, KeyError):
        print("Data tidak valid, diabaikan")
        return

    data.append({"waktu": time.strftime("%Y-%m-%d %H:%M:%S"), "suhu": suhu})
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}")

    if len(data) >= JUMLAH_SAMPEL:
        client.disconnect()

# Tahap 1: Akuisisi data dari MQTT
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT)
client.loop_forever()

# Tahap 2: Simpan ke CSV
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("Data tersimpan di data_suhu.csv")

# Tahap 3: Analisis statistik dan deteksi anomali menggunakan data Moving Average
# Menghitung Moving Average dengan window=5 pada kolom suhu mentah
df["suhu_ma"] = df["suhu"].rolling(window=5, min_periods=1).mean() 

# Menghitung nilai rata-rata dan standar deviasi dari data Moving Average
rata2_ma = df["suhu_ma"].mean()
std_ma = df["suhu_ma"].std()

# Perhitungan z-score menggunakan data suhu_ma alih-alih data suhu mentah
if std_ma == 0:
    df["zscore"] = 0.0
else:
    df["zscore"] = (df["suhu_ma"] - rata2_ma) / std_ma

# Menentukan batas anomali berdasarkan filter z-score (batas |z| > 2)
df["anomali"] = df["zscore"].abs() > 2

print("=== Hasil Deteksi dengan Pre-Filter Moving Average ===")
print(f"Rata-rata (MA)  : {rata2_ma:.2f}")
print(f"Std deviasi(MA) : {std_ma:.2f}")
print(f"Jumlah anomali  : {df['anomali'].sum()}") 
# Catatan Perbandingan: Jumlah anomali yang terdeteksi biasanya akan lebih SEDIKIT 
# dibandingkan tanpa Moving Average. Hal ini terjadi karena MA "menghaluskan" 
# lonjakan tajam dari noise/anomali lokal, sehingga nilai Z-Score dari puncak 
# anomali tidak setinggi sebelumnya (tersebar).

# Tahap 4: Visualisasi
plt.figure(figsize=(10, 4))
plt.plot(df.index, df["suhu"], marker="o", color="lightgray", label="Suhu Mentah", alpha=0.6) # Menampilkan data mentah sbg background
plt.plot(df.index, df["suhu_ma"], marker="s", color="blue", label="Suhu (Moving Avg 5)") # Menampilkan data MA utama
anom = df[df["anomali"]]
plt.scatter(anom.index, anom["suhu_ma"], color="red", s=100, label="Anomali (di MA)", zorder=3) # Anomali pada garis MA
plt.axhline(rata2_ma, color="gray", linestyle="--", label="Rata-rata MA")
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Data Suhu (Moving Average) dan Deteksi Anomali")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu_ma.png")
plt.show()
```

## Dokumentasi

<img width="2810" height="1547" alt="rangkaian percobaan" src="https://github.com/user-attachments/assets/31c2dfeb-4a8f-4afa-9b90-a4afde8123a5" /> <br>
Rangkaian percobaan 5A dan 5B <br><br>

<img width="955" height="1016" alt="p5a" src="https://github.com/user-attachments/assets/70ce3849-aa6b-4501-8d67-a34984b3dd0e" /> <br>
Serial Monitor Percobaan 5A <br><br>

<img width="895" height="547" alt="p5b sm" src="https://github.com/user-attachments/assets/6e8ae824-40b9-45cd-9d7e-9805215209ac" /> <br>
Serial Monitor Percobaan 5B  <br><br>

<img width="1917" height="1020" alt="vscode" src="https://github.com/user-attachments/assets/d4de4048-ff79-4bc3-b38e-9a58ab56c56b" /> <br>
terminal vscode <br><br>

<img width="1000" height="400" alt="graph" src="https://github.com/user-attachments/assets/5927a555-a3e8-4fdf-851f-e80c4e614607" /> <br>
grafik_suhu <br><br>

