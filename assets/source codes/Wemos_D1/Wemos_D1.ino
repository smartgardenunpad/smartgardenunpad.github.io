/* PKM SMART GARDEN
 *  2016 (c) Tim Smart Garden
 *  
 * Sistem Monitoring Pertumbuhan Tanaman 
 * Intensitas Cahaya (lux), Humiditas (%rh), Temperatur (C)
 * Data dikirim ke Thingspeak via GET HTTP
 *  
 * ======================================================= 
 * Wemos D1 R1 Pinout
 * 
 * Sensor :
 *      DHT22 : D9 / 2 /IO2
 *      BH1750: D14 (SDA) / 4 / IO4, D15(SCL) / 5 / IO5
 * 
 * Wake up from Deepsleep Mode : D2 / 16 / IO16 -> RST
 */

#include <DHT.h>
#include <ESP8266WiFi.h>

#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include <BH1750.h>
#include <Wire.h> // I2C Library

#define DHTPIN 2     // what pin we're connected to

#define DHTTYPE DHT22   // DHT 22  (AM2302)

ESP8266WiFiMulti WiFiMulti;
BH1750 LightSensor;

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

static char lux_str[15];
static char hum_str[15];
static char tem_str[15];

int sleepTimeS = 25;

// Fungsi Kirim Data via HTTP GET (~1 Detik)
void sendTheData(String link){
  HTTPClient http;
// ganti dengan URL API Last Feed punyamu sendiri
        http.begin(link);

        // mulai koneksi dan ambil HTTP Header
        int httpCode = http.GET();

        // httpCode akan bernilai negatif bila error
        if(httpCode > 0)
        {
            // cetak httpCode ke Serial
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);


            // bila nilai dari server diterima
//            if(httpCode == HTTP_CODE_OK)
//            {
//                // cetak string json dari server
//                String json = http.getString();
//                Serial.println(json);
//            }

        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        // tutup koneksi HTTP
        http.end();
        delay(1000);
}

int var_toggle = (1 ^ 0);
int toggle = 1;
int try_count=0;

void connectWifi(int toggle){
  if(toggle== 1){
    Serial.println("Trying Unpad Wifi...");
    WiFiMulti.addAP("UnpadWiFi");
    return;
  }else{
    Serial.println("Trying kungkang...");
    WiFiMulti.addAP("kungkang", "slothian");
    return;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println();Serial.print("Device Started!");
 
  dht.begin();
  LightSensor.begin();

  connectWifi(var_toggle);
  // set Wifi SSID dan passwordnya
  //WiFiMulti.addAP("UnpadWiFi");
  //WiFiMulti.addAP("kungkang","slothian");
}

void loop() {

  // Minimal delay (2 s) buat pembacaan DHT
  delay(2*1000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float hum = dht.readHumidity();
  // Read temperature as Celsius
  float tem = dht.readTemperature();
  
  uint16_t lux = LightSensor.readLightLevel();

  Serial.println();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.print(" lx\t");
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(tem)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: "); 
  Serial.print(hum);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(tem);
  Serial.println(" °C ");


  // tunggu koneksi Wifi
    if((WiFiMulti.run() == WL_CONNECTED))
    { try_count=0;
      dtostrf(lux, 5, 2, lux_str);
      dtostrf(hum, 5, 2, hum_str);
      dtostrf(tem, 5, 2, tem_str);

      String url="http://api.thingspeak.com/update?api_key=PX3EE29I38BWOXOS&field1=";
      url=url+lux_str;
      url=url+"&field2=";
      url=url+hum_str;
      url=url+"&field3=";
      url=url+tem_str;
     
      sendTheData(url);
        //delay(25*1000);

        Serial.print("Going to sleep for ");
        Serial.print(sleepTimeS);
        Serial.println(" s");
        ESP.deepSleep(sleepTimeS * 1000000);
        delay(500);
      }else{
        Serial.print("failed to connect ");
        Serial.println(try_count);
        try_count++;
        if(try_count==5){
          try_count=0;
        toggle^=var_toggle;
        connectWifi(toggle);
        }
      }

//  Serial.println(millis()/1000);
// deepSleep time is defined in microseconds. Multiply
  // seconds by 1e6 
//ESP.deepSleep(15 * 1000000);
}



