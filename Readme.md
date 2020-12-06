# Environment Monitoring using Esp32 Cam and DHT11

It is a basic IOT based device which is used to monitor Temperature, Humidity and Heat Index of a surrounding area where this device is installed.

### DHT11 Temperature and Humidty Sensor

DHT11 Temperature & Humidity Sensor features a temperature & humidity sensor complex with a calibrated digital signal output. By using the exclusive digital-signal-acquisition technique and temperature & humidity sensing technology, it ensures high reliability and excellent long-term stability. This sensor includes a resistive-type humidity measurement component and an NTC temperature measurement component, and connects to a high performance  MCU's, offering excellent quality, fast response, anti-interference ability and cost-effectiveness.



<img src="images\DHT11.jpg" style="zoom:40%;" />



Each DHT11 element is strictly calibrated in the laboratory that is extremely accurate on humidity calibration. The calibration coefficients are stored as programmes in the OTP memory, which are used by the sensor’s internal signal detecting process. The single-wire serial interface makes system integration quick and easy. Its small size, low power consumption and up-to-20 meter signal transmission making it the best choice for various applications, including those most demanding ones. The component is 4-pin single row pin package. It is convenient to connect and special packages can be provided according to users’ request.



### Esp32 Cam

The ESP32-CAM board is an affordable development board that combines an ESP32-S chip, an OV2640 camera, several GPIOs to connect peripherals and a microSD card slot. It allows you to set up a video streaming web server, build a surveillance camera, take photos, face recognition and detection, and much more.



<img src="images\Esp32_cam.jfif" style="zoom:150%;" />





### Hardware Requirements

1. Esp32 Cam
2. DHT11 Temperature and Humidity Sensor
3. USB to TTL Programmer (preferably CH340G & FTDI programmer)
4. Breadboard
5. Jumper wires

### Header Files used

-  **`WiFi.h`** : This header files includes builtin WiFi functions which helps us to configure WiFi either as a  server or as a client.

-  **`DHT.h`** : This header files includes builtin functions for interfacing DHT11 sensor.

### Hardware Connections

- #### **PROGRAMMING**

  ​                                             **Esp32 Cam -----------FTDI Programmer**

  ​                                                   VoR------------------------- Tx

  ​                                                   VoT--------------------------Rx

  ​                                                   Io0---------------------------Gnd

  ​                                                    5v ---------------------------Vcc

  ​                                                   Gnd  ------------------------Gnd



####           Connection:-

​              

<img src="images\Esp32_Programming.jpg" style="zoom:50%;" />



- #### **DHT11 Interfacing**

​                                              **Esp32-----------------------DHT11----------------------- FTDI Programmer**

​                                                 Io4 --------------------------DATA---------------------------------- NC

​                                                +5v ---------------------------+5v  -----------------------------------+5v

​                                                Gnd--------------------------- Gnd --------------------------------- Gnd



####       **Connection**:-

<img src="images\DHT11_connection.jpg" style="zoom:50%;" />

### Server Setup



- Open www.thingspeak.com in a web browser.
- Login or Signup.
- Create New Channel.
- Set    **Field 1** as **Humidity%**,   **Field 2**  as **Temperature  °C**, **Field 3**  as **Temperature  K**  &  **Fields 4** as **Heat Index °C** and save the channel.
- Copy the **Write API Key** and paste it in the source code.



##### Channel Fields:-

<img src="images\1.png" style="zoom:80%;" />

##### Serial Monitor:-

<img src="images\2.png" style="zoom:90%;" />

### Source Code



```c++
#include <DHT.h>
#include <WiFi.h>

String apiWritekey = "***************";    //Paste yout API key here
const char* ssid = "***********";       // Your SSID 
const char* password = "***********" ;        // Yout Password
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
```

