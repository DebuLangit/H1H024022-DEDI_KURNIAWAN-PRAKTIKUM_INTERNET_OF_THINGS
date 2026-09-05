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
```
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Memulai akuisisi data sensor DHT22 dengan Rata-Rata...");
  dht.begin();
  delay(2000);
}

void loop() {
  float totalSuhu = 0;
  float totalKelembaban = 0;
  int pembacaanValid = 0;

  // Melakukan pengambilan data sebanyak 5 kali
  for (int i = 0; i < 5; i++) {
    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();

    if (isnan(kelembaban) || isnan(suhu)) {
      Serial.println("Gagal membaca sampel data dari sensor!");
    } else {
      totalSuhu += suhu;
      totalKelembaban += kelembaban;
      pembacaanValid++;
    }

    // Jeda 2 detik antar pengambilan sampel
    delay(2000);
  }

  // Menampilkan hasil rata-rata jika terdapat pembacaan yang valid
  if (pembacaanValid > 0) {
    float rataSuhu = totalSuhu / pembacaanValid;
    float rataKelembaban = totalKelembaban / pembacaanValid;

    Serial.print("=== Rata-rata dari ");
    Serial.print(pembacaanValid);
    Serial.println(" sampel valid ===");
    Serial.print("Rata-rata Suhu       : ");
    Serial.print(rataSuhu);
    Serial.println(" °C");
    Serial.print("Rata-rata Kelembaban : ");
    Serial.print(rataKelembaban);
    Serial.println(" %");
    Serial.println("------------------------------------------");
  } else {
    Serial.println("Semua percobaan pengambilan sampel gagal!");
  }
} 
```
Penjelasan fungsi setiap baris kode baru yang ditambahkan ke dalam fungsi `loop()` untuk menghitung rata-rata dari 5 kali pembacaan sensor:
- `float totalSuhu = 0;` Mendeklarasikan variabel bertipe `float` untuk menampung penjumlahan seluruh data suhu dari sampel yang berhasil dibaca. Diinisialisasi dengan nilai `0`.
- `float totalKelembaban = 0;` Mendeklarasikan variabel bertipe `float` untuk menampung akumulasi data kelembaban. Diinisialisasi dengan nilai `0`.
- `int pembacaanValid = 0;` Mendeklarasikan variabel penanda bernilai integer untuk menghitung berapa kali sensor berhasil memberikan data valid (bukan NaN).
- `for (int i = 0; i < 5; i++) { ... }` Struktur perulangan (*looping*) untuk mengulang proses pembacaan data sensor sebanyak 5 kali pengambilan sampel.
- `totalSuhu += suhu;` Menambahkan nilai suhu yang baru dibaca ke dalam akumulator `totalSuhu` (singkatan dari `totalSuhu = totalSuhu + suhu`).
- `totalKelembaban += kelembaban;` Menambahkan nilai kelembaban yang baru dibaca ke dalam akumulator `totalKelembaban`.
- `pembacaanValid++;` Menambahkan angka `1` pada penghitung sampel yang valid setiap kali pembacaan tidak terdeteksi `isnan()`.
- `delay(2000);` (di dalam perulangan) Memberikan jeda waktu 2 detik antar pengambilan sampel sesuai spesifikasi teknis sensor DHT22.
- `if (pembacaanValid > 0) { ... }` Kondisi untuk memastikan bahwa pembagi tidak bernilai nol (mencegah *division by zero error*) jika seluruh 5 kali percobaan gagal dibaca.
- `float rataSuhu = totalSuhu / pembacaanValid;` Menghitung rata-rata suhu dengan membagi total nilai suhu yang terkumpul dengan jumlah pembacaan yang valid.
- `float rataKelembaban = totalKelembaban / pembacaanValid;` Menghitung rata-rata kelembaban dengan membagi total kelembaban yang terkumpul dengan jumlah pembacaan yang valid.

## Dokumentasi
<img width="559" height="316" alt="gambar" src="https://github.com/user-attachments/assets/1969153c-3c2f-410b-8faa-0652ae595702" /> </br>
Foto rangkaian </br>
<img width="341" height="368" alt="gambar" src="https://github.com/user-attachments/assets/7b84d40a-8eb1-487c-ad07-d174f22db6f8" /> </br>
suhu dan kelembaban dalam keadaan normal </br>
<img width="328" height="362" alt="gambar" src="https://github.com/user-attachments/assets/d1fe95a6-ac44-40ed-9170-fab2b4d7429d" /> </br>
suhu dan kelembaban dalam genggaman tangan </br>
<img width="337" height="365" alt="gambar" src="https://github.com/user-attachments/assets/25114947-81d5-4620-81ee-fff5f59de351" /> </br>
suhu dan kelembaban di dekat AC </br>
<img width="327" height="366" alt="gambar" src="https://github.com/user-attachments/assets/e75d721d-a547-4a5b-bb06-b3ea451c0c08" /> </br>
suhu dan kelembaban di dekat api </br>



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
