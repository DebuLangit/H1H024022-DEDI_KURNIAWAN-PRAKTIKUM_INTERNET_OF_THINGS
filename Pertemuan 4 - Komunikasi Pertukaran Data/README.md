# Percobaan 4A: Subscribe dan Deserialisasi Data JSON untuk Kendali Aktuator

## 1. Detail Percobaan

Percobaan 4A bertujuan mendemonstrasikan penerimaan perintah dari broker MQTT ke ESP8266. ESP8266 terhubung ke WiFi dan broker HiveMQ dengan melakukan subscribe pada topic tertentu. Pesan JSON yang diterima diproses melalui callback untuk mengambil perintah ON atau OFF, kemudian digunakan untuk mengendalikan LED.

## 2. Penjelasan Kode dan Fungsi (Function)

```cpp
// Library untuk menghubungkan ESP8266 ke jaringan WiFi
#include <ESP8266WiFi.h>

// Library untuk komunikasi menggunakan protokol MQTT
#include <PubSubClient.h>

// Library untuk membaca dan mengolah data dalam format JSON
#include <ArduinoJson.h>


// Nama jaringan WiFi yang akan digunakan
const char* ssid = "MamaLime";

// Password jaringan WiFi
const char* password = "sayaakanlawan";

// Alamat broker MQTT yang digunakan
const char* mqttServer = "broker.hivemq.com";

// Port MQTT standar tanpa enkripsi
const int mqttPort = 1883;

// Topik MQTT yang digunakan untuk menerima perintah
const char* topicPerintah = "KelompokDediPyka/perintah";

// Nomor GPIO ESP8266 yang digunakan untuk mengendalikan LED
const int ledPin = 4;


// Membuat objek WiFiClient untuk koneksi jaringan
WiFiClient espClient;

// Membuat objek MQTT Client menggunakan koneksi WiFi
PubSubClient client(espClient);


// Fungsi callback akan dipanggil otomatis ketika pesan MQTT diterima
void callback(char* topic, byte* payload, unsigned int length) {

  // Variabel untuk menampung pesan yang diterima
  String pesan;

  // Membaca setiap karakter dari payload MQTT
  for (unsigned int i = 0; i < length; i++) {

    // Mengubah data byte menjadi karakter lalu memasukkannya ke String
    pesan += (char)payload[i];
  }


  // Menampilkan informasi bahwa pesan telah diterima
  Serial.print("Pesan diterima [");

  // Menampilkan nama topik MQTT
  Serial.print(topic);

  // Menampilkan tanda pemisah
  Serial.print("]: ");

  // Menampilkan isi pesan yang diterima
  Serial.println(pesan);


  // Membuat dokumen JSON untuk menyimpan hasil parsing
  JsonDocument doc;

  // Mengubah teks JSON menjadi struktur data yang dapat dibaca program
  DeserializationError error = deserializeJson(doc, pesan);


  // Memeriksa apakah proses parsing JSON mengalami kesalahan
  if (error) {

    // Menampilkan pesan kesalahan pada Serial Monitor
    Serial.print("Gagal parsing JSON: ");

    // Menampilkan jenis kesalahan JSON
    Serial.println(error.c_str());

    // Menghentikan fungsi callback jika JSON tidak valid
    return;
  }


  // Mengambil nilai "perintah" dari data JSON
  const char* perintah = doc["perintah"];


  // Memeriksa apakah perintah yang diterima adalah "ON"
  if (String(perintah) == "ON") {

    // Menyalakan LED dengan memberikan logika HIGH
    digitalWrite(ledPin, HIGH);

    // Menampilkan status aktuator
    Serial.println("Aktuator: ON");


  // Jika perintah yang diterima adalah "OFF"
  } else if (String(perintah) == "OFF") {

    // Mematikan LED dengan memberikan logika LOW
    digitalWrite(ledPin, LOW);

    // Menampilkan status aktuator
    Serial.println("Aktuator: OFF");
  }
}


// Fungsi untuk menghubungkan ESP8266 ke jaringan WiFi
void hubungkanWiFi() {

  // Memulai koneksi menggunakan SSID dan password
  WiFi.begin(ssid, password);

  // Menampilkan informasi proses koneksi
  Serial.print("Menghubungkan ke WiFi");


  // Mengulang selama ESP8266 belum terhubung
  while (WiFi.status() != WL_CONNECTED) {

    // Memberikan jeda selama 500 milidetik
    delay(500);

    // Menampilkan titik sebagai indikator proses koneksi
    Serial.print(".");
  }


  // Menampilkan pesan ketika WiFi berhasil terhubung
  Serial.println("\nWiFi berhasil terhubung!");
}


// Fungsi untuk menghubungkan ESP8266 ke broker MQTT
void hubungkanMQTT() {

  // Mengulang selama perangkat belum terhubung ke MQTT
  while (!client.connected()) {

    // Menampilkan status proses koneksi
    Serial.print("Menghubungkan ke broker MQTT...");


    // Membuat ID client MQTT secara acak
    // random() digunakan agar ID tidak mudah sama dengan client lain
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);


    // Mencoba menghubungkan client ke broker MQTT
    if (client.connect(clientId.c_str())) {

      // Menampilkan bahwa koneksi berhasil
      Serial.println("berhasil terhubung!");


      // Berlangganan ke topik perintah
      client.subscribe(topicPerintah);


      // Menampilkan topik yang digunakan
      Serial.print("Subscribe ke topic: ");
      Serial.println(topicPerintah);


    } else {

      // Menampilkan pesan jika koneksi gagal
      Serial.print("gagal, rc=");

      // Menampilkan kode status koneksi MQTT
      Serial.print(client.state());

      // Memberikan informasi bahwa perangkat akan mencoba kembali
      Serial.println(" coba lagi dalam 2 detik");

      // Menunggu 2 detik sebelum mencoba kembali
      delay(2000);
    }
  }
}


// Fungsi setup() hanya dijalankan satu kali ketika ESP8266 mulai
void setup() {

  // Memulai komunikasi Serial dengan baud rate 115200
  Serial.begin(115200);

  // Mengatur pin LED sebagai OUTPUT
  pinMode(ledPin, OUTPUT);

  // Memastikan LED dalam kondisi mati saat awal
  digitalWrite(ledPin, LOW);

  // Menghubungkan ESP8266 ke WiFi
  hubungkanWiFi();

  // Menentukan alamat dan port broker MQTT
  client.setServer(mqttServer, mqttPort);

  // Mendaftarkan fungsi callback untuk menangani pesan MQTT
  client.setCallback(callback);
}


// Fungsi loop() dijalankan terus-menerus
void loop() {

  // Jika koneksi MQTT terputus, lakukan koneksi ulang
  if (!client.connected()) {
    hubungkanMQTT();
  }

  // Memproses komunikasi MQTT dan memeriksa pesan yang masuk
  // Fungsi ini harus dipanggil secara terus-menerus
  client.loop();
}
```


