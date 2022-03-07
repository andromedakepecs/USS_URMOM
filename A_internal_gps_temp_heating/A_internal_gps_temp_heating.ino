/*
USS Urmom: Arduino A
Internal sensing
  Monitors internal temperature using 10k ohm thermistor in series with 10k ohm resistor
  Triggers heating pads at cold temperatures
  Read and print gps data using Sparkfun SAM-M8Q
    - //http://librarymanager/All#SparkFun_u-blox_GNSS 
  Connects to RTC module DS3234
    - https://github.com/sparkfun/SparkFun_DS3234_RTC_Arduino_Library 

@author Andromeda Kepecs
*/

#include <SparkFunDS3234RTC.h>
#include <SPI.h>

#include <Wire.h> //Needed for I2C to GNSS

#include <SparkFun_u-blox_GNSS_Arduino_Library.h> 
SFE_UBLOX_GNSS myGNSS;

// Digital output pins
#define HEAT_PAD_PIN 5
#define RTC_PIN 10

// Analog input pins
#define THERM_PIN 0

// Temps to trigger heating pad in Celcius
const int MIN_TEMP = 10;
const int MAX_TEMP = 15;

// Steinhart-Hart Coefficients for ~10k ohm thermistor
const float SH_A = 0.001125308852122;
const float SH_B = 0.000234711863267; 
const float SH_C = 0.000000085663516;

// Value of resistor used in series with thermistor
const float RESISTANCE1 = 10000;

// Timing
const int DELAY = 5; // How many seconds the sensor takes a reading, TODO adjust
const int SEC = 1000; // 1 sec = 1000 ms
const int BAUD_RATE = 9600;

void setup() {
  rtc.begin(RTC_PIN);
  Serial.begin(BAUD_RATE);

  // RTC has been configured, behind by approx 10 sec
  //rtc.setTime(0, 12, 18, 1, 6, 3, 22);
  //rtc.autoTime(); 

  pinMode(HEAT_PAD_PIN, OUTPUT);

  Wire.begin();

  if (myGNSS.begin() == false) // Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); // Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save (only) the communications port settings to flash and BBR

  // csv file header
  Serial.println("Hour,Minute,Second,Temperature(C),Heatpad_on?,Latitude,Longitude,Altitude");
}

void loop() {
  rtc.update(); // Update time

  // Read time:
  int second = rtc.second();
  int minute = rtc.minute();
  int hour = rtc.hour();

  // Read GPS data
  // This module only responds when a new position is available?
  long latitude = myGNSS.getLatitude();
  long longitude = myGNSS.getLongitude();
  long altitude = myGNSS.getAltitude();

  // Read temperature
  int temperature = calculate_temperature(THERM_PIN);

  // Trigger heating pad if below min temperature
  bool heatpad_on = false;
  if (temperature < MIN_TEMP) {
    digitalWrite(HEAT_PAD_PIN, HIGH);
    heatpad_on = true;
  }
  else if (temperature > MAX_TEMP) {
    digitalWrite(HEAT_PAD_PIN, LOW);
    heatpad_on = false;
  }

  // Print all data in csv format
  Serial.print(hour);
  Serial.print(",");
  Serial.print(minute);
  Serial.print(",");
  Serial.print(second);
  Serial.print(",");

  Serial.print(temperature);
  Serial.print(",");
  Serial.print(heatpad_on);
  Serial.print(",");
  
  Serial.print(latitude);
  Serial.print(",");
  Serial.print(longitude);
  Serial.print(",");
  Serial.print(altitude);

  Serial.println();

  delay(DELAY * SEC); // Delay in seconds between updates
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
  int temperature = int(1.00 / (SH_A + SH_B*logr + SH_C*logr*logr*logr));
  temperature -= 273.15; // Convert to celcius
  return temperature;
}
