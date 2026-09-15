# Percobaan 2A: Konfigurasi Mode Station (STA)

## 1. Detail Percobaan
Percobaan ini mengubah mikrokontroler menjadi penyedia jaringan mandiri atau pemancar WiFi lokal (hotspot). Mikrokontroler tidak terhubung ke internet atau router luar, melainkan memancarkan sinyal WiFi-nya sendiri dengan nama "ESP32_DediPoenya" dan sandi "12345678". Perangkat lain (seperti laptop atau smartphone) dapat mendeteksi dan terhubung ke mikrokontroler ini.

## 2. Penjelasan Kode dan Fungsi (Function)

```ino
#include <ESP8266WiFi.h>

const char* ssid = "AkuNakMeletup";
const char* password = "AbsoluteNiba";

const int ledPin = 2; // LED indikator status koneksi

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Set mode WiFi menjadi Station
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Jika berhasil terhubung
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("RSSI (dBm) : ");
  Serial.println(WiFi.RSSI());
  digitalWrite(ledPin, HIGH); // nyalakan LED sebagai indikator
}

void loop() {
  // Cek status koneksi setiap 5 detik
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Status: Terhubung");
  } else {
    Serial.println("Status: Terputus");
    digitalWrite(ledPin, LOW);
  }
  delay(5000);
}
```

Kode ini dibagi menjadi dua fungsi utama bawaan Arduino, yaitu setup() dan loop():

- Bagian Deklarasi Awal:
  - const char* ssid dan const char* password: Menyimpan nama dan sandi jaringan WiFi tujuan
  - const int ledPin = 2;: Menetapkan pin 2 (biasanya terhubung ke LED built-in pada board) sebagai indikator visual.

- Fungsi setup() (Berjalan satu kali saat perangkat dinyalakan):
  - `Serial.begin(115200);`: Membuka jalur komunikasi serial ke komputer dengan kecepatan 115200 bps untuk menampilkan teks di Serial Monitor.
  - `pinMode(ledPin, OUTPUT);` & `digitalWrite(ledPin, LOW);`: Mengatur pin LED sebagai output dan memastikan LED mati di awal program.
  - `WiFi.mode(WIFI_STA);`: Mengatur mode radio WiFi menjadi Station (klien).
  - `WiFi.begin(ssid, password);`: Memerintahkan mikrokontroler mulai proses menyambung ke WiFi dengan kredensial yang diberikan.
  - `WiFi.localIP()`, `WiFi.macAddress()`, `WiFi.RSSI()`: Fungsi bawaan dari pustaka WiFi untuk mengambil data alamat IP lokal, alamat MAC perangkat keras, dan kekuatan sinyal (RSSI).

- `Fungsi loop()` (Berjalan berulang tanpa henti):
  - Fungsi ini diprogram untuk terus berjalan mengevaluasi status koneksi menggunakan jeda waktu delay(5000); atau setiap 5 detik.

## 3. Penjelasan Percabangan dan Kondisional

Program ini menggunakan dua bentuk struktur kondisional:
- Perulangan `while (WiFi.status() != WL_CONNECTED)`: Ini adalah kondisional blokir (blocking). Selama status WiFi bukan WL_CONNECTED (belum terhubung), program akan tertahan di dalam blok ini. Ia akan menunggu selama 500 milidetik (delay(500)) dan mencetak karakter titik (.) secara berulang di Serial Monitor hingga koneksi berhasil.
- Percabangan if...else di dalam loop():
  - `if (`WiFi.status() == WL_CONNECTED)`: Mengevaluasi status koneksi saat program sedang berjalan. Jika terhubung, Serial Monitor mencetak "Status: Terhubung".
  - `else`: Jika kondisi di atas salah (koneksi terputus), ia mencetak "Status: Terputus" dan mematikan LED (digitalWrite(ledPin, LOW)).

## 4. Library yang Digunakan

#include <ESP8266WiFi.h>: Ini adalah pustaka (library) khusus untuk mengelola fungsi jaringan WiFi pada mikrokontroler keluarga ESP8266. (Catatan: Jika Anda menggunakan perangkat ESP32, baris ini biasanya harus diganti menjadi #include <WiFi.h> agar tidak terjadi error kompilasi, namun logika programnya tetap persis sama).

## 5. Modifikasi Program (Pertanyaan Praktikum)

```cpp
#include <WiFi.h> // Gunakan <ESP8266WiFi.h> jika memakai ESP8266

