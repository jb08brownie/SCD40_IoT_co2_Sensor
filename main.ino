#include <SensirionI2CScd4x.h>
#include <Wire.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

const char* ssid = "NAME_OF_WIFI";
const char* password = "WIFI_PASSWORD";

WiFiClient client;

unsigned long myChannelNumber = 2;
const char* myWriteAPIKey = "THINGSPEAK_CHANNEL_WRITE_API_KEY";

unsigned long sleepDuration = 600000000;

SensirionI2CScd4x scd4x;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    scd4x.begin(Wire);
    WiFi.mode(WIFI_STA);
    ThingSpeak.begin(client);
    scd4x.startPeriodicMeasurement();

    connectToWifi();

    if (WiFi.status() == WL_CONNECTED)  {
      readAndUploadSensorData();
    }
    Serial.println("Entering deep sleep for " + String(sleepDuration / 1000 / 1000) + " seconds.");
  ESP.deepSleep(sleepDuration);
}

void loop() {
  // Empty loop since all operations are done in setup()
}

void connectToWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to WiFi");
    int retries = 0;
    int maxRetries = 5;
    while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
      WiFi.begin(ssid, password);
      delay(5000);
      retries++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected to WiFi.");
    }  else {
        Serial.println("\nFailed to connect to WiFi!");
      }    
  }
}

void readAndUploadSensorData() {
  uint16_t co2 = 0;
  float temperature = 0.0f;
  float humidity = 0.0f;
  delay(100);
  scd4x.readMeasurement(co2, temperature, humidity);

  uint16_t error = scd4x.readMeasurement(co2, temperature, humidity);
  if (error) {
    Serial.print("Error reading measurement: ");
    Serial.println(error);
  }

  Serial.print("Co2:");
  Serial.print(co2);
  Serial.print("\t");
  Serial.print("Temperature:");
  Serial.print(temperature);
  Serial.print("\t");
  Serial.print("Humidity:");
  Serial.println(humidity);

  ThingSpeak.setField(1, co2);
  ThingSpeak.setField(2, temperature);
  ThingSpeak.setField(3, humidity);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x==200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code "+ String(x));    
  }

}


