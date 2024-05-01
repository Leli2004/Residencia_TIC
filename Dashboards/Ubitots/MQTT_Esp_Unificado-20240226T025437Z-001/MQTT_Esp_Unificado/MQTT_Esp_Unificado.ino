#include <Arduino.h>
#include "MQTT_Client.h" //Arquivo com as funções de mqtt
#include <WiFi.h>
#include <PubSubClient.h> //Biblioteca para as publicações via mqtt
#include <Wire.h>

#define HTU21D 0x40 
#define WIFISSID "SSID_here" //Coloque seu SSID de WiFi aqui
#define PASSWORD "pwd_here" //Coloque seu password de WiFi aqui
#define TOKEN "toj=ken_here" //Coloque seu TOKEN do Ubidots aqui
#define VARIABLE_LABEL_TEMPERATURE_INT "VARIABLE_LABEL_TEMPERATURE_INT " //Label referente a variável de temperatura criada no ubidots
#define VARIABLE_LABEL_HUMIDITY_INT "VARIABLE_LABEL_HUMIDITY_INT" //Label referente a variável de umidade criada no ubidots
#define DEVICE_ID "device_id_here" //ID do dispositivo (Device id, também chamado de client name)
#define SERVER "things.ubidots.com" //Servidor do Ubidots (broker)

//Porta padrão
#define PORT 1883

//Tópico aonde serão feitos os publish, "esp32-dht" é o DEVICE_LABEL
#define TOPIC "/v1.6/devices/control"

//Objeto WiFiClient usado para a conexão wifi
WiFiClient ubidots;
//Objeto PubSubClient usado para publish–subscribe
PubSubClient client(ubidots);

float temperature; //Temperatura que será obtida pelo sensor DHT
float humidity; //Umidade que será obtida pelo sensor DHT

void reconnect() 
{  
  //Loop até que o MQTT esteja conectado
  while (!client.connected()) 
  {
    Serial.println("Attempting MQTT connection...");
    
    //Tenta conectar
    if (client.connect(DEVICE_ID, TOKEN,"")) 
      Serial.println("connected");
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      //Aguarda 2 segundos antes de retomar
      delay(2000);
    }
  }
}

bool mqttInit()
{
  //Inicia WiFi com o SSID e a senha
  WiFi.begin(WIFISSID, PASSWORD);
 
  //Loop até que o WiFi esteja conectado
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  //Exibe no monitor serial
  Serial.println("Connected to network");

  //Seta servidor com o broker e a porta
  client.setServer(SERVER, PORT);
  
  //Conecta no ubidots com o Device id e o token, o password é informado como vazio
  while(!client.connect(DEVICE_ID, TOKEN, ""))
  {
      Serial.println("MQTT - Connect error");
      return false;
  }

  Serial.println("MQTT - Connect ok");
  return true;
}

float lerTemperatura(byte endereco) {

  Wire.beginTransmission(endereco); // Inicia a transmissão I2C com o sensor
  Wire.write(0xE3); // Envia o comando de leitura de temperatura
  Wire.endTransmission(); // Encerra a tramissão I2C

  delay(50); // Aguarda a conversão
  
  Wire.requestFrom(endereco, 3); // Lê 3 bytes 

  if (Wire.available() >= 3) { // Verifica se há 3 ou mais bytes disponíveis

    // Lê os bytes disponíveis no buffer do barramento I2C 
    byte msb = Wire.read(); // Primeiro byte (most significant byte), parte "grande" do valor lido
    byte lsb = Wire.read(); // Segundo byte (least significant byte), parte "pequena" do valor lido
    byte crc = Wire.read(); // Terceiro byte (ciclic redundancy check)

    unsigned int valorBruto = ((unsigned int)msb << 8) | lsb; // Cria um valor composto por 16 bits com a temperatura lida no sensor
    valorBruto &= 0xFFFC; // Limpa os dois últimos bits (bits de status, inúteis para o cálculo da temperatura)

    float t = -46.85 + 175.72 * (valorBruto / 65536.0);
    
    return t;
    
  } else {

    return -999;

  }
   
}

float lerUmidade(byte endereco) {
  
  Wire.beginTransmission(endereco); // Inicia a transmissão I2C com o sensor
  Wire.write(0xE5); // Envia o comando de leitura de temperatura
  Wire.endTransmission(); // Encerra a tramissão I2C

  delay(50); // Aguarda a conversão

  Wire.requestFrom(endereco, 3); // Lê 3 bytes
  
  if (Wire.available() >= 3) { // Verifica se há 3 ou mais bytes disponíveis

    // Lê os bytes disponíveis no buffer do barramento I2C 
    byte msb = Wire.read(); // Primeiro byte (most significant byte), parte "grande" do valor lido
    byte lsb = Wire.read(); // Segundo byte (least significant byte), parte "pequena" do valor lido
    byte crc = Wire.read(); // Terceiro byte (ciclic redundancy check)

    unsigned int valorBruto = ((unsigned int)msb << 8) | lsb; // Cria um valor composto por 16 bits com a umidade lida no sensor
    valorBruto &= 0xFFFC; // Limpa os dois últimos bits (bits de status, inúteis para o cálculo da umidade)

    float u = -6.0 + 125.0 * (valorBruto / 65536.0);
    
    return u;

  } else {

    return -999;

  }
}

bool sendValues(float temperature, float humidity)
{
  
  temperature = lerTemperatura(HTU21D);
  humidity = lerUmidade(HTU21D);

  char json[250];
 
  //Atribui para a cadeia de caracteres "json" os valores referentes a temperatura e os envia para a variável do ubidots correspondente
  sprintf(json,  "{\"%s\":{\"value\":%02.02f, \"context\":{\"temperature\":%02.02f, \"humidity\":%02.02f}}}", VARIABLE_LABEL_TEMPERATURE_INT, temperature, temperature, humidity);  

  if(!client.publish(TOPIC, json))
    return false;

  //Atribui para a cadeia de caracteres "json" os valores referentes a umidade e os envia para a variável do ubidots correspondente
  sprintf(json,  "{\"%s\":{\"value\":%02.02f, \"context\":{\"temperature\":%02.02f, \"humidity\":%02.02f}}}", VARIABLE_LABEL_HUMIDITY_INT, humidity, temperature, humidity);  
      
  if(!client.publish(TOPIC, json))
    return false;

  //Se tudo der certo retorna true
  return true;
}

void setup() 
{
  //Para debug, iniciamos a comunicação serial com 115200 bps
  Serial.begin(115200);
  Wire.begin();
  WiFi.begin(WIFISSID, PASSWORD);

  //Inicializa mqtt (conecta o esp com o wifi, configura e conecta com o servidor da ubidots)
  if(!mqttInit())
  {        
    delay(3000);
    Serial.println("Failed!");
    ESP.restart();
  }
  Serial.println("OK");
}


void loop() 
{
  temperature = lerTemperatura(HTU21D); // Somente para mostrar no monitor serial
  humidity = lerUmidade(HTU21D); // Somente para mostrar no monitor serial

  //Se o esp foi desconectado do ubidots, tentamos reconectar
  if(!client.connected())
    reconnect();

  delay(2500);  
  if(sendValues(temperature, humidity))
  {      
    Serial.println("Successfully sent data");
    Serial.println(temperature); // Somente para mostrar no monitor serial
    Serial.println(humidity); // Somente para mostrar no monitor serial
  }
  else
  {      
    Serial.println("Failed to send sensor data");

  }    
    
  //Esperamos 2.5s para dar tempo de ler as mensagens acima
  delay(2500);    
}








