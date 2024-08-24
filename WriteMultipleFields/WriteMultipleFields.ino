#include <DHT.h>
#include <WiFiNINA.h>
#include <ThingSpeak.h>
#include "secrets.h"

char wifiSSID[] = SECRET_SSID;     // Wi-Fi SSID
char wifiPassword[] = SECRET_PASS; // Wi-Fi password

WiFiClient wifiClient;

unsigned long channelNumber = SECRET_CH_ID;               // ThingSpeak Channel ID
const char * writeAPIKey = SECRET_WRITE_APIKEY;           // ThingSpeak API key

#define SENSOR_PIN 2           // DHT22 Sensor Pin
#define SENSOR_TYPE DHT22      // Sensor type is DHT22

DHT dhtSensor(SENSOR_PIN, SENSOR_TYPE);

void setup() 
{
  Serial.begin(9600);  
  dhtSensor.begin();          

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.begin(wifiSSID, wifiPassword) != WL_CONNECTED) 
  {
    Serial.print("_");
    delay(5000);
  }

  Serial.println("\nConnected to Wi-Fi");
  ThingSpeak.begin(wifiClient);  // Initialize ThingSpeak
}

void loop() 
{
  // Reading temperature and humidity data
  float currentTemperature = dhtSensor.readTemperature();
  float currentHumidity = dhtSensor.readHumidity();

  // Check if the reads are out of the expected range
  if (currentTemperature < -40.0 || currentTemperature > 80.0 || currentHumidity < 0.0 || currentHumidity > 100.0) 
  {
    Serial.println("Invalid readings from DHT sensor! Values are out of expected range.");
    return;
  }

  // Print the temperature and humidity to the Serial Monitor
  Serial.print("Current room Temperature: ");
  Serial.print(currentTemperature);
  Serial.println(" °C");

  Serial.print("Current room Humidity: ");
  Serial.print(currentHumidity);
  Serial.println(" %");

  // Update ThingSpeak field 1 with the temperature data
  ThingSpeak.setField(1, currentTemperature);
  
  // Update ThingSpeak field 2 with the humidity data
  ThingSpeak.setField(2, currentHumidity);

  // Write the data to ThingSpeak
  int statusCode = ThingSpeak.writeFields(channelNumber, writeAPIKey);

  if (statusCode == 200) 
  {
    Serial.println("Temperature and Humidity successfully sent to ThingSpeak.");
  } 
  else 
  {
    Serial.println("Error sending data to ThingSpeak. HTTP error code: " + String(statusCode));
  }

  // 60 seconds delay
  delay(60000);
}
