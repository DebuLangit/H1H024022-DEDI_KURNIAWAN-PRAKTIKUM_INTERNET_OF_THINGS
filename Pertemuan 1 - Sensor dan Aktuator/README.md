# Percobaan 1A Akuisisi Data Sensor DHT22 (Suhu dan Kelembaban)
## Detail Percobaan
Percobaan 1A mengimplementasikan akuisisi data suhu dan kelembaban menggunakan sensor DHT11 saat praktikum pada ESP32. Sensor dihubungkan ke GPIO 4 dengan tegangan 3,3V, kemudian data dibaca menggunakan library DHT.h dan ditampilkan pada Serial Monitor setiap 2 detik. Fungsi isnan() digunakan untuk memastikan data yang diterima valid.
## Penjelasan Code
Kode digunakan untuk membaca suhu dan kelembaban dari sensor DHT11 yang terhubung pada GPIO 4. Program menginisialisasi sensor dan komunikasi serial, kemudian membaca data secara berulang dan menampilkannya pada Serial Monitor setiap 3 detik.
## Penjelasan Function
- `setup()`: Melakukan inisialisasi awal satu kali saat mikrokontroler dijalankan.
- `loop()`: Menjalankan proses pembacaan sensor secara berulang.
- `Serial.begin(115200)`: Mengatur komunikasi serial dengan baud rate 115200.
- `delay()`: Memberikan jeda waktu pada program.
- `Serial.print()` / `println()`: Menampilkan data pada Serial Monitor.
- `dht.begin()`: Menginisialisasi sensor DHT11.
- `readHumidity()`: Membaca nilai kelembaban relatif.
- `readTemperature()`: Membaca nilai suhu dalam °C.
- `isnan()`: Memeriksa apakah data sensor valid atau bernilai NaN.
## Penjelasan Percabangan/Conditionial
Percabangan `if-else` digunakan untuk memvalidasi hasil pembacaan. Jika suhu atau kelembaban tidak valid, program menampilkan pesan kegagalan. Jika data valid, hasil pembacaan suhu dan kelembaban ditampilkan ke Serial Monitor.
## Library/Dependencies
- Library <DHT.h> digunakan untuk mempermudah komunikasi dengan sensor DHT11 sehingga proses pembacaan suhu dan kelembaban dapat dilakukan menggunakan fungsi seperti readTemperature() dan readHumidity().
- Board package ESP8266, agar program dapat dikompilasi dan diupload ke ESP8266.
## Pertanyaan Praktikum
1. Diagram Alur (Flowchart) Proses Akuisisi Data Sensor DHT22
2. Fungsi Perintah `isnan()`
   - Fungsi isnan() digunakan untuk memeriksa apakah data suhu atau kelembaban dari sensor DHT bernilai NaN (Not-a-Number) akibat gangguan komunikasi, daya, kabel, atau kerusakan sensor. Dengan demikian, program dapat membedakan data yang valid dan data yang mengalami error.
3. Alasan Diperlukannya Delay Minimal 2 Detik
   - Delay minimal 2 detik diperlukan karena sensor DHT memiliki proses pembacaan dan konversi data yang membutuhkan waktu. Pembacaan terlalu cepat dapat menyebabkan data belum diperbarui, menghasilkan data yang salah atau berulang, maupun menyebabkan kegagalan pembacaan `(NaN)`.
4. Modifikasi Program (Rata-Rata 5 Kali Pembacaan)
## Dokumentasi


# Percobaan 2A: Kendali Aktuator Relay Berdasarkan Data Sensor
## Detail Percobaan
Percobaan ini mengimplementasikan hubungan antara sensor dan aktuator pada ESP32. Sensor DHT22 membaca suhu lingkungan, kemudian nilainya dibandingkan dengan threshold 30°C. Jika suhu melebihi batas, ESP32 memberikan sinyal HIGH untuk mengaktifkan relay/LED, sedangkan pada suhu ≤ 30°C aktuator dimatikan. Kondisi suhu dan aktuator dapat dipantau melalui Serial Monitor.
## Penjelasan Code
## Penjelasan Function
## Penjelasan Percabangan/Conditionial
## Library/Dependencies
## Pertanyaan Praktikum
## Dokumentasi


# Pertanyaan Analisis