## 3. Penjelasan Percabangan dan Kondisional

*   `if (error)`: Mengevaluasi apakah proses deserialisasi (parsing) format JSON dari string ke objek JSON berhasil atau gagal. Jika gagal (format tidak valid), program akan mencetak pesan error dan fungsi callback dihentikan dengan `return`.
*   `if (String(perintah) == "ON")` dan `else if (String(perintah) == "OFF")`: Memeriksa isi dari variabel `perintah`. Jika bernilai "ON", maka LED dinyalakan (atau diatur kecerahannya pada versi modifikasi). Jika "OFF", LED dimatikan.
*   `while (WiFi.status() != WL_CONNECTED)`: Menahan (blocking) eksekusi program dan terus mencetak titik (`.`) selama mikrokontroler belum mendapatkan alamat IP dari router WiFi.
*   `while (!client.connected())`: Looping untuk memastikan perangkat terhubung ke broker MQTT. Jika koneksi terputus atau belum terhubung, perangkat akan terus mencoba menghubungkan ulang (reconnect) dan melakukan subscribe ulang.
*   `if (client.connect(clientId.c_str()))`: Memeriksa apakah upaya koneksi ke broker MQTT dengan ID acak berhasil dilakukan.
*   `if (!client.connected())`: Memeriksa apakah koneksi MQTT terputus di dalam `loop()`.


## 4. Library yang Digunakan