const char* ssid = "AkuNakMeletup";
const char* password = "AbsoluteNiba";
const int ledPin = 2; 

unsigned long previousMillis = 0;
const long interval = 10000; // Interval cek koneksi 10 detik

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi berhasil terhubung!");
  digitalWrite(ledPin, HIGH);
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Cek koneksi setiap interval tertentu secara non-blocking
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Koneksi terputus! Mencoba menghubungkan kembali...");
      digitalWrite(ledPin, LOW);
      
      WiFi.disconnect(); // Putuskan sesi yang error
      WiFi.reconnect();  // Coba hubungkan ulang ke SSID terakhir
      
    } else {
      Serial.println("Status: Terhubung");
      digitalWrite(ledPin, HIGH);
    }
  }
}
```
Penjelasan :
- unsigned long previousMillis = 0; dan const long interval = 10000;: Digunakan untuk membuat timer non-blocking dengan millis() agar program tidak terhenti total (freeze) oleh delay().
- if (currentMillis - previousMillis >= interval): Memastikan pengecekan dan eksekusi koneksi hanya dilakukan setiap 10 detik.
- if (WiFi.status() != WL_CONNECTED): Mengevaluasi apakah status koneksi WiFi terputus.
- WiFi.disconnect();: Memutus paksa sisa koneksi socket yang mungkin masih menggantung (hanging) saat jaringan terputus.
- WiFi.reconnect();: Perintah bawaan pustaka WiFi untuk secara otomatis mencoba menyambung ulang ke jaringan menggunakan SSID dan password yang telah diinisialisasi di fungsi setup().


# Percobaan 2B: Konfigurasi Mode Access Point (AP)

## 1. Detail Percobaan

Percobaan ini mengubah mikrokontroler menjadi penyedia jaringan mandiri atau pemancar WiFi lokal (hotspot). Mikrokontroler tidak terhubung ke internet atau router luar, melainkan memancarkan sinyal WiFi-nya sendiri dengan nama "ESP32_DediPoenya" dan sandi "12345678". Perangkat lain (seperti laptop atau smartphone) dapat mendeteksi dan terhubung ke mikrokontroler ini.

## 2. Penjelasan Kode dan Fungsi (Function)

```cpp
#include <ESP8266WiFi.h>

const char* ap_ssid = "ESP32_DediPoenya";
const char* ap_password = "12345678"; // minimal 8 karakter

void setup() {
  Serial.begin(115200);
  // Set mode WiFi menjadi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  
  IPAddress apIP = WiFi.softAPIP(); // Ditambahkan '='
  
  Serial.println("Access Point aktif!");
  Serial.print("SSID : ");
  Serial.println(ap_ssid);
  Serial.print("IP Address: ");
  Serial.println(apIP);
}

