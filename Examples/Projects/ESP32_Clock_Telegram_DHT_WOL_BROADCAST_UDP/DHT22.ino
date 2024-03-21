
#include "DHT.h"

#define DHTPIN 27  // Digital pin connected to the DHT sensor
//#define DHTTYPE DHT11   // DHT 11
// #define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321
#define DHTTYPE DHT21  // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);


unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 2000;        // interval at which to blink (milliseconds)


void setup_dht() {

  Serial.println(F("DHT22 init..."));

  dht.begin();
}

void loop_dht() {
  // Wait a few seconds between measurements.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // // Read temperature as Fahrenheit (isFahrenheit = true)
    // float f = dht.readTemperature(true);


    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    // float hif = dht.computeHeatIndex(f, h);
    // // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    // Serial.print(F("Humidity: "));
    // Serial.print(h);
    // Serial.print(F("%  Temperature: "));
    // Serial.print(t);
    // Serial.print(F("°C Heat index: "));
    // // Serial.print(f);
    // // Serial.print(F("°F  Heat index: "));
    // Serial.print(hic);
    // Serial.print(F("°C "));
    // // Serial.print(hif);
    // // Serial.println(F("°F"));
    // dht_output = (String) "Humidity: " + h + "%  Temperature: " + t + "°C Heat index: " + hic + "°C";
    //json
    StaticJsonDocument<96> doc;
    doc["Sensor"] = "dht22";
    doc["Temperature"] = t+0.0001;  //to avoid int casting
    doc["Humidity"] = h+0.0001;     //to avoid int casting
    doc["Heat Index"] = hic;
    dht_output = "";
    serializeJson(doc, dht_output);


    Serial.println(dht_output);

    if (WiFi.status() == WL_CONNECTED) {
      // UDP broadcast
      loop_asudp(dht_output);
    }
    else{
      Serial.println("No Wifi...");
    }
  }
}