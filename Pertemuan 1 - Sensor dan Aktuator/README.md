# Percobaan 1A Akuisisi Data Sensor DHT22 (Suhu dan Kelembaban)
## Detail Percobaan
Percobaan 1A mengimplementasikan akuisisi data suhu dan kelembaban menggunakan sensor DHT22 pada ESP32. Sensor dihubungkan ke GPIO 4 dengan tegangan 3,3V, kemudian data dibaca menggunakan library DHT.h dan ditampilkan pada Serial Monitor setiap 2 detik. Fungsi isnan() digunakan untuk memastikan data yang diterima valid.
## Penjelasan Code
Kode digunakan untuk membaca suhu dan kelembaban dari sensor DHT11 yang terhubung pada GPIO 4. Program menginisialisasi sensor dan komunikasi serial, kemudian membaca data secara berulang dan menampilkannya pada Serial Monitor setiap 3 detik.
## Penjelasan Function
## Penjelasan Percabangan/Conditionial
## Library/Dependencies
## Pertanyaan Praktikum
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
