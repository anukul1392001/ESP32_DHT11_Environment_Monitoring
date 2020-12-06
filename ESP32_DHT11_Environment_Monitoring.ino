#include <DHT.h>
#include <WiFi.h>

String apiWritekey = "45J4JHNPFD8KKCIP"; 
const char* ssid = "Home Automation";
const char* password = "123456789" ;
const char* server = "api.thingspeak.com";

#define DHTPIN 4
#define DHTTYPE DHT11
 
WiFiClient client;

DHT dht(DHTPIN, DHTTYPE);
 
void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.disconnect();
  delay(10);
  WiFi.begin(ssid, password);
 
  Serial.println("\n");
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("ESP32 cam connected to wifi...");
  Serial.println(ssid);
  Serial.println("\n");
}
 
void loop() {
  
  delay(2000);
   int value=0;
   float humidity = dht.readHumidity();
   float temp_c = dht.readTemperature();
   float temp_f = dht.readTemperature(true);

     if (isnan(humidity) || isnan(temp_c) || isnan(temp_f)) {
    Serial.println(("Failed to read from DHT11 sensor!"));
    return;
  }

  // float heat_idx_f = dht.computeHeatIndex(temp_f, humidity);
   float heat_idx_c = dht.computeHeatIndex(temp_f, humidity, false);


  
     
 if (client.connect(server,80))
  {   
    String tsData = apiWritekey;
           
           tsData +="&field1=";
           tsData += int(humidity);
           tsData += "\r\n\r\n";


          
           tsData +="&field2=";
           tsData += int(temp_c);
           tsData += "\r\n\r\n";


          
           tsData +="&field3=";
           tsData += int(temp_f);
           tsData += "\r\n\r\n";


          
           tsData +="&field4=";
           tsData += int(heat_idx_c);
           tsData += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiWritekey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(tsData.length());
     client.print("\n\n");  // the 2 carriage returns indicate closing of Header fields & starting of data
     client.print(tsData);
     
     Serial.println("_______________________________________________________________");
     
     Serial.print("Humidity: ");
     Serial.println(humidity);
     Serial.print("Temperature °C: ");
     Serial.println(temp_c);
     Serial.print("Temperature °F: ");
     Serial.println(temp_f);
     Serial.print("Heat Index °C: ");
     Serial.println(heat_idx_c);
     
     Serial.println("Data Uploaded to Thingspeak server....");

     Serial.println("_______________________________________________________________");
  }
  client.stop();
  
  Serial.println("Waiting to upload next reading...");
  Serial.println("\n");
  
  delay(15000);
}
