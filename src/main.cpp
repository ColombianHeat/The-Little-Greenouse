#include <Arduino.h>

#include <Adafruit_Sensor.h>
#include "DHT.h"
#include <LiquidCrystal.h>
#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define moistureSensorPin A1 
// #define sensorPower 7
#define levelSensorPin A0 

float moistureSensorValue = 0; 
int val = 0;
DHT dht(DHTPIN, DHTTYPE);
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

RTC_DS3231 rtc;

const char *plantModeArray[] = {"Tomato", "Cherry", "Grape"};
int plantMode = 0;

int level = 0; // Water level variable

// constants used for operation of the water pump
const int pumpPin = 13;
unsigned long wateringInterval = 5000; //how long to wait until checking whether or not to water (default 12 hours or 43,200,000 ms)
unsigned long pumpActivationLength = 1000; // when activated, the pump will turn on for this much time
int pumpState = LOW; // determines whether pump is on or off
int lowMoisture = 630; // below this threshold, the pump will activate when it reaches the end of the watering interval
//unsigned long currentWaterMillis = millis();
unsigned long previousWaterMillis = 0; // stores how long it's been since checking moisture levels for watering purposes
int waterLevelAlarm = LOW;

// constants used for operation of the grow light
const int lightPin = 10;
int lightCommand = LOW;
unsigned long lightInterval = 4000; // Period of the grow light cycle (default 24 hours or 86,400,000 ms)
unsigned long lightActivationLength = 1000; // how long to leave the light on per period (default 10 hours or 36,000,000 ms)
int lightState = HIGH; // determines whether the light is on or off
unsigned long previousLightMillis = 0; // used to keep time, which will determine when the light will turn on/off

// constants used for operation of the fan
const int fanPin = 9;
int fanCommand = LOW;
unsigned long fanInterval = 6000; // Period of the fan cycle (default 120 minutes or 7,200,000 ms)
unsigned long fanActivationLength = 3000; // how long to leave the fan on per period (default 60 minutes or 3,600,000 ms)
int fanState = HIGH; // determines whether fan is on or off
unsigned long previousFanMillis = 0; // used to keep time, which will determine when the fan will turn on/off


int readWaterLevel() {
  // digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
  // delay(10);              // wait 10 milliseconds
  val = analogRead(levelSensorPin);    // Read the analog value form sensor
  // digitalWrite(sensorPower, LOW);   // Turn the sensor OFF
  return val;             // send current reading
}

void waterPump(){  // this function determines when to turn on the water pump
  if (level < 150) {
    waterLevelAlarm = HIGH;
    return;
  }

  if (millis() - previousWaterMillis >= wateringInterval) {
    //if (moistureSensorValue<=lowMoisture) {
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

void growLight() {
  if (lightCommand == HIGH && lightState == LOW) {
    lightState = HIGH;
    Serial.println("GROW LIGHT ON");
    digitalWrite(lightPin, lightState);
  }
  else if (lightCommand == LOW && lightState == HIGH) {
    lightState = LOW;
    Serial.println("GROW LIGHT OFF");
    digitalWrite(lightPin, lightState);
  }
}

// void growLight() {  // this function determines when to turn on/off the grow light   // Keeping this in the back pocket just in case
//   if (millis() - previousLightMillis >= lightInterval && lightState == LOW) {
//     previousLightMillis = millis();
//     lightState = HIGH;         // turn light on if it was off
//     Serial.print("GROW LIGHT ON: ");
//     Serial.println(millis());
//     digitalWrite(lightPin, lightState);
//   }
//   else if (millis() - previousLightMillis >= lightActivationLength && lightState == HIGH) {
//     lightState = LOW; 
//     Serial.print("GROW LIGHT OFF: ");
//     Serial.println(millis());
//     digitalWrite(lightPin, lightState);
//   }
// }

void fan(){  // this function determines when to turn on/off the fan
  if (fanCommand == HIGH && fanState == LOW) {
    fanState = HIGH;         // turn fan on if it was off
    Serial.println("FAN ON");
    digitalWrite(fanPin, fanState);
  }
  else if (fanCommand == LOW && fanState == HIGH) {
    fanState = LOW; // turn fan off if it was on
    Serial.println("FAN OFF");
    digitalWrite(fanPin, fanState);
  }
}

void updateRTC() {
  const char txt[6][15] = { "year [4-digit]", "month [1~12]", "day [1~31]",
                            "hours [0~23]", "minutes [0~59]", "seconds [0~59]"};
  String str = "";
  long newDate[6];

  while (Serial.available()) {
    Serial.read();  // clear serial buffer
  }

  for (int i = 0; i < 6; i++) {

    Serial.print("Enter ");
    Serial.print(txt[i]);
    Serial.print(": ");

    while (!Serial.available()) {
      ; // wait for user input
    }

    str = Serial.readString();  // read user input
    newDate[i] = str.toInt();   // convert user input to number and save to array

    Serial.println(newDate[i]); // show user input
  }

  // update RTC
  rtc.adjust(DateTime(newDate[0], newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]));
  Serial.println("RTC Updated!");
  Serial.print("Current date is: ");
  Serial.print(newDate[0]);
  Serial.print("-");
  Serial.print(newDate[1]);
  Serial.print("-");
  Serial.print(newDate[2]);
  Serial.print("   ");
  Serial.print(newDate[3]);
  Serial.print(":");
  Serial.print(newDate[4]);
  Serial.print(":");
  Serial.print(newDate[5]);
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("Temperature & Humidity + Moisture Level + Water Level Data"));

  // pinMode(sensorPower, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  // Set to LOW so no power flows through the sensor
  // digitalWrite(sensorPower, LOW);

  dht.begin();
  rtc.begin();
  lcd.begin(16, 2);
}

