// PIR Sensor
int led = 13;                // the pin that the LED is atteched to
int sensor = 9;              // the pin that the sensor is atteched to PIR sensor
int state = LOW;             // by default, no motion detected
int valpir = 0;                 // variable to store the sensor status (value)



int val = 0;  
int LED = 12; 
int isFlamePin = 7;  // This is our input pin
int isFlame = HIGH;
#include "DHT.h"
#define DHTPIN 2 
#define MQ2pin (1)
float sensorValue;

#include <Arduino_LSM6DS3.h>
#include <Firebase_Arduino_WiFiNINA.h>

#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);


#define FIREBASE_HOST " host id"
#define FIREBASE_AUTH " pass "
#define WIFI_SSID "admin"
#define WIFI_PASSWORD "admin@1234"
FirebaseData firebaseData;


String path = "/IMU_LSM6DS3";
String jsonStr;
 void setup()  
 {  
   Serial.begin(9600); // sensor buart rate  
   pinMode(3,OUTPUT); //  LED PIN  
   pinMode(LED, OUTPUT); // put onboard LED as output
   pinMode(isFlamePin, INPUT); //flame sensor should be input as it is giving data
    dht.begin();

  pinMode(led, OUTPUT);      // initalize LED as an output
  pinMode(sensor, INPUT);    // initialize sensor as an input
  
  Serial.print("Connecting to WiFi…");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(300);
  }
  Serial.print(" IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);
  Serial.println("Kandarp");
 
 }  
 void loop()   
 {  
  sensorValue = analogRead(MQ2pin);
 
 // PIR Sensor
 
  valpir = digitalRead(sensor);   // read sensor value
    if (valpir == HIGH) {           // check if the sensor is HIGH
      digitalWrite(led, HIGH);   // turn LED ON
      delay(500);                // delay 100 milliseconds 
      
      if (state == LOW) {
        Serial.println("Motion detected!"); 
        state = HIGH;       // update variable state to HIGH
      }
    } 
    else {
        digitalWrite(led, LOW); // turn LED OFF
        delay(500);             // delay 200 milliseconds 
        
        if (state == HIGH){
          Serial.println("Motion stopped!");
          state = LOW;       // update variable state to LOW
      }
    }

  // DHT sensor
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);


  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
   isFlame = digitalRead(isFlamePin);

     

  
  val = analogRead(A0); // LDR Sensor output pin connected  
  int newval  =  val/10; 
  
  analogWrite(3 , newval);
    if (isFlame== LOW)  //if it is low
  {
    Serial.println("FLAME, FLAME, FLAME"); //Print Flame Flame
     digitalWrite(LED, HIGH);  //LED on
    
  }
  else                           //if not
  {                               
    Serial.println("no flame"); //print no  flame
      //off the LED
     
       digitalWrite(LED, LOW); 
  }
  Serial.print("Smoke sensor value :");
  Serial.println(sensorValue);
  if(sensorValue > 200)
  {
    Serial.print(" | Smoke detected!");
  }
  
  
  jsonStr = "{\"Humidity Sensor Value\":" + String(h,6) + ",\"Temperature Sensor Value(in celcius)\":" + String(t,6)+" ,\"Temperature Sensor Value(in Fahrenheit)\":" + String(f,6)+ " ,\"Flame Sensor \":" + String(isFlame)+  " ,\"LDR Sensor Value\":" + String(val)+ " ,\"Smoke Sensor Value\":" + String(sensorValue)+ ",\"PIR Sensor Value\":" + String(sensor)+ "}";
 if (Firebase.pushJSON(firebaseData, path + "/data", jsonStr)) {
      Serial.println(firebaseData.dataPath() + " = " + firebaseData.pushName());
    }
    else {
      Serial.println("hello");
      Serial.println("Error: " + firebaseData.errorReason());
    }
  
  
  
  Serial.println("");


  Serial.print(F("Humidity: "));
  Serial.println(h);
  Serial.print(F("%  Temperature: "));
  Serial.println(t);
  Serial.print(F("°C "));
  Serial.println(f);
  delay(1000);
  } 
