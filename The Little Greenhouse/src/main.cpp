#include <Arduino.h>

#include <Adafruit_Sensor.h>
#include "DHT.h"
#include <LiquidCrystal.h>
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define SensorPin A1 
#define sensorPower 7
#define sensorPin A0
float sensorValue = 0; 
int val = 0;
DHT dht(DHTPIN, DHTTYPE);
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// constants used for operation of the water pump
int pumpPin = 13;
const long wateringInterval = 6000; //how long to wait until checking whether or not to water (default 12 hours or 43,200,000 ms)
int pumpActivationLength = 6000; // when activated, the pump will turn on for this much time
int pumpState = LOW; // determines whether pump is on or off
int lowMoisture = 630; // below this threshold, the pump will activate when it reaches the end of the watering interval
//unsigned long currentWaterMillis = millis();
unsigned long previousWaterMillis = 0; // stores how long it's been since checking moisture levels for watering purposes

// constants used for operation of the grow light
int lightPin = 10;
const long lightInterval = 86400000; // how long to wait until turning on the light (default 24 hours or 86,400,000 ms)
const long lightActivationLength = 36000000; // how long to leave the light on (default 10 hours or 36,000,000 ms)
int lightState = LOW; // determines whether the light is on or off
unsigned long previousLightMillis = 0; // used to keep time, which will determine when the light will turn on/off

// constants used for operation of the fan
int fanPin = 9;
const long fanInterval = 7200000; // how long to wait until starting the fan (default 120 minutes or 7,200,000 ms)
const long fanActivationLength = 3600000; // how long to run the fan for (default 60 minutes or 3,600,000 ms)
int fanState = LOW; // determines whether fan is on or off
unsigned long previousFanMillis = 0; // used to keep time, which will determine when the fan will turn on/off


int readSensor() {
  digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
  delay(10);              // wait 10 milliseconds
  val = analogRead(sensorPin);    // Read the analog value form sensor
  digitalWrite(sensorPower, LOW);   // Turn the sensor OFF
  return val;             // send current reading
}

void waterPump(){  // this function determines when to turn on the water pump
  if (millis() - previousWaterMillis >= wateringInterval) {
    Serial.println(millis());
    //if (sensorValue<=lowMoisture) {
      previousWaterMillis = millis();
      
      if (pumpState == LOW) {
      pumpState = HIGH;         // turn pump on if soil moisture is below threshold
      }

  digitalWrite(pumpPin, pumpState);
  delay(pumpActivationLength);  // if pump was turned on by the function, wait and turn it off

  if (pumpState == HIGH) {
    pumpState = LOW;
    digitalWrite(pumpPin, pumpState);
  }
      //}
  }
}

void growLight() {  // this function determines when to turn on/off the grow light
    if (millis() - previousLightMillis >= lightInterval) {
    //Serial.println(millis());
      previousLightMillis = millis();
      
    if (lightState == LOW) {
    lightState = HIGH;         // turn light on if it was off
    }
    else if (lightState == HIGH) {
    lightState = LOW;          // turn light off if it was on
    }
    digitalWrite(lightPin, lightState);
  }
}

void fan(){  // this function determines when to turn on/off the fan
  if (millis() - previousFanMillis >= fanInterval) {
    //Serial.println(millis());
      previousWaterMillis = millis();
      
    if (fanState == LOW) {
    fanState = HIGH;         // turn fan on if it was off
    }
    else if (fanState == HIGH) {
    fanState = LOW;          // turn fan off if it was on
    }
    digitalWrite(fanPin, fanState);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("Temperature & Humidity + Moisture Level + Water Level Data"));
  dht.begin();
  
  Serial.begin(9600); 
  pinMode(sensorPower, OUTPUT);
  pinMode(pumpPin,OUTPUT);
  // Set to LOW so no power flows through the sensor
  digitalWrite(sensorPower, LOW);
  
  Serial.begin(9600);

  
  Serial.begin(9600);

lcd.begin(16, 2);
}

void loop() {
  for (int i = 0; i <= 100; i++) 
  { 
   sensorValue = sensorValue + analogRead(SensorPin); 
   delay(1); 
  } 
 delay(1000); 
 sensorValue = sensorValue/100.0; 
 Serial.print("Moisture level: ");
 Serial.println(sensorValue); 
 delay(300);
  int level = readSensor();
  
  Serial.print("Water level: ");
  Serial.println(level);
  delay(300);
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
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

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("C "));
  Serial.print(f);
  Serial.print(F("F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("C "));
  Serial.print(hif);
  Serial.println(F("F"));
  Serial.println();
  Serial.println();

  
lcd.clear();

   lcd.print("Temperature:");
   lcd.setCursor(0,1);//set the cursor on the second row and first column
   lcd.print((float)dht.readTemperature(),0);//print the temperature
   lcd.print("C");
   delay(3000);
   lcd.clear();
   lcd.print("Humidity:");
   lcd.setCursor(0,1);//set the cursor on the second row and first column
   lcd.print((float)dht.readHumidity(),0);//print the humidity
   lcd.print("%");
   delay(3000);
   lcd.clear();
   lcd.print("Moisture level:");
      lcd.setCursor(0,1);//set the cursor on the second row and first column
lcd.print((sensorValue),0);
   delay(3000);
   lcd.clear();
      lcd.print("Water level:");
         lcd.setCursor(0,1);//set the cursor on the second row and first column
lcd.print(level);
   delay(1000);
   lcd.clear();

waterPump(); // calls for water pump function which will activate the pump when needed 
growLight(); // calls for grow light function
fan();       // calls for fan function
   
}