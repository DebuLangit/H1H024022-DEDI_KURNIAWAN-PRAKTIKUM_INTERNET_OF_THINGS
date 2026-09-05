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
   <img width="451" height="512" alt="per praktikum iot mod 1 p1" src="https://github.com/user-attachments/assets/256bf9af-257d-44c4-a724-c9279e80371e" /> </br>
   diagram alur
3. Fungsi Perintah `isnan()`
   - Fungsi isnan() digunakan untuk memeriksa apakah data suhu atau kelembaban dari sensor DHT bernilai NaN (Not-a-Number) akibat gangguan komunikasi, daya, kabel, atau kerusakan sensor. Dengan demikian, program dapat membedakan data yang valid dan data yang mengalami error.
4. Alasan Diperlukannya Delay Minimal 2 Detik
   - Delay minimal 2 detik diperlukan karena sensor DHT memiliki proses pembacaan dan konversi data yang membutuhkan waktu. Pembacaan terlalu cepat dapat menyebabkan data belum diperbarui, menghasilkan data yang salah atau berulang, maupun menyebabkan kegagalan pembacaan `(NaN)`.
5. Modifikasi Program (Rata-Rata 5 Kali Pembacaan)
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
Program merupakan sistem kendali otomatis yang menggunakan DHT11 pada GPIO 4 untuk membaca suhu dan relay pada GPIO 5 sebagai aktuator. Suhu dibandingkan dengan threshold 30°C. Jika suhu > 30°C, relay menyala (ON), sedangkan jika suhu ≤ 30°C, relay mati (OFF). Proses dilakukan berulang setiap 2 detik dan hasilnya ditampilkan pada Serial Monitor.

## Penjelasan Function
- `setup()`: Melakukan konfigurasi awal sensor, komunikasi serial, dan pin relay.
- `loop()`: Menjalankan pembacaan sensor dan pengendalian relay secara berulang.
- `Serial.begin(115200)`: Mengaktifkan komunikasi Serial Monitor dengan kecepatan 115200 bps.
- `dht.begin()`: Menginisialisasi sensor DHT11.
- `pinMode(RELAYPIN, OUTPUT)`: Mengatur GPIO 5 sebagai output untuk relay.
- `digitalWrite()`: Mengatur kondisi relay menggunakan sinyal HIGH atau LOW.
- `dht.readTemperature()`: Membaca suhu dalam satuan Celsius.
- `isnan()`: Memeriksa apakah data sensor valid.
- `delay(2000)`: Memberikan jeda selama 2 detik sebelum pembacaan berikutnya.

## Penjelasan Percabangan/Conditionial
Program menggunakan dua percabangan utama:
- `if (isnan(suhu))`: Memeriksa apakah pembacaan suhu berhasil. Jika tidak valid, program menampilkan pesan kesalahan.
- `if (suhu > suhuThreshold)`: Menentukan kondisi relay. Suhu > 30°C menyebabkan relay ON, sedangkan suhu ≤ 30°C menyebabkan relay OFF.

## Library/Dependencies
- Library <DHT.h> digunakan untuk mempermudah komunikasi dengan sensor DHT11 sehingga proses pembacaan suhu dan kelembaban dapat dilakukan menggunakan fungsi seperti readTemperature() dan readHumidity().
- Board package ESP8266, agar program dapat dikompilasi dan diupload ke ESP8266.

## Pertanyaan Praktikum
1. Mengapa diperlukan nilai ambang batas (threshold)? </br> Nilai threshold digunakan sebagai batas pengambilan keputusan bagi mikrokontroler untuk menentukan kapan aktuator harus ON atau OFF berdasarkan data sensor. Dengan adanya threshold, perubahan data sensor dapat diterjemahkan menjadi tindakan otomatis.

2. Apa yang terjadi jika suhuThreshold diturunkan menjadi 20°C? </br> Jika threshold diturunkan menjadi 20°C, sedangkan suhu ruangan berada di atas 25°C, kondisi suhu > suhuThreshold akan selalu terpenuhi. Akibatnya, aktuator akan terus menyala (ON) dan hanya mati jika suhu turun hingga 20°C atau lebih rendah.