*   `ESP8266WiFi.h`: Mengelola koneksi jaringan WiFi (menghubungkan ESP8266 ke access point).
*   `PubSubClient.h`: Menyediakan fungsi untuk koneksi MQTT, termasuk melakukan *publish* data dan *subscribe* ke topik tertentu.
*   `ArduinoJson.h`: Digunakan untuk mengubah (serialize) objek data menjadi format string JSON atau membaca (deserialize/parsing) pesan string JSON yang masuk menjadi variabel yang bisa dibaca program.

## 5. Modifikasi program kontrol kecerahan (PWM) berdasarkan nilai `intensitas` dari payload JSON

```cpp
// Mengimpor library yang dibutuhkan untuk WiFi, MQTT, dan manipulasi data JSON
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Konfigurasi kredensial WiFi dan broker MQTT
const char* ssid = "MamaLime";
const char* password = "sayaakanlawan";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* topicPerintah = "KelompokDediPyka/perintah";

// Pin untuk LED
const int ledPin = 4;

// Inisialisasi object client WiFi dan MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Fungsi callback dieksekusi otomatis ketika pesan masuk dari topik yang di-subscribe
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  // Menggabungkan byte payload menjadi tipe data String
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }
  
  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  // Membuat dokumen JSON dan memparsing string pesan masuk
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);

  // Percabangan untuk menangani error jika format pesan bukan JSON valid
  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return; // Keluar dari fungsi jika error
  }

  // Mengambil nilai string "perintah" dari JSON
  const char* perintah = doc["perintah"];
  
  // [MODIFIKASI] Mengambil nilai integer "intensitas" dari JSON (misal 0 - 1023 untuk ESP8266)
  int intensitas = doc["intensitas"]; 

  // Percabangan logika aktuator berdasarkan perintah
  if (String(perintah) == "ON") {
    // [MODIFIKASI] Menggunakan analogWrite untuk mengatur kecerahan LED sesuai nilai intensitas (PWM)
    analogWrite(ledPin, intensitas);
    Serial.print("Aktuator: ON, Intensitas: ");
    // [MODIFIKASI] Menampilkan nilai intensitas di Serial Monitor
    Serial.println(intensitas); 
  } else if (String(perintah) == "OFF") {
    // [MODIFIKASI] Menggunakan analogWrite dengan nilai 0 untuk mematikan LED sepenuhnya
    analogWrite(ledPin, 0); 
    Serial.println("Aktuator: OFF");
  }
}

// Fungsi untuk menghubungkan ESP8266 ke router WiFi
void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

// Fungsi untuk membuat koneksi ke broker MQTT
void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    // Membuat Client ID yang unik menggunakan random hex
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    
    // Jika berhasil terhubung ke broker
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
      // Wajib melakukan subscribe setiap kali koneksi MQTT (re)connect
      client.subscribe(topicPerintah); 
      Serial.print("Subscribe ke topic: ");
      Serial.println(topicPerintah);
    } else {
      // Jika gagal, tampilkan status kode error
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

// Fungsi inisialisasi yang berjalan sekali saat mikrokontroler dinyalakan
void setup() {
  Serial.begin(115200); // Kecepatan komunikasi serial
  
  pinMode(ledPin, OUTPUT); // Mengatur pin LED sebagai OUTPUT
  digitalWrite(ledPin, LOW); // Memastikan LED mati di awal
  
  hubungkanWiFi(); // Panggil fungsi koneksi WiFi
  
  client.setServer(mqttServer, mqttPort); // Tentukan alamat broker MQTT
  client.setCallback(callback); // Daftarkan fungsi callback untuk menangani pesan masuk
}

// Fungsi utama yang berjalan terus-menerus
void loop() {
  // Jika MQTT terputus, coba hubungkan kembali
  if (!client.connected()) {
    hubungkanMQTT();
  }
  // Menjaga koneksi MQTT tetap aktif dan memproses antrian pesan masuk
  client.loop(); 
}
```


# Percobaan 4B: Pertukaran Data Dua Arah (Publish dan Subscribe Secara Bersamaan)

## 1. Detail Percobaan

