# Percobaan 3A : Komunikasi Data Menggunakan HTTP

## 1. Detail Percobaan
Percobaan 3A bertujuan untuk mendemonstrasikan proses pengiriman data sensor dari mikrokontroler ke server internet menggunakan protokol **HTTP dengan metode POST**. Mikrokontroler ESP8266/ESP32 terlebih dahulu terhubung ke jaringan WiFi, kemudian data simulasi sensor berupa suhu sebesar 28,5°C dan kelembaban 65,0% dikemas dalam format **JSON**. Komunikasi dilakukan melalui jalur **HTTPS** menggunakan `WiFiClientSecure`, dengan validasi sertifikat SSL dinonaktifkan menggunakan `setInsecure()` untuk keperluan pengujian. Data selanjutnya dikirim ke endpoint pengujian `https://httpbin.org/post`, yang memberikan respons berupa data yang diterima sehingga keberhasilan proses pengiriman dapat diverifikasi. Proses pengiriman data tersebut dilakukan secara berulang setiap 10 detik.


## 2. Penjelasan Kode dan Fungsi (Function)

```cpp
#include <ESP8266WiFi.h>       // Fungsi: Menangani koneksi modul WiFi ESP8266
#include <ESP8266HTTPClient.h> // Fungsi: Menyediakan fungsi HTTP Client (GET, POST, dll)
#include <WiFiClientSecure.h>  // Fungsi: Menyediakan koneksi TCP dengan enkripsi TLS/SSL (HTTPS)
#include <ArduinoJson.h>       // Fungsi: Membuat, memparsing, dan memanipulasi format data JSON

// Deklarasi kredensial WiFi (SSID dan Password)
const char* ssid = "404";
const char* password = "pikap077";
// Endpoint server tujuan untuk menerima request POST
const char* serverUrl = "https://httpbin.org/post"; 

// Fungsi setup() berjalan satu kali saat mikrokontroler dinyalakan
void setup() {
  Serial.begin(115200);        // Memulai komunikasi serial dengan baudrate 115200 bps
  WiFi.begin(ssid, password);  // Memerintahkan modul WiFi untuk mulai terhubung ke router
  Serial.print("Menghubungkan ke WiFi");
  
  // Menahan program di sini selama WiFi belum terkoneksi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);                // Tunggu 500ms
    Serial.print(".");         // Cetak titik sebagai indikator loading
  }
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
}

// Fungsi loop() berjalan berulang-ulang tanpa henti
void loop() {
  // Mengecek apakah koneksi WiFi masih terhubung
  if (WiFi.status() == WL_CONNECTED) {
    
    // 1. Membuat objek client TCP yang mendukung HTTPS
    WiFiClientSecure client;
    
    // 2. Menonaktifkan verifikasi sertifikat SSL server (hanya untuk testing agar praktis)
    client.setInsecure(); 

    // Membuat objek untuk mengelola HTTP Request
    HTTPClient http;
    
    // 3. Memulai persiapan koneksi ke URL menggunakan client HTTPS tadi
    http.begin(client, serverUrl); 
    
    // Memberi tahu server bahwa data yang akan kita kirim berformat JSON
    http.addHeader("Content-Type", "application/json");
    
    // Membuat objek penampung dokumen JSON di memori (ArduinoJson versi 7 ke atas)
    JsonDocument doc;
    doc["suhu"] = 28.5;       // Menambahkan pasangan key "suhu" dengan value 28.5
    doc["kelembaban"] = 65.0; // Menambahkan pasangan key "kelembaban" dengan value 65.0
    
    String requestBody;       // Variabel penampung teks
    // Mengubah struktur objek JSON menjadi teks String (Serialization)
    serializeJson(doc, requestBody);
    
    Serial.print("Mengirim data: ");
    Serial.println(requestBody);
    
    // Mengirim HTTP POST request membawa string JSON dan menangkap kode balasannya
    int httpResponseCode = http.POST(requestBody);
    
    // Evaluasi kode balasan
    if (httpResponseCode > 0) {
      Serial.print("Kode Response HTTP: ");
      Serial.println(httpResponseCode);  // Jika sukses, biasanya bernilai 200 (OK)
      Serial.println("Isi Response:");
      Serial.println(http.getString());  // Mengambil dan mencetak isi balasan dari server
    } else {
      Serial.print("Pengiriman gagal, kode error: ");
      Serial.println(httpResponseCode);  // Kode negatif menunjukkan error jaringan/klien
    }
    
    // Menutup koneksi HTTP untuk membersihkan memori mikrokontroler
    http.end();
  }
  
  // Memberi jeda 10.000 milidetik (10 detik) sebelum siklus loop mengulang dari awal
  delay(10000); 
}
```


## 3. Penjelasan Percabangan dan Kondisional