3. Perbedaan kendali kondisi tunggal dan histerisis
   - Kondisi tunggal: Menggunakan satu threshold. Jika suhu berada di sekitar batas tersebut, aktuator dapat sering ON-OFF (chattering).
   - Histerisis: Menggunakan dua threshold, yaitu batas atas untuk menyalakan dan batas bawah untuk mematikan aktuator. Jarak antara keduanya menjadi area aman (deadband) yang mencegah aktuator terlalu sering berganti kondisi.

## Dokumentasi
<img width="383" height="511" alt="gambar" src="https://github.com/user-attachments/assets/6a7dc8a7-d546-4a0c-975b-b65972a7d9b6" /> </br>
foto rangkaian

# Pertanyaan Analisis

1. Uraian Hasil Tugas pada Setiap Percobaan
   - Percobaan 1A — Akuisisi Data Sensor </br> Mikrokontroler berhasil membaca data **suhu dan kelembaban** dari sensor DHT11/DHT22. Data dikirim secara digital dan ditampilkan pada **Serial Monitor** secara berkala. Program juga menggunakan fungsi `isnan()` untuk mendeteksi kegagalan pembacaan sensor.
   - Percobaan 2A — Kendali Aktuator </br> Sistem berhasil menghubungkan data sensor dengan aktuator berupa **relay atau LED**. Nilai suhu digunakan sebagai dasar pengambilan keputusan berdasarkan **threshold**. Aktuator akan **ON** ketika suhu melebihi batas yang ditentukan dan **OFF** ketika suhu berada di bawah atau sama dengan batas tersebut.

2. Pengaruh Akurasi dan Waktu Tanggap Sensor
   - Akurasi Sensor </br> Akurasi sensor berpengaruh terhadap ketepatan keputusan mikrokontroler. Kesalahan pembacaan suhu dapat menyebabkan aktuator menyala atau mati pada waktu yang tidak sesuai sehingga dapat memengaruhi efisiensi dan kinerja sistem.
   - Waktu Tanggap (*Response Time*) </br> Waktu tanggap menentukan seberapa cepat sistem merespons perubahan kondisi lingkungan. Sensor dengan respons lambat menyebabkan mikrokontroler terlambat menerima perubahan data sehingga aktuator juga terlambat memberikan respons.

3. Cara Kerja Sistem
Sistem bekerja melalui tiga tahapan utama:
   - **Akuisisi (Input)** </br> Sensor DHT membaca suhu dan kelembaban lingkungan kemudian mengirimkan data digital ke mikrokontroler.
   - **Pemrosesan (Process)** </br> Mikrokontroler mengolah data sensor menjadi nilai numerik menggunakan library `DHT.h`. Nilai tersebut kemudian dibandingkan dengan **threshold** menggunakan logika `if-else`.
   - **Aktuasi (Output)** </br> Hasil pemrosesan digunakan untuk memberikan sinyal `HIGH` atau `LOW` kepada aktuator seperti relay. Aktuator kemudian melakukan tindakan sesuai kondisi yang telah ditentukan.

4. Kombinasi Akuisisi dan Kendali pada Sistem IoT </br> Penggabungan proses akuisisi data sensor dan kendali aktuator memungkinkan terbentuknya sistem **otomatis dan closed-loop**. Sistem dapat mengambil keputusan berdasarkan kondisi lingkungan tanpa memerlukan intervensi manusia secara terus-menerus.
   - Penerapan pada Smart Farming </br> Sensor suhu dan kelembaban dapat digunakan untuk memantau kondisi tanaman. Jika kondisi lingkungan atau kelembaban tanah melewati batas tertentu, mikrokontroler dapat mengaktifkan relay dan **pompa air** secara otomatis. Pompa akan dimatikan ketika kondisi kembali normal.
   - Penerapan pada Smart Home </br> Sensor suhu dapat digunakan untuk memantau suhu ruangan dan mengendalikan **kipas atau AC**. Jika suhu melebihi batas yang ditentukan, sistem dapat mengaktifkan perangkat pendingin secara otomatis dan mematikannya ketika suhu sudah kembali sesuai kondisi yang diinginkan.