void loop() {
  // Menampilkan jumlah perangkat yang terhubung setiap 5 detik
  int jumlahClient = WiFi.softAPgetStationNum(); // Ditambahkan '='
  
  Serial.print("Jumlah perangkat terhubung: ");
  Serial.println(jumlahClient);
  delay(5000);
}
```

- Deklarasi Awal
  - `const char* ap_ssid` dan `ap_password` digunakan untuk menentukan nama jaringan (SSID) dan kata sandi yang akan dipancarkan oleh ESP32 sebagai **Access Point**.
  - Kata sandi harus memiliki minimal **8 karakter** agar dapat digunakan pada jaringan dengan keamanan WPA2.

- Fungsi `setup()` : dijalankan satu kali saat ESP32 mulai bekerja.

  - `Serial.begin(115200);`  Digunakan untuk memulai komunikasi serial dengan baud rate **115200** sehingga informasi dapat ditampilkan pada Serial Monitor.

  - `WiFi.mode(WIFI_AP);`  Mengatur ESP32 agar bekerja dalam mode **Access Point (AP)**, sehingga ESP32 dapat membuat jaringan WiFi sendiri.

  - `WiFi.softAP(ap_ssid, ap_password);`  Digunakan untuk mengaktifkan **hotspot WiFi** pada ESP32 menggunakan SSID dan password yang telah ditentukan.

  - `WiFi.softAPIP();`  Digunakan untuk mendapatkan **IP Address** dari Access Point ESP32. Secara default, IP yang digunakan biasanya adalah `192.168.4.1`. Nilai tersebut disimpan ke dalam variabel `apIP` dan ditampilkan pada Serial Monitor.

- Fungsi `loop()` dijalankan secara berulang selama ESP32 aktif.
  - `WiFi.softAPgetStationNum();`  Digunakan untuk mengetahui jumlah perangkat (*client/station*) yang sedang terhubung ke Access Point ESP32.

  - Nilai jumlah perangkat disimpan dalam variabel integer `jumlahClient`.

  - Informasi jumlah client kemudian ditampilkan pada **Serial Monitor setiap 5 detik** menggunakan `delay(5000)`.

## 3. Penjelasan Percabangan dan Kondisional

Berbeda dengan percobaan 2A, pada kode percobaan 2B tidak terdapat struktur percabangan atau kondisional eksplisit (seperti if, else, atau while). Program berjalan secara sekuensial (lurus) dari atas ke bawah pada fungsi setup(), lalu mengeksekusi fungsi loop() secara berulang tanpa adanya evaluasi kondisi logika benar/salah (true/false).

## 4. Library yang Digunakan

- #include <ESP8266WiFi.h>: Sama seperti pada percobaan 2A, pustaka ini memuat sekumpulan perintah khusus (seperti WiFi.softAP() dan WiFi.softAPgetStationNum()) untuk mengontrol perangkat keras WiFi pada chip ESP8266.

## 5. Modifikasi Program (Pertanyaan Praktikum)

```cpp
#include <WiFi.h> // Gunakan <ESP8266WiFi.h> jika memakai ESP8266

// Kredensial untuk mode Station (terhubung ke router rumah)
const char* sta_ssid = "WiFi_Rumah";
const char* sta_password = "PasswordRumah123";

// Kredensial untuk mode Access Point (dipancarkan oleh ESP32)
const char* ap_ssid = "ESP32_DediPoenya";
const char* ap_password = "12345678";

void setup() {
  Serial.begin(115200);
  
  // Set mode WiFi menjadi gabungan AP dan Station
  WiFi.mode(WIFI_AP_STA);
  
  // 1. Konfigurasi Access Point
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Access Point aktif!");
  Serial.print("IP Address AP: ");
  Serial.println(WiFi.softAPIP());
  
  // 2. Konfigurasi Station
  WiFi.begin(sta_ssid, sta_password);
  Serial.print("Menghubungkan ke WiFi Rumah");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nBerhasil terhubung ke WiFi Rumah!");
  Serial.print("IP Address Station: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Pantau jumlah klien yang terhubung ke AP kita
  int jumlahClient = WiFi.softAPgetStationNum();
  Serial.print("Jumlah klien di AP: ");
  Serial.println(jumlahClient);
  delay(5000);
}
```

Penjelasan:
- const char* sta_ssid dan sta_password: Deklarasi variabel baru untuk menyimpan data login ke router rumah.
- WiFi.mode(WIFI_AP_STA);: Perintah utama yang memberitahu chip ESP32 untuk menyalakan mode Station dan Access Point secara bersamaan.
- WiFi.softAP(...): Mengaktifkan layanan pemancar (AP) terlebih dahulu sehingga meskipun koneksi Station nanti gagal/lama, pemancar lokal sudah bisa diakses.
- WiFi.begin(...) sampai WiFi.localIP(): Rutinitas koneksi untuk mode Station seperti pada Percobaan 2A, diletakkan setelah inisialisasi AP.


# Dokumentasi

<img width="559" height="419" alt="gambar" src="https://github.com/user-attachments/assets/b1a3d101-3849-4c51-b5f5-e7e9ff527f45" /> <br>
Rangkaian percobaan 2A <br><br>
<img width="559" height="419" alt="gambar" src="https://github.com/user-attachments/assets/9c402452-da8d-4176-8a76-10bcba570efe" /> <br>
Rangkaian percobaan 2B <br><br>
<img width="684" height="505" alt="1 percobaan 1A" src="https://github.com/user-attachments/assets/f1f4fc40-7899-4232-b920-67acb666792a" /> <br>
Serial Monitor Percobaan 2A <br><br>
<img width="680" height="763" alt="2 percobaan 2" src="https://github.com/user-attachments/assets/c5f73008-f397-401a-8c51-465ba2e6af73" /> <br>
Serial Monitor Percobaan 2B  <br><br>