Percobaan 4B bertujuan mendemonstrasikan komunikasi dua arah (*bidirectional*) secara *non-blocking* menggunakan MQTT. Mikrokontroler berperan sebagai *Publisher* dan *Subscriber*, dengan membaca suhu dari sensor DHT11 setiap 5 detik menggunakan `millis()`, kemudian mengirimkannya dalam format JSON. Secara bersamaan, mikrokontroler menerima perintah melalui topik MQTT untuk mengendalikan LED dan buzzer secara responsif tanpa mengganggu proses pembacaan sensor.


## 2. Penjelasan Kode dan Fungsi (Function)

```cpp
// Library untuk koneksi WiFi pada ESP8266
#include <ESP8266WiFi.h>

// Library untuk komunikasi MQTT
#include <PubSubClient.h>

// Library untuk mengolah data JSON
#include <ArduinoJson.h>

// Library untuk menggunakan sensor DHT
#include <DHT.h>


// Nama jaringan WiFi
const char* ssid = "MamaLime";

// Password jaringan WiFi
const char* password = "sayaakanlawan";

// Alamat broker MQTT
const char* mqttServer = "broker.hivemq.com";

// Port MQTT
const int mqttPort = 1883;

// Topik untuk mengirim data sensor
const char* topicData = "KelompokDediPyka/data";

// Topik untuk menerima perintah
const char* topicPerintah = "KelompokDediPyka/perintah";


// GPIO yang digunakan untuk DATA sensor DHT11
#define DHTPIN 5

// Menentukan jenis sensor yang digunakan adalah DHT11
#define DHTTYPE DHT11


// GPIO yang digunakan untuk LED
const int ledPin = 4;


// Membuat objek sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Membuat objek koneksi WiFi
WiFiClient espClient;

// Membuat objek MQTT Client
PubSubClient client(espClient);


// Menyimpan waktu terakhir data sensor dikirim
unsigned long waktuTerakhirPublish = 0;

// Menentukan interval pengiriman data, yaitu 5000 ms atau 5 detik
const long intervalPublish = 5000;

// Fungsi callback dipanggil ketika terdapat pesan MQTT yang masuk
void callback(char* topic, byte* payload, unsigned int length) {

  // Variabel untuk menyimpan pesan yang diterima
  String pesan;


  // Membaca seluruh karakter pada payload
  for (unsigned int i = 0; i < length; i++) {

    // Mengubah byte menjadi karakter
    pesan += (char)payload[i];
  }


  // Membuat objek untuk menyimpan data JSON
  JsonDocument doc;


  // Mengubah pesan JSON menjadi struktur data
  // Jika parsing gagal, fungsi langsung dihentikan
  if (deserializeJson(doc, pesan)) return;


  // Mengambil nilai "perintah" dari JSON
  const char* perintah = doc["perintah"];


  // Jika perintah adalah ON, LED HIGH.
  // Jika selain ON, LED dibuat LOW.
  digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);


  // Menampilkan perintah yang diterima pada Serial Monitor
  Serial.print("Perintah diterima -> Aktuator: ");

  // Menampilkan nilai perintah
  Serial.println(perintah);
}

// Fungsi untuk menghubungkan ESP8266 ke WiFi
void hubungkanWiFi() {

  // Memulai koneksi menggunakan SSID dan password
  WiFi.begin(ssid, password);


  // Menunggu sampai ESP8266 berhasil terhubung
  while (WiFi.status() != WL_CONNECTED) {

    // Memberikan jeda 500 ms
    delay(500);
  }


  // Menampilkan pesan setelah berhasil terhubung
  Serial.println("WiFi berhasil terhubung!");
}

// Fungsi untuk menghubungkan ESP8266 ke broker MQTT
void hubungkanMQTT() {

  // Mengulang koneksi selama MQTT belum terhubung
  while (!client.connected()) {

    // Membuat ID client MQTT secara acak
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);


    // Mencoba menghubungkan ESP8266 ke broker
    if (client.connect(clientId.c_str())) {

      // Subscribe ke topik yang digunakan untuk menerima perintah
      client.subscribe(topicPerintah);

      // Menampilkan informasi koneksi
      Serial.println("Terhubung dan subscribe topic perintah");


    } else {

      // Jika gagal, tunggu 2 detik sebelum mencoba kembali
      delay(2000);
    }
  }
}

// Fungsi setup() dijalankan satu kali ketika ESP8266 dinyalakan
void setup() {

  // Memulai komunikasi Serial dengan baud rate 115200
  Serial.begin(115200);

  // Mengatur GPIO LED sebagai OUTPUT
  pinMode(ledPin, OUTPUT);

  // Memulai sensor DHT11
  dht.begin();

  // Menghubungkan ESP8266 ke WiFi
  hubungkanWiFi();

  // Menentukan alamat dan port broker MQTT
  client.setServer(mqttServer, mqttPort);

  // Menentukan fungsi callback untuk pesan MQTT
  client.setCallback(callback);
}

// Fungsi loop() berjalan terus-menerus
void loop() {

  // Memastikan koneksi MQTT tetap tersedia
  if (!client.connected()) hubungkanMQTT();


  // Memproses pesan MQTT yang masuk
  client.loop();


  // Mengecek apakah sudah waktunya mengirim data sensor
  if (millis() - waktuTerakhirPublish > intervalPublish) {

    // Menyimpan waktu pengiriman terakhir
    waktuTerakhirPublish = millis();


    // Membaca suhu dari sensor DHT11
    float suhu = dht.readTemperature();


    // Memastikan hasil pembacaan sensor valid
    if (!isnan(suhu)) {

      // Membuat dokumen JSON
      JsonDocument doc;


      // Menyimpan nilai suhu ke dalam JSON
      doc["suhu"] = suhu;


      // Menyediakan buffer untuk menyimpan JSON
      char buffer[128];


      // Mengubah data JSON menjadi teks
      serializeJson(doc, buffer);


      // Mengirim data JSON ke topicData melalui MQTT
      client.publish(topicData, buffer);


      // Menampilkan data yang dikirim pada Serial Monitor
      Serial.print("Data terkirim: ");

      // Menampilkan isi JSON
      Serial.println(buffer);
    }
  }
}
```


