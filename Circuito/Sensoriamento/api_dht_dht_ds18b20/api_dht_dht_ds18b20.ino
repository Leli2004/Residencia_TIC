#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

/* #define SDA1 21 // Pino SDA para o primeiro barramento I2C
#define SCL1 22 // Pino SCL para o primeiro barramento I2C */

#define DHTTYPE DHT22 
#define DHTPIN 25 // Pino DHT interno (SCL)
#define DHTPIN2 18 // Pino DHT externo (SCL)
#define ONE_WIRE_BUS 32 // pino Output DS18B20

//TwoWire I2Cum = TwoWire(0); // Cria o primeiro barramento I2C
DHT dht (DHTPIN, DHTTYPE); // Cria o objeto dht
DHT dht2 (DHTPIN2, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS); // Prepara uma instância oneWire para comunicar com qualquer outro dispositivo oneWire
DallasTemperature sensors(&oneWire);  // Passa uma referência oneWire para a biblioteca DallasTemperature

#define WIFISSID "ssid_here"
#define PASSWORD "pwd_here"

#define MICROCONTROLLER_CODE "esp32amo" // Código fixo para o microcontrolador

float tempInt;
float umidInt;
float tempExt;
float umidExt;
float tempAlimento;

unsigned long previousMillis = 0;
const long interval = 30000; // 30 

void setup() {
  Serial.begin(115200);
  //I2Cum.begin(SDA1, SCL1, 400000); 
  dht.begin(); // Inicia o dht
  dht2.begin(); // Inicia o dht
  sensors.begin();  // Inicia a biblioteca DS18B20

  WiFi.begin(WIFISSID, PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Estabelecendo conexão com WiFi...");
  }
  Serial.println("Conectado à rede!");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
		
	/*tempInt = readTemperature(&I2Cum); // Somente para mostrar no monitor serial
	umidInt = readHumidity(&I2Cum); // Somente para mostrar no monitor serial*/
  umidExt = dht2.readHumidity(); // Somente para mostrar no monitor serial
	tempExt = dht2.readTemperature(); // Somente para mostrar no monitor serial
	umidInt = dht.readHumidity(); // Somente para mostrar no monitor serial
	tempInt = dht.readTemperature(); // Somente para mostrar no monitor serial
  sensors.requestTemperatures(); // para o DS18B20
  tempAlimento = sensors.getTempCByIndex(0); 
	
    Serial.print("Temperatura Interna: ");
    Serial.print(tempInt);
    Serial.println(" *C");
    Serial.print("Umidade Interna: ");
    Serial.print(umidInt);
    Serial.println(" %");
    Serial.print("Temperatura Externa: ");
    Serial.print(tempExt);
    Serial.println(" *C");
    Serial.print("Umidade Externa: ");
    Serial.print(umidExt);
    Serial.println(" %");
    Serial.print("Temperatura Alimento: ");
    Serial.print(tempAlimento);
    Serial.println(" *C");
    sendToAPI(tempInt, umidInt, tempExt, umidExt, tempAlimento);
	Serial.print("*** "); Serial.println(" ");
  }
}

void sendToAPI(float tempInt, float umidInt, float tempExt, float umidExt, float tempAlimento) {
  HTTPClient http;

  String url = "url_api_here";
  String data = "{";
  data += "\"microcontroller_code\":\"" + String(MICROCONTROLLER_CODE) + "\", ";
  data += "\"temperature_int\": " + String(tempInt, 2) + ", ";
  data += "\"humidity_int\": " + String(umidInt, 2) + ", ";
  data += "\"temperature_ext\": " + String(tempExt, 2) + ", ";
  data += "\"humidity_ext\": " + String(umidExt, 2) + ", ";
  data += "\"temperature_alimento\": " + String(tempAlimento, 2);
  data += "}";

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(data);

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.print("HTTP response code: ");
    Serial.println(httpCode);
    Serial.print("Response data: ");
    Serial.println(payload);
  } else {
    Serial.println("HTTP request failed");
  }

  http.end();
}

/*
float readTemperature(TwoWire *wire) {
  wire->beginTransmission(0x40);
  wire->write(0xE3);
  wire->endTransmission();
  delay(50);
  wire->requestFrom(0x40, 2);
  if (wire->available() < 2) return NAN;
  uint16_t rawTemp = wire->read() << 8 | wire->read();
  return -46.85 + 175.72 * (rawTemp / 65536.0);
}

float readHumidity(TwoWire *wire) {
  wire->beginTransmission(0x40);
  wire->write(0xE5);
  wire->endTransmission();
  delay(50);
  wire->requestFrom(0x40, 2);
  if (wire->available() < 2) return NAN;
  uint16_t rawHum = wire->read() << 8 | wire->read();
  return -6.0 + 125.0 * (rawHum / 65536.0);
}
*/