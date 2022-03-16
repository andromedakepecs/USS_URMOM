/*
USS Urmom: Arduino B
External personal experiments
  3 thermistors (Andromeda/Carter)
    Monitors external temperature using 10k ohm thermistor in series with 10k ohm resistor
  Carbon monoxide (Alli)
    Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
  Humidity (Marshall)
    Using DHT Temperature Humidity sensor
    - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
    - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
Real Time Clock DS3234 module
  - https://github.com/sparkfun/SparkFun_DS3234_RTC_Arduino_Library 

@author Andromeda Kepecs
*/
#include <SparkFunDS3234RTC.h>
#include <SPI.h>

#include "DHT.h"

// Analog in pins
#define CONTROL_THERM_PIN 0
#define BLACK_THERM_PIN 1
#define WHITE_THERM_PIN 2
#define CARBON_MONOXIDE_PIN 3

// Digital out pins
#define DHT_PIN 2
#define RTC_PIN 10

// DHT Sensor
#define DHT_TYPE DHT22 // DHT 22  (AM2302), AM2321
DHT dht(DHT_PIN, DHT_TYPE);

// Steinhart-Hart Coefficients for a 10k ohm thermistor
const float SH_A = 0.001125308852122;
const float SH_B = 0.000234711863267; 
const float SH_C = 0.000000085663516;

// Value of resistor used in series with thermistor
const float RESISTANCE1 = 10000; 

// Timing 
const int DELAY = 5; // How often a reading is taken in seconds, TODO adjust
const int SEC = 1000;
const int BAUD_RATE = 9600;

void setup() {
  Serial.begin(BAUD_RATE);
  dht.begin();
  rtc.begin(RTC_PIN);

  // TODO Configure RTC
  
  int sec = 0;
  int min = 37;
  int hour = 15;
  int day_week = 3; // sunday = 1 etc
  int day = 9;
  int month = 3;
  int year = 22;
  rtc.setTime(sec, min, hour, day_week, day, month, year);
  
  //rtc.autoTime(); 

  // csv file header
  Serial.println("Hour,Minute,Second,Control_Therm(C),Black_Therm(C),White_Therm(C),Carbon_monoxide,Humidity,Temperature(C),Heat_Index(C)");
}

void loop() {
  rtc.update();

  // Read time:
  int second = rtc.second();
  int minute = rtc.minute();
  int hour = rtc.hour();

  // Temperature readings
  float therm_control = calculate_temperature(CONTROL_THERM_PIN);
  float therm_black = calculate_temperature(BLACK_THERM_PIN);
  float therm_white = calculate_temperature(WHITE_THERM_PIN);

  // Carbon monoxide readings
  int carbon_monoxide = analogRead(CARBON_MONOXIDE_PIN);

  // Humidity Temperature Sensor Readings
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Read temperature as Celcius
  float heat_index = dht.computeHeatIndex(temperature, humidity, false); // Compute heat index in Celcius

  // Serial print readings in csv format
  Serial.print(hour);
  Serial.print(",");
  Serial.print(minute);
  Serial.print(",");
  Serial.print(second);
  Serial.print(",");

  Serial.print(therm_control);
  Serial.print(",");
  Serial.print(therm_black);
  Serial.print(",");
  Serial.print(therm_white);
  Serial.print(",");

  Serial.print(carbon_monoxide);
  Serial.print(",");
  
  Serial.print(humidity);
  Serial.print(",");
  Serial.print(temperature);
  Serial.print(",");
  Serial.print(heat_index);

  Serial.println();

  delay(DELAY * SEC);
}

/*
Calculate temperature using Steinhart-Hart equation and coefficients
@param int analog pin being read from
returns float temperature
*/
float calculate_temperature(int pin) {
  float voltage_out = analogRead(pin);
  float resistance2 = RESISTANCE1 * (1023 / voltage_out - 1.00);
  float logr = log(resistance2);
  float temperature = (1.00 / (SH_A + SH_B*logr + SH_C*logr*logr*logr));
  temperature -= 273.15; // Convert to celcius
  return temperature;
}
