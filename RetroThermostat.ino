

#include "ESP8266WiFi.h"
#include "DHT.h"
#define DHTPIN 5    //digital pin connected to
                    //pin2 to D4 on esp board   pin5 to D1 on esp board

#define DHTTYPE DHT22  // DHT 22

DHT dht(DHTPIN,DHTTYPE);


const char WEBSITE[] = "api.pushingbox.com"; //pushingbox API server
const String devid = "v3CA43978045C5DF"; //device ID from Pushingbox 

const char* MY_SSID = "Kinetic_500757";
const char* MY_PWD =  "fg5nvwpgf6";

//-----------------------------------------------------

#include <ESP8266WiFi.h>

#include <WiFiClient.h>
 
#include <ESP8266HTTPClient.h> // http web access library
 
#include <ArduinoJson.h> // JSON decoding library

int averageTempF = 0;

#include <Servo.h>

Servo servo;

//char ssid[] = SECRET_SSID; //  your network SSID (name) 
//char pass[] = SECRET_PSW;//  your network PASSWORD () 
char ssid[] = "Kinetic_500757"; //  your network SSID (name) 
//char ssid[] = "xfinitywifi"; //  your network SSID (name) 
char pass[] = "fg5nvwpgf6";//  your network PASSWORD () 
//open weather map api key 
String apiKey= "bffc89c840b6a906cfe393c82b496932"; 
//the city you want the weather for 
String location= "Williamsport, US"; 
int status = WL_IDLE_STATUS; 
int override = 0;
char server[] = "api.openweathermap.org";


WiFiClient client;



void setup() {
  
  // DHT22/database initilization:
  Serial.begin(115200);
  dht.begin();
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  Serial.println("going into wl connect");

  while (WiFi.status() != WL_CONNECTED) //not connected,..waiting to connect
    {
      delay(1000);
      Serial.print(".");
    }
  Serial.println("wl connected");
  Serial.println("");
  Serial.println("Credentials accepted! Connected to wifi\n ");
  Serial.println("");


// appreture initialization:
  servo.attach(D8);
  
  Serial.begin(115200);//set serial comm baud rate
  delay(1000);

  WiFi.begin(ssid, pass);
  Serial.print("Connecting.");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");
  delay(1000);

  

}

void loop() {

  
  // DHT22/Database control:
  
  //Wait between measurements longer then normal to slow donwn
  //google sheet populate, as We dont want to exceed free service quota
  delay(10000); //10 seconds, (sampling rate vs. service call quota)

  float humidityData = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float celData = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float fehrData = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidityData) || isnan(celData) || isnan(fehrData))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hicData = dht.computeHeatIndex(celData, humidityData, false);
  // Compute heat index in Fahrenheit (the default)
  float hifData = dht.computeHeatIndex(fehrData, humidityData);

  //Print to Serial monitor or Terminal of your chocice at 115200 Baud
  Serial.print("Humidity: ");
  Serial.print(humidityData);
  Serial.print(" %\t");
  Serial.print("Temperature in Cel: ");
  Serial.print(celData);
  Serial.print(" *C ");
  Serial.print("Temperature in Fehr: ");
  Serial.print(fehrData);
  Serial.print(" *F\t");
  Serial.print("Heat index in Cel: ");
  Serial.print(hicData);
  Serial.print(" *C ");
  Serial.print("Heat index in Fehr: ");
  Serial.print(hifData);
  Serial.print(" *F\n");
    
  WiFiClient client;  //Instantiate WiFi object

    //Start or API service using our WiFi Client through PushingBox
    if (client.connect(WEBSITE, 80))
      { 
         client.print("GET /pushingbox?devid=" + devid
       + "&humidityData=" + (String) humidityData
       + "&celData="      + (String) celData
       + "&fehrData="     + (String) fehrData
       + "&hicData="      + (String) hicData
       + "&hifData="      + (String) hifData
         );

      client.println(" HTTP/1.1"); 
      client.print("Host: ");
      client.println(WEBSITE);
      client.println("User-Agent: ESP8266/1.0");
      client.println("Connection: close");
      client.println();
      }





//appreture control:

 override = 0;
 getWeather();
 delay(860000000);//only check weather every 24hrs
}

void getWeather() {
 Serial.println("\nStarting connection to server...");
 // if you get a connection, report back via serial:
 if (client.connect(server, 80)) {
   Serial.println("connected to server");
   override = 1;
   // Make a HTTP request:
   client.print("GET /data/2.5/forecast?");
   client.print("q="+location);
   client.print("&appid="+apiKey);
   client.print("&cnt=3");
   client.println("&units=metric");
   client.println("Host: api.openweathermap.org");
   client.println("Connection: close");
   client.println();

 delay(1000);
 String line = "";
// Serial.println("yeet1");
// while (client.connected()) {
 while (override == 1) {
//  Serial.println("yeet2");
   line = client.readStringUntil('\n');
//   Serial.println(line);////////////////////////////////
   Serial.println("parsingValues");
   //create a json buffer where to store the json data
   StaticJsonBuffer<5000> jsonBuffer;
   JsonObject& root = jsonBuffer.parseObject(line);
   if (!root.success()) {
     Serial.println("parseObject() failed");
     return;
   }
   
 //get the data from the json tree
int day0temp = root["list"][0]["main"]["temp"];
int day1temp = root["list"][1]["main"]["temp"];
int day2temp = root["list"][2]["main"]["temp"];

int day0tempF = day0temp *(9/5)+32;
int day1tempF = day1temp *(9/5)+32;
int day2tempF = day2temp *(9/5)+32;

averageTempF = (day0tempF + day1tempF + day2tempF)/3;
 
 // Print values.
 Serial.println(day0tempF);
 Serial.println(day1tempF);
 Serial.println(day2tempF);
 Serial.println((day0tempF+day1tempF+day2tempF)/3); 
 Serial.println("Average:");
 Serial.println(averageTempF);

 servo.write(10);
 delay(1000);

//on/off
//averageTempF = 80;

// servo.write(180);

 if (averageTempF < 70){
  servo.write(180);
  delay(1000);
 }

 override = 0;
 }
   
 } 
  else {
   Serial.println("unable to connect");
 }


  

}