## 3. Penjelasan Percabangan dan Kondisional

* `if (deserializeJson(doc, pesan)) return;`: Jalan pintas untuk mengecek error. Jika parsing JSON gagal (mengembalikan nilai true pada kondisi error), fungsi `callback` langsung dibatalkan.

* `String(perintah) == "ON" ? HIGH : LOW`: Ternary operator yang bertindak sebagai if-else sebaris. Jika perintah bernilai "ON", fungsi ini mengembalikan status logika `HIGH`. Jika tidak, mengembalikan `LOW`.

* `if (!isnan(suhu))`: Validasi pembacaan sensor. Kondisi ini memastikan data yang dikirim hanya dieksekusi jika suhu berupa angka valid (bukan Not a Number akibat kegagalan baca sensor DHT).

* `if (millis() - waktuTerakhirPublish > intervalPublish)`: Pola pengkondisian non-blocking timer. Menggantikan `delay()` dengan membandingkan selisih waktu sistem yang berjalan saat ini `(millis())` dengan waktu terakhir perintah dieksekusi.

* `while (WiFi.status() != WL_CONNECTED)`: Penantian sambungan jaringan WiFi.

* `while (!client.connected())`: Penantian sambungan broker MQTT.
  
* `if (client.connect(...)) ... else`: Mengecek keberhasilan koneksi ke broker MQTT.
  
* `if (!client.connected())`: Menjaga koneksi MQTT tetap aktif dalam perulangan utama.



## 4. Library yang Digunakan

* `ESP8266WiFi.h`, `PubSubClient.h`, `ArduinoJson.h`: Berfungsi sama seperti pada Percobaan 4A.

* `DHT.h`: Library khusus dari Adafruit untuk membaca data suhu dan kelembaban langsung dari pin data sensor DHT11/DHT22 dengan mudah tanpa menulis protokol one-wire secara manual.

## 5. Modifikasi program agar menambahkan satu topic perintah baru untuk mengendalikan aktuator kedua (misalnya buzzer), dengan fungsi callback yang dapat membedakan topic mana yang menerima pesan

