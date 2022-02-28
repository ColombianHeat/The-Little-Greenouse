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

const char *plantModeArray[] = {"Tomato", "Cherry", "Grape"};
int plantMode = 0;

// constants used for operation of the water pump
const int pumpPin = 13;
unsigned long wateringInterval = 10000; //how long to wait until checking whether or not to water (default 12 hours or 43,200,000 ms)
unsigned long pumpActivationLength = 6000; // when activated, the pump will turn on for this much time
int pumpState = LOW; // determines whether pump is on or off
int lowMoisture = 630; // below this threshold, the pump will activate when it reaches the end of the watering interval
//unsigned long currentWaterMillis = millis();
unsigned long previousWaterMillis = 0; // stores how long it's been since checking moisture levels for watering purposes

// constants used for operation of the grow light
const int lightPin = 10;
unsigned long lightInterval = 24000; // Period of the grow light cycle (default 24 hours or 86,400,000 ms)
unsigned long lightActivationLength = 8000; // how long to leave the light on per period (default 10 hours or 36,000,000 ms)
int lightState = HIGH; // determines whether the light is on or off
unsigned long previousLightMillis = 0; // used to keep time, which will determine when the light will turn on/off

// constants used for operation of the fan
const int fanPin = 9;
unsigned long fanInterval = 12000; // Period of the fan cycle (default 120 minutes or 7,200,000 ms)
unsigned long fanActivationLength = 6000; // how long to leave the fan on per period (default 60 minutes or 3,600,000 ms)
int fanState = HIGH; // determines whether fan is on or off
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
    //if (sensorValue<=lowMoisture) {
      previousWaterMillis = millis();
      
      if (pumpState == LOW) {
      pumpState = HIGH;         // turn pump on if soil moisture is below threshold
      }
  Serial.print("PUMP ON: ");
  Serial.println(millis());
  digitalWrite(pumpPin, pumpState);
  delay(pumpActivationLength);  // if pump was turned on by the function, wait and turn it off

  if (pumpState == HIGH) {
    pumpState = LOW;
    Serial.print("PUMP OFF: ");
    Serial.println(millis());
    digitalWrite(pumpPin, pumpState);
  }
      //}
  }
}

// note to self: the light will be OFF for the first full light cycle, then it will begin to cycle properly
void growLight() {  // this function determines when to turn on/off the grow light
  if (millis() - previousLightMillis >= lightInterval && lightState == LOW) {
    previousLightMillis = millis();
    lightState = HIGH;         // turn light on if it was off
    Serial.print("GROW LIGHT ON: ");
    Serial.println(millis());
    digitalWrite(lightPin, lightState);
  }
  else if (millis() - previousLightMillis >= lightActivationLength && lightState == HIGH) {
    lightState = LOW; 
    Serial.print("GROW LIGHT OFF: ");
    Serial.println(millis());
    digitalWrite(lightPin, lightState);
  }
}

void fan(){  // this function determines when to turn on/off the fan
  if (millis() - previousFanMillis >= fanInterval && fanState == LOW) {
    previousFanMillis = millis();
    fanState = HIGH;         // turn light on if it was off
    Serial.print("FAN ON: ");
    Serial.println(millis());
    digitalWrite(fanPin, fanState);
  }
  else if (millis() - previousFanMillis >= fanActivationLength && fanState == HIGH) {
    fanState = LOW; 
    Serial.print("FAN OFF: ");
    Serial.println(millis());
    digitalWrite(fanPin, fanState);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("Temperature & Humidity + Moisture Level + Water Level Data"));
  dht.begin();
  
  pinMode(sensorPower, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  // Set to LOW so no power flows through the sensor
  digitalWrite(sensorPower, LOW);

  lcd.begin(16, 2);
}

void loop() {
  if (strcmp(plantModeArray[plantMode], "Tomato") == 0) {
    Serial.println("TOMATO MODE ENGAGED");
    wateringInterval = 10000;
    pumpActivationLength = 6000;
    lowMoisture = 630; // Maybe this can be the same for all modes??

    lightInterval = 24000;
    lightActivationLength = 6000;

    fanInterval = 12000;
    fanActivationLength = 6000;
  }
  else if (strcmp(plantModeArray[plantMode], "Cherry") == 0) {
    Serial.println("CHERRY MODE ENGAGED");
    wateringInterval = 10000;
    pumpActivationLength = 6000;
    lowMoisture = 630; // Maybe this can be the same for all modes??

    lightInterval = 24000;
    lightActivationLength = 6000;

    fanInterval = 12000;
    fanActivationLength = 6000;
  }
  else if (strcmp(plantModeArray[plantMode], "Grape") == 0) {
    Serial.println("GRAPE MODE ENGAGED");
    wateringInterval = 10000;
    pumpActivationLength = 6000;
    lowMoisture = 630; // Maybe this can be the same for all modes??

    lightInterval = 24000;
    lightActivationLength = 6000;

    fanInterval = 12000;
    fanActivationLength = 6000;
  }
  else {
    Serial.println("No valid plant mode selected!");
  }

  for (int i = 0; i <= 100; i++) 
  { 
   sensorValue = sensorValue + analogRead(SensorPin); 
   delay(1); 
  } 
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
    //return;
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

  
lcd.clear();

   lcd.print("Temperature:");
   lcd.setCursor(0,1);//set the cursor on the second row and first column
   lcd.print((float)dht.readTemperature(),0);//print the temperature
   lcd.print("C");
   delay(1000);
   lcd.clear();
   lcd.print("Humidity:");
   lcd.setCursor(0,1);//set the cursor on the second row and first column
   lcd.print((float)dht.readHumidity(),0);//print the humidity
   lcd.print("%");
   delay(1000);
   lcd.clear();
   lcd.print("Moisture level:");
      lcd.setCursor(0,1);//set the cursor on the second row and first column
lcd.print((sensorValue),0);
   delay(1000);
   lcd.clear();
      lcd.print("Water level:");
         lcd.setCursor(0,1);//set the cursor on the second row and first column
lcd.print(level);
   delay(1000);
   lcd.clear();

  waterPump(); // calls for water pump function which will activate the pump when needed 
  growLight(); // calls for grow light function
  fan();       // calls for fan function
  Serial.println();
   
}
