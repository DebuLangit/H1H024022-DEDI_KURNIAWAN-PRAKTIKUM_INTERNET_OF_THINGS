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
