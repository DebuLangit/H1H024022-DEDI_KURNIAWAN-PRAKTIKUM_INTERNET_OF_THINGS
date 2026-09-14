#include <DHT.h>

#define DHTPIN 4       // GPIO 4 = D2, DATA DHT11
#define DHTTYPE DHT11  // Menggunakan DHT11

#define RELAYPIN 5     // GPIO 5 = D1, IN relay

DHT dht(DHTPIN, DHTTYPE);

const float suhuThreshold = 30.0;  // Batas suhu 30°C

void setup() {
  Serial.begin(115200);

  dht.begin();

  pinMode(RELAYPIN, OUTPUT);

  // Relay mati saat awal
  digitalWrite(RELAYPIN, LOW);

  Serial.println("Sistem DHT11 + Relay");
  Serial.println("Menunggu pembacaan sensor...");
}

void loop() {

  // Membaca suhu
  float suhu = dht.readTemperature();

  // Periksa apakah pembacaan berhasil
  if (isnan(suhu)) {

    Serial.println("Gagal membaca data sensor!");

  } else {

    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C -> ");

    // Jika suhu lebih dari 30°C
    if (suhu > suhuThreshold) {

      digitalWrite(RELAYPIN, HIGH);
      Serial.println("Aktuator: ON");

    } else {

      digitalWrite(RELAYPIN, LOW);
      Serial.println("Aktuator: OFF");
    }
  }

  delay(2000);
}