Terdapat 3 struktur kontrol/kondisional penting dalam program ini:
- `while (WiFi.status() != WL_CONNECTED)` (Perulangan Kondisional):
Program akan terjebak/tertahan di dalam loop ini selama status WiFi belum WL_CONNECTED. Ini mencegah program berlanjut menjalankan perintah HTTP saat perangkat belum punya akses internet.

- `if (WiFi.status() == WL_CONNECTED)` (Percabangan):
Berada di dalam loop(). Ini berfungsi sebagai proteksi ganda. Jika di tengah jalan tiba-tiba koneksi WiFi terputus, blok kode HTTP POST tidak akan dieksekusi, sehingga mikrokontroler tidak mengalami crash (error fatal).

- `if (httpResponseCode > 0) ... else ...` (Percabangan):
  Berfungsi mengevaluasi hasil pengiriman POST.
  - Jika nilai > 0 (misal 200, 404, 500), artinya request berhasil keluar dan server memberikan balasan HTTP murni.
  - Jika bernilai negatif (masuk ke blok else), artinya request gagal bahkan sebelum sampai ke server (misal: koneksi terputus, DNS gagal, atau timeout dari sisi mikrokontroler).

## 4. Library yang Digunakan

- `ESP8266WiFi.h`: Pustaka inti untuk mengatur lapisan jaringan nirkabel (menghubungkan SSID, mendapatkan IP, mengecek status jaringan).

- `WiFiClientSecure.h`: Pustaka lapisan transpor (TCP). Modifikasi dari `WiFiClient` standar agar mendukung enkripsi pertukaran data (SSL/TLS) pada port 443.

- `ESP8266HTTPClient.h`: Pustaka lapisan aplikasi. Menyederhanakan proses pengiriman dan penerimaan header dan body protokol HTTP (termasuk fungsi POST(), addHeader(), dan penguraian kode respons).

- `ArduinoJson.h`: Pustaka dari pihak ketiga (Benoit Blanchon) yang berfungsi memanipulasi format teks JSON, seperti merakit data (serialize) sebelum dikirim, atau mengurai data (deserialize) yang masuk dari server.

## 5. Modifikasi Program (ESP32 + Penambahan Waktu millis())

```cpp
// 1. Penyesuaian Library untuk board ESP32
#include <WiFi.h>              // Library WiFi bawaan khusus untuk ESP32
#include <HTTPClient.h>        // Library HTTP Client bawaan khusus untuk ESP32
#include <WiFiClientSecure.h>  // Library TCP Secure (HTTPS) untuk ESP32
#include <ArduinoJson.h>       // Library JSON (sama untuk semua board Arduino)

// 2. Deklarasi kredensial jaringan
const char* ssid = "404";      // Nama WiFi (SSID)
const char* password = "pikap077"; // Kata sandi WiFi
const char* serverUrl = "https://httpbin.org/post"; // URL endpoint server untuk uji coba POST

void setup() {
  Serial.begin(115200);        // Memulai monitor serial pada baudrate 115200
  WiFi.begin(ssid, password);  // Memulai proses koneksi ESP32 ke jaringan WiFi
  
  Serial.print("Menghubungkan ke WiFi");
  // Tunggu hingga status WiFi menjadi WL_CONNECTED
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);                // Jeda setengah detik antar pengecekan
    Serial.print(".");         // Cetak titik untuk indikasi proses masih berjalan
  }
  Serial.println();            // Ganti baris pada serial monitor
  Serial.println("WiFi berhasil terhubung!"); // Pesan sukses koneksi
}

void loop() {
  // Mengecek apakah ESP32 masih terhubung ke WiFi
  if (WiFi.status() == WL_CONNECTED) {
    
    WiFiClientSecure client;   // Membuat instance client jaringan terenkripsi
    client.setInsecure();      // Bypass validasi sertifikat (PENTING untuk test HTTPS lokal/publik)

    HTTPClient http;           // Membuat instance HTTP request
    
    // Memulai persiapan target HTTP ke URL tujuan dengan jalur secure
    http.begin(client, serverUrl); 
    
    // Menambahkan header agar server membaca payload sebagai JSON
    http.addHeader("Content-Type", "application/json");
    
    // Membuat penampung struktur JSON
    JsonDocument doc;
    doc["suhu"] = 28.5;        // Mengisi nilai suhu (statis)
    doc["kelembaban"] = 65.0;  // Mengisi nilai kelembaban (statis)
    
    // MODIFIKASI: Menambahkan data waktu (millis) ke dalam JSON
    // millis() mengembalikan jumlah milidetik sejak mikrokontroler dinyalakan
    doc["waktu_ms"] = millis(); 
    
    String requestBody;        // Variabel untuk menampung teks JSON final
    // Mengubah objek "doc" menjadi string dan menyimpannya di "requestBody"
    serializeJson(doc, requestBody);
    
    Serial.print("Mengirim data: "); // Mencetak log ke serial monitor
    Serial.println(requestBody);     // Mencetak isi JSON (contoh: {"suhu":28.5,"kelembaban":65,"waktu_ms":12500})
    
    // Menembakkan HTTP POST dengan membawa teks JSON
    int httpResponseCode = http.POST(requestBody);
    
    // Cek apakah HTTP berhasil mengirim dan menerima respons
    if (httpResponseCode > 0) {
      Serial.print("Kode Response HTTP: "); // Cetak keterangan
      Serial.println(httpResponseCode);     // Cetak kode dari server (200 berarti sukses)
      Serial.println("Isi Response:");      // Cetak keterangan
      Serial.println(http.getString());     // Cetak balasan server (httpbin memantulkan isi request)
    } else {
      // Jika terjadi error dari sisi mikrokontroler saat proses pengiriman
      Serial.print("Pengiriman gagal, kode error: "); 
      Serial.println(httpResponseCode);     // Cetak error (contoh: -1 artinya connection refused/failed)
    }
    
    // Menutup koneksi HTTP untuk membersihkan resource/memori ESP32
    http.end();
  }
  
  // Tunggu 10.000 milidetik (10 detik) sebelum ESP32 mengulang proses pengukuran dan pengiriman
  delay(10000); 
}
```


