#include <UbiConstants.h>
#include <UbiTypes.h>
#include <UbidotsEsp32Mqtt.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "DHT.h"

#define DHTPIN 27
#define DHTTYPE DHT11
#define ABS(x) ((x) < 0 ? -(x) : (x))

TFT_eSPI tft = TFT_eSPI();
DHT dht (DHTPIN, DHTTYPE);

const char *UBIDOTS_TOKEN = "BBUS-fi3XAndawByZY8ygOmSobJmXpFZhPs";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "UPBWiFi";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "esp32";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "Temp"; // Put here your Variable label to which data  will be published
const char *TEMPERATURA_VARIABLE_LABEL = "Temp"; // Temperatura
const char *HUMEDAD_VARIABLE_LABEL = "Hume"; // Humedad

const int PUBLISH_FREQUENCY = 1000; 

unsigned long timer;
uint8_t analogPin = 27; 

Ubidots ubidots(UBIDOTS_TOKEN);



void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void setup()
{
  
  Serial.begin(115200);
  
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
 float Hume = dht.readHumidity();
  float Temp=dht.readTemperature();
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1);
  tft.setTextColor(TFT_VIOLET , TFT_BLACK);
  tft.drawString("Realizado por:", 10, 5, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Nicolas ", 10, 23, 4);
  tft.setTextColor(TFT_RED , TFT_BLACK);
  tft.drawString("Temperatura", 140, 70, 2);
  tft.setTextColor(TFT_BLUE , TFT_BLACK);
  tft.drawString("Humedad", 10, 70, 2);
  tft.fillRect(110, 65, 3, 80, TFT_VIOLET);
  
  if(isnan(Hume) || isnan(Temp)){
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

void loop()
{
  
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  float Hume= dht.readHumidity();
  float Temp=dht.readTemperature();
  if (ABS(millis() - timer) > PUBLISH_FREQUENCY) 
  {
    Serial.print("Temperatura: ");
    Serial.print(Temp);
    Serial.print(" | Humedad: ");
    Serial.println(Hume);
    tft.setTextColor(TFT_RED , TFT_BLACK);
    tft.drawString(String(Temp), 150, 100, 2);
    tft.setTextColor(TFT_BLUE , TFT_BLACK);
    tft.drawString(String(Hume), 20, 100, 2);
    ubidots.add(TEMPERATURA_VARIABLE_LABEL, Temp);  
    ubidots.add(HUMEDAD_VARIABLE_LABEL, Hume);   
        ubidots.publish(DEVICE_LABEL); 

    timer = millis();
  }
  ubidots.loop();
}