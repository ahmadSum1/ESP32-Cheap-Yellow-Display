#include "DHT.h"
#define DHTPIN 27 // Digital pin connected to the DHT sensor
// #define DHTTYPE DHT11   // DHT 11
//  #define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321
#define DHTTYPE DHT21 // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
#ifdef dht22
DHT sen_ht = dht;
#define AHTXX_ERROR 255
#endif

#ifdef aht21
#include <Wire.h>
#include <AHTxx.h>

#define I2C_SDA 27
#define I2C_SCL 22

AHTxx sen_ht(AHTXX_ADDRESS_X38, AHT2x_SENSOR); // sensor address, sensor type

#endif
#define forceReRead false
unsigned long previousMillis = 0; // will store last time was updated
const long interval = 3000;       // interval at which to sample (milliseconds)

void setup_ht()
{

#ifdef dht22
  Serial.println(F("DHT22 init..."));
  sen_ht.begin();
#endif
#ifdef aht21
  Serial.println(F("AHT21 init..."));
  sen_ht.begin(I2C_SDA, I2C_SCL);
  delay(1000);
#endif

  Wire.setClock(400000); // experimental I2C speed! 400KHz, default 100KHz
}

void loop_ht()
{
  // Wait a few seconds between measurements.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you took measurement
    previousMillis = currentMillis;

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    // Read temperature as Celsius (the default)
    float t = sen_ht.readTemperature();
    float h = sen_ht.readHumidity(forceReRead);
    // // Read temperature as Fahrenheit (isFahrenheit = true)
    // float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).

    if (isnan(h) || isnan(t) || (h == AHTXX_ERROR) || (t == AHTXX_ERROR))
    {
      Serial.println(F("Failed to read from xHT sensor!"));
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
    // json
    StaticJsonDocument<96> doc;
    doc["Sensor"] = "dht22";
    doc["Temperature"] = t + 0.0001; // to avoid int casting
    doc["Humidity"] = h + 0.0001;    // to avoid int casting
    doc["Heat Index"] = hic;
    dht_output = "";
    serializeJson(doc, dht_output);

    Serial.println(dht_output);

    if (WiFi.status() == WL_CONNECTED)
    {
      if (ledState)
      {
        rgb_LED_BUILTIN.brightness(0, 255, 0, 25); // 50% brightness
      }
      // UDP broadcast
      // asudp_pub(dht_output);
    }
    else
    {
      Serial.println("No Wifi...");

      rgb_LED_BUILTIN.flash(RGBLed::RED, 50); // Interval 100ms
    }
  }
}