void loop() {

  // Establishes time variables for later use
  int RTCyear = rtc.now().year();
  int RTCmonth = rtc.now().month();
  int RTCday = rtc.now().day();
  int RTChour = rtc.now().hour();
  int RTCminute = rtc.now().minute();
  int RTCsecond = rtc.now().second();

  // Selecting the current mode. Different modes have different timings for the output devices
  if (strcmp(plantModeArray[plantMode], "Tomato") == 0) {
    Serial.println("TOMATO MODE ENGAGED");
    wateringInterval = 4000;
    pumpActivationLength = 2000;
    lowMoisture = 630; // Maybe this can be the same for all modes??

    if ((waterLevelAlarm == LOW) && (lightCommand == LOW) && ( ((RTCsecond >= 0) && (RTCsecond <= 4)) | ((RTCsecond >= 10) && (RTCsecond <= 14)) | ((RTCsecond >= 20) && (RTCsecond <= 24)) | ((RTCsecond >= 30) && (RTCsecond <= 34)) | ((RTCsecond >= 40) && (RTCsecond <= 44)) | ((RTCsecond >= 50) && (RTCsecond <= 54)) )) {
      lightCommand = HIGH; // Light on every 10 seconds
    }

    if (waterLevelAlarm == LOW && (lightCommand == HIGH) && ( ((RTCsecond >= 5) && (RTCsecond <= 9)) | ((RTCsecond >= 15) && (RTCsecond <= 19)) | ((RTCsecond >= 25) && (RTCsecond <= 29)) | ((RTCsecond >= 35) && (RTCsecond <= 39)) | ((RTCsecond >= 45) && (RTCsecond <= 49)) | ((RTCsecond >= 55) && (RTCsecond <= 59)) )) {
      lightCommand = LOW; // Light off every 10 seconds
    }

    if (waterLevelAlarm == HIGH) { // THIS NEEDS TO BE REPLACED. THESE VARIABLES WERE TAKEN OUT OF THE GROW LIGHT FUNCTION.
      lightInterval = 2000;
      lightActivationLength = 1000;
    }

    if ((fanCommand == LOW) && ( ((RTCsecond >= 0) && (RTCsecond <= 4)) | ((RTCsecond >= 10) && (RTCsecond <= 14)) | ((RTCsecond >= 20) && (RTCsecond <= 24)) | ((RTCsecond >= 30) && (RTCsecond <= 34)) | ((RTCsecond >= 40) && (RTCsecond <= 44)) | ((RTCsecond >= 50) && (RTCsecond <= 54)) )) {
      fanCommand = HIGH; // Fan on every 10 seconds
    }

    if ((fanCommand == HIGH) && ( ((RTCsecond >= 5) && (RTCsecond <= 9)) | ((RTCsecond >= 15) && (RTCsecond <= 19)) | ((RTCsecond >= 25) && (RTCsecond <= 29)) | ((RTCsecond >= 35) && (RTCsecond <= 39)) | ((RTCsecond >= 45) && (RTCsecond <= 49)) | ((RTCsecond >= 55) && (RTCsecond <= 59)) )) {
      fanCommand = LOW; // Fan off every 10 seconds
    }
    
  }

  else if (strcmp(plantModeArray[plantMode], "Cherry") == 0) {
    Serial.println("CHERRY MODE ENGAGED");
    wateringInterval = 10000;
    pumpActivationLength = 6000;
    lowMoisture = 630; // Maybe this can be the same for all modes??

    if ((waterLevelAlarm == LOW) && (lightCommand == LOW) && ( (RTChour > 6) && (RTChour < 22) )) {
      lightCommand = HIGH; // Light on between 7:00 am and 9:59 pm
    }

    if ((waterLevelAlarm == LOW) && (lightCommand == HIGH) && ( (RTChour > 21) | (RTChour < 7) )) {
      lightCommand = LOW; // Light off between 10:00 pm and 6:59 am
    }

    if (waterLevelAlarm == HIGH) {
      lightInterval = 2000;
      lightActivationLength = 1000;
    }

    if ((fanCommand == LOW) && ( (RTChour > 6) && (RTChour <= 23) )) {
      fanCommand = HIGH; // Fan on between 7:00 am and 11:59 pm
    }

    if ((fanCommand == HIGH) && ( (RTChour >= 0) | (RTChour < 7) )) {
      fanCommand = LOW; // Fan off between 12:00 am and 6:59 am
    }
  }
  
  else if (strcmp(plantModeArray[plantMode], "Grape") == 0) {
    Serial.println("GRAPE MODE ENGAGED");
    wateringInterval = 10000;
    pumpActivationLength = 6000;
    lowMoisture = 630; // Maybe this can be the same for all modes??

    if ((waterLevelAlarm == LOW) && (lightCommand == LOW) && ( (RTChour > 7) && (RTChour < 17) )) {
      lightCommand = HIGH; //Light on between 8:00 am and 4:59 pm
    }

    if ((waterLevelAlarm == LOW) && (lightCommand == HIGH) && ( (RTChour > 16) | (RTChour < 8) )) {
      lightCommand = LOW; // Light off between 5:00 pm and 7:59 am
    }

    if ((fanCommand == LOW) && ( (RTChour > 7) && (RTChour < 19) )) {
      fanCommand = HIGH; // Fan on between 8:00 am and 6:59 pm
    }

    if ((fanCommand == HIGH) && ( (RTChour > 18) | (RTChour < 8) )) {
      fanCommand = LOW; // Fan off between 7:00 pm and 7:59 am
    }
  }

  else {
    Serial.println("No valid plant mode selected!");
  }

  // Updates RTC time if user inputs a "u" to the serial monitor
  if (Serial.available()) {
    char input = Serial.read();
  if (input == 'u') updateRTC();
  }

  // Averages 100 moisture readings to use as the recorded output
  for (int i = 0; i <= 100; i++) 
  { 
   moistureSensorValue = moistureSensorValue + analogRead(moistureSensorPin); 
   delay(1); 
  } 
 moistureSensorValue = moistureSensorValue/100.0; 

  // Prints to serial monitor the current time
  Serial.print(RTCyear);
  Serial.print("-");
  Serial.print(RTCmonth);
  Serial.print("-");
  Serial.print(RTCday);
  Serial.print("   ");
  Serial.print(RTChour);
  Serial.print(":");
  Serial.print(RTCminute);
  Serial.print(":");
  Serial.print(RTCsecond);
  Serial.println();

  // Printing sensor readings to serial monitor
 Serial.print("Moisture level: ");
 Serial.println(moistureSensorValue); 
 delay(200);
  
  level = readWaterLevel(); 
  Serial.print("Water level: ");
  Serial.println(level);
  delay(200);
  
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

  // Clears and prints sensor values to LCD
  lcd.clear();

    lcd.print("TEMP:");
    lcd.setCursor(5,0); // 6th column, 1st row
    lcd.print((float)dht.readTemperature(),0);//print the temperature
    lcd.setCursor(7,0); // 8th column, 1st row
    lcd.print("C HUM:");
    lcd.setCursor(13,0); // 14th column, 1st row
    lcd.print((float)dht.readHumidity(),0); // print the humidity
    lcd.setCursor(15,0); // 16th column, 1st row
    lcd.print("%");

    lcd.setCursor(0,1); // 1st column, 2nd row
    lcd.print("MOIS:"); 
    lcd.setCursor(5,1); // 6th column, 2nd row
    lcd.print(100 - (((moistureSensorValue) - 400)/450*100),0);
    lcd.setCursor(7,1);
    lcd.print(" H20:");
    lcd.print((level)/535*100);
    delay(200);

  if ((waterLevelAlarm == HIGH) && (level > 200)) {
    waterLevelAlarm = LOW;
  }

  // If water level is low, don't run the pump
  if (waterLevelAlarm == LOW) {
    waterPump(); // calls for water pump function
  }
  
  growLight(); // calls for grow light function
  fan();       // calls for fan function
  Serial.println();
   
}