# Percobaan 3B: Komunikasi Data melalui MQTT

## 1. Detail Percobaan

Percobaan ini mendemonstrasikan pengiriman data sensor simulasi dari mikrokontroler ESP8266 menggunakan protokol MQTT (Message Queuing Telemetry Transport). ESP8266 bertindak sebagai Publisher yang mengemas data suhu (28.5) dan kelembaban (65.0) dalam format JSON, lalu mengirimkannya ke alamat topik `pyka/esp8266/latihan`. Data ini dititipkan ke public broker gratis dari HiveMQ (`broker.hivemq.com`) pada port 1883. Proses pengiriman data (publish) dilakukan secara terus-menerus setiap 5 detik.

## 2. Penjelasan Kode dan Fungsi (Function)

```cpp
#include <ESP8266WiFi.h>   // Library untuk koneksi WiFi ESP8266
#include <PubSubClient.h>  // Library klien MQTT (Publish-Subscribe)
#include <ArduinoJson.h>   // Library untuk membuat dan mengelola data JSON

// ==========================
// WIFI
// ==========================
const char* ssid = "404";          // Nama jaringan WiFi
const char* password = "pikap077"; // Kata sandi WiFi

// ==========================
// MQTT
// ==========================
const char* mqttServer = "broker.hivemq.com"; // Alamat server broker MQTT publik
const int mqttPort = 1883;                    // Port standar protokol MQTT (tanpa enkripsi)
const char* mqttTopic = "pyka/esp8266/latihan"; // Nama kotak surat (topik) tujuan pengiriman data

// ==========================
// OBJECT
// ==========================
WiFiClient espClient;           // Membuat klien jaringan dasar TCP/IP
PubSubClient client(espClient); // Membangun klien MQTT di atas klien TCP/IP tadi

// ==========================
// FUNGSI HUBUNGKAN WIFI
// ==========================
// Fungsi ini bertugas mengkoneksikan ESP8266 ke router WiFi
void hubungkanWiFi() {
  WiFi.begin(ssid, password); // Mulai koneksi WiFi
  Serial.print("Menghubungkan ke WiFi");

  // Tahan program di sini selama WiFi belum terkoneksi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
  Serial.print("IP ESP8266: ");
  Serial.println(WiFi.localIP()); // Cetak alamat IP yang didapat dari router
}

// ==========================
// FUNGSI HUBUNGKAN MQTT
// ==========================
// Fungsi ini bertugas menghubungkan ESP8266 ke broker MQTT HiveMQ
void hubungkanMQTT() {
  // Looping terus menerus sampai klien MQTT terhubung ke broker
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");

    // Membuat ID Klien yang unik berdasarkan ID Chip ESP8266 (syarat wajib dari broker)
    String clientId = "ESP8266Client-";
    clientId += String(ESP.getChipId(), HEX);

    // Mencoba melakukan koneksi ke broker menggunakan ID Klien tersebut
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
    } else {
      // Jika gagal, cetak kode error (rc) dari broker
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000); // Tunggu 2 detik sebelum mencoba koneksi lagi
    }
  }
}

// ==========================
// SETUP
// ==========================
// Berjalan satu kali di awal untuk inisialisasi
void setup() {
  Serial.begin(115200); // Memulai komunikasi serial

  hubungkanWiFi(); // Panggil fungsi koneksi WiFi

  // Beritahu library MQTT alamat server dan port broker yang akan digunakan
  client.setServer(mqttServer, mqttPort);
}

// ==========================
// LOOP
// ==========================
// Berjalan terus-menerus
void loop() {
  // Cek apakah koneksi MQTT terputus; jika ya, panggil fungsi reconnect
  if (!client.connected()) {
    hubungkanMQTT();
  }

  // Wajib dipanggil rutin agar koneksi dengan broker tetap hidup (keep-alive)
  client.loop();

  // ==========================
  // MEMBUAT DATA JSON
  // ==========================
  JsonDocument doc; // Menyiapkan memori untuk objek JSON
  doc["suhu"] = 28.5;       // Memasukkan data simulasi suhu
  doc["kelembaban"] = 65.0; // Memasukkan data simulasi kelembaban

  char buffer[128]; // Wadah sementara (array karakter) untuk menyimpan teks JSON
  serializeJson(doc, buffer); // Mengonversi objek JSON menjadi teks dan menyimpannya di buffer

  // ==========================
  // PUBLISH KE MQTT
  // ==========================
  // Mengirim isi buffer ke alamat topik yang telah ditentukan
  bool berhasil = client.publish(mqttTopic, buffer);

  // Evaluasi apakah data berhasil meluncur ke broker
  if (berhasil) {
    Serial.print("Data terkirim ke topic ");
    Serial.print(mqttTopic);
    Serial.print(": ");
    Serial.println(buffer);
  } else {
    Serial.println("Gagal mengirim data!");
  }

  // Jeda 5 detik sebelum mengirim data berikutnya
  delay(5000);
}
```


