#include "UbidotsEsp32Mqtt.h"
#include <DHT.h>
#include <TFT_eSPI.h>
#include <SPI.h>


#define DHTPIN 27
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
TFT_eSPI tft = TFT_eSPI();

const char *UBIDOTS_TOKEN = "BBFF-n53TVIOnKbhirkPbOBXi3XVNmzYPm5";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "UPBWiFi";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "sensor:_x1";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL_1 = "humedad"; // Put here your Variable label to which data  will be published
const char *VARIABLE_LABEL_2 = "temperatura";


const int PUBLISH_FREQUENCY = 1000; // Update rate in milliseconds

unsigned long timer;
uint8_t analogPin = 27; // Pin used to read data from GPIO34 ADC_CH6.

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

void setup() {
    //SCREEN
  tft.init();
  tft.fillScreen(TFT_RED);
  tft.setTextColor(TFT_BLACK);
 
  //DHT
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));
  dht.begin();

  //UBIDOTS
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  timer = millis();
}

void loop() {
  delay(10000);
  float h=dht.readHumidity();
  float t=dht.readTemperature();

  if(isnan(h) || isnan(t)){
    Serial.println(F("ERROR EN LECTURA"));
    
    return;
  }
  Serial.print(F("Humedad: "));
  Serial.print(h);
  String humedad = "Humedad :" + String(h);
  tft.drawString(humedad,10,30);
  Serial.print(F("% Temperatura: "));
  Serial.print(t);
  Serial.println(F("°C"));
  String temperatura = "Temperatura :" + String(t);  tft.drawString(temperatura,10,10);
  
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  if (abs(millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    float value = analogRead(analogPin);
    ubidots.add(VARIABLE_LABEL_1, h);
    ubidots.add(VARIABLE_LABEL_2, t);
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  ubidots.loop();
}