```cpp
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* ssid = "MamaLime";
const char* password = "sayaakanlawan";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

// Deklarasi topik-topik MQTT
const char* topicData = "KelompokDediPyka/data";
const char* topicPerintah = "KelompokDediPyka/perintah";
// [MODIFIKASI] Menambahkan topik baru khusus untuk mengendalikan buzzer
const char* topicBuzzer = "KelompokDediPyka/buzzer"; 

#define DHTPIN 5
#define DHTTYPE DHT11

const int ledPin = 4;
// [MODIFIKASI] Mendefinisikan pin mikrokontroler yang terhubung ke aktuator Buzzer (misal GPIO 14 / D5)
const int buzzerPin = 14; 

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient; 
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000;

// Fungsi callback untuk menangani semua pesan masuk dari topik yang disubscribe
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }
  
  JsonDocument doc;
  if (deserializeJson(doc, pesan)) return; // abaikan jika parsing gagal

  // [MODIFIKASI] Percabangan untuk memeriksa jika pesan berasal dari topik LED (topicPerintah)
  if (strcmp(topic, topicPerintah) == 0) {
    const char* perintah = doc["perintah"];
    digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
    Serial.print("LED Perintah: ");
    Serial.println(perintah);
  } 
  // [MODIFIKASI] Percabangan (else if) memeriksa jika pesan berasal dari topik Buzzer (topicBuzzer)
  else if (strcmp(topic, topicBuzzer) == 0) {
    // [MODIFIKASI] Mengekstrak kunci "status" dari pesan JSON untuk topik buzzer (contoh: {"status": "ON"})
    const char* status = doc["status"]; 
    // [MODIFIKASI] Menyalakan (HIGH) atau mematikan (LOW) pin buzzer berdasarkan nilai dari "status"
    digitalWrite(buzzerPin, String(status) == "ON" ? HIGH : LOW);
    Serial.print("Buzzer Perintah: ");
    Serial.println(status);
  }
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("WiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      // Subscribe ke topik kontrol LED
      client.subscribe(topicPerintah);
      // [MODIFIKASI] Menambahkan pendaftaran (subscribe) agar mikrokontroler juga mendengarkan pesan dari topik Buzzer
      client.subscribe(topicBuzzer); 
      Serial.println("Terhubung dan subscribe ke topik perintah LED & Buzzer");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  // [MODIFIKASI] Mengatur pin buzzer sebagai OUTPUT penggerak aktuator
  pinMode(buzzerPin, OUTPUT); 
  // [MODIFIKASI] Memastikan buzzer dalam keadaan mati saat perangkat pertama kali menyala
  digitalWrite(buzzerPin, LOW); 
  
  dht.begin();
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) hubungkanMQTT();
  client.loop(); 

  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();
    float suhu = dht.readTemperature();
    if (!isnan(suhu)) {
      JsonDocument doc;
      doc["suhu"] = suhu;
      char buffer[128];
      serializeJson(doc, buffer);
      client.publish(topicData, buffer); // Mengirim data ke topik KelompokDediPyka/data
      Serial.print("Data terkirim: ");
      Serial.println(buffer);
    }
  }
}
```

# Dokumentasi

<img width="614" height="250" alt="gambar" src="https://github.com/user-attachments/assets/5b2ebfb0-00f5-4cf0-b633-47c74681abfb" /> <br>
Rangkaian percobaan 4A <br><br>

<img width="620" height="258" alt="gambar" src="https://github.com/user-attachments/assets/c3b3cfa9-0d3a-40ca-9006-f798c20616a2" /> <br>
Rangkaian percobaan 4B <br><br>

<img width="719" height="701" alt="gambar" src="https://github.com/user-attachments/assets/94684451-46a4-40aa-93b5-9a49362ef544" /> <br>
Serial Monitor Percobaan 4A <br><br>

<img width="713" height="585" alt="gambar" src="https://github.com/user-attachments/assets/3adc211f-951e-4c7b-831e-89ab5ab70c09" /> <br>
Serial Monitor Percobaan 4B  <br><br>

<img width="679" height="619" alt="gambar" src="https://github.com/user-attachments/assets/bcd65fa1-2c37-4c87-aee5-ee47fc8fe3de" /> <br>
MQTT Explorer 4A <br><br>

<img width="677" height="680" alt="gambar" src="https://github.com/user-attachments/assets/26f7d3b4-3f39-468d-8387-72023bc5c67b" /> <br>
MQTT Explorer 4B <br><br>