## 3. Penjelasan Percabangan dan Kondisional

- `while (WiFi.status() != WL_CONNECTED)`: Perulangan penahan (blocking). Program tidak akan melanjutkan eksekusi ke baris berikutnya sampai mikrokontroler benar-benar mendapatkan koneksi dan IP dari router WiFi. Selama menunggu, ia mencetak titik (.).

- `while (!client.connected())`: Perulangan penahan di dalam fungsi `hubungkanMQTT()`. Berfungsi mengunci program dalam fase reconnect hingga ESP8266 berhasil berjabat tangan dengan broker MQTT.

- `if (client.connect(clientId.c_str()))`: Percabangan evaluasi koneksi. Jika permintaan koneksi ke broker diterima, program mencetak "berhasil terhubung!". Jika ditolak (blok else), ia mencetak status error dari broker dan menunda percobaan ulang selama 2 detik.

- `if (!client.connected())` (di dalam fungsi `loop`): Mekanisme auto-reconnect. Secara proaktif mengecek status koneksi setiap siklus loop. Jika sewaktu-waktu koneksi internet terputus di tengah jalan, perintah ini akan langsung memicu fungsi `hubungkanMQTT()` agar sistem kembali pulih secara otomatis.

- `if (berhasil)`: Evaluasi hasil eksekusi perintah `client.publish()`. Menentukan apakah teks serial print akan menampilkan pesan "Data terkirim" atau "Gagal mengirim data!".

## 4. Library yang Digunakan

- `ESP8266WiFi.h`: Pustaka fundamental jaringan ESP8266. Digunakan untuk menghubungkan perangkat ke titik akses nirkabel (SSID dan Password), mengecek status koneksi (`WiFi.status()`), dan mengambil alamat IP (`WiFi.localIP()`).

- `PubSubClient.h`: Pustaka protokol MQTT (dibuat oleh Nick O'Leary). Menyediakan fungsi-fungsi esensial MQTT seperti menentukan alamat server (`setServer`), menghubungkan diri ke broker (`connect`), menjaga detak jantung koneksi (`loop`), dan mengirim pesan (`publish`).

- `ArduinoJson.h`: Pustaka manipulasi data (dibuat oleh Benoit Blanchon). Digunakan untuk membangun struktur data berpasangan (key-value) di dalam memori (JsonDocument) dan menyusunnya menjadi format teks string (melalui `serializeJson()`) agar siap dikirim melalui MQTT.



# Dokumentasi

<img width="383" height="511" alt="gambar" src="https://github.com/user-attachments/assets/2e55be71-e8ce-4b34-a643-65622409e301" /> <br>
Rangkaian percobaan 3A & 3B <br><br>

<img width="522" height="511" alt="gambar" src="https://github.com/user-attachments/assets/3f504a3e-a679-4154-aac8-8a66804a4ac1" /> <br>
Serial Monitor Percobaan 3A <br><br>

<img width="456" height="511" alt="gambar" src="https://github.com/user-attachments/assets/a8e553ca-0b1a-4a3c-b385-36eff076d4b9" /> <br>
Serial Monitor Percobaan 3B  <br><br>

<img width="470" height="511" alt="gambar" src="https://github.com/user-attachments/assets/e36aabcd-80ea-45af-829f-3e0dd675ae69" /> <br>
MQTT Explorer <br><br>

