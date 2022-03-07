#include <SparkFunDS3234RTC.h>
#include <SPI.h>

#define DS13074_CS_PIN 10

#include <Wire.h> //Needed for I2C to GNSS

#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;

/*
Arduino A
Internal sensing
Monitors internal temperature to trigger heating pads (9600 baud ok),
Read and print gps and pressure data (baud??)
Sync with real time clock (9600 baud)

By USS Urmom
@author Andromeda Kepecs
*/

// Digital output pins
const int HEAT_PAD = 10;
const int RTC = 10;
// RTC and gps use additional pins

// Analog input pins
const int THERM_PIN = 0; // Connect other end of thermistor to 5V

// Temps to trigger heating pad in Celcius
const int MIN_TEMP = 10;
const int MAX_TEMP = 15;

// Steinhart-Hart Coefficients for ~10k ohm thermistor
const float SH_A = 0.001125308852122;
const float SH_B = 0.000234711863267; 
const float SH_C = 0.000000085663516;

// Value of resistor used in series with thermistor
const float R1 = 10000;

// Timing
const int DELAY = 5; // How many seconds the sensor takes a reading
const int SEC = 1000; // 1 sec = 1000 ms
const int BAUD_RATE = 9600;

void setup() {
  rtc.begin(RTC);
  Serial.begin(BAUD_RATE);

  // RTC has been configured, behind by approx 10 sec
  //rtc.setTime(0, 12, 18, 1, 6, 3, 22);
  //rtc.autoTime(); 

  pinMode(HEAT_PAD, OUTPUT);

  //Wire.begin();

  /*if (myGNSS.begin() == false) // Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); // Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save (only) the communications port settings to flash and BBR
  */

  // csv file headers
  Serial.println("Hour,Minute,Second,Temperature(C),Heatpad_on?,Latitude,Longitude,Altitude");
}

void loop() {
  rtc.update(); // Update time

  // Read time:
  int second = rtc.second();
  int minute = rtc.minute();
  int hour = rtc.hour();

  // Read GPS data
  // Query module only every second. Doing it more often will just cause I2C traffic.
  // The module only responds when a new position is available
  /*long latitude = myGNSS.getLatitude();
  long longitude = myGNSS.getLongitude();
  long altitude = myGNSS.getAltitude();
  byte SIV = myGNSS.getSIV();*/

  // Read temperature
  int Vo = analogRead(THERM_PIN); // Read in voltage from thermistor pin
  float R2 = calculate_resistance(Vo, R1);
  int temperature = int(calculate_temperature(R2));  

  // Trigger heating pad if below min temperature
  bool heatpad_on = false;
  if (temperature < MIN_TEMP) {
    digitalWrite(HEAT_PAD, HIGH);
    heatpad_on = true;
  }
  else if (temperature > MAX_TEMP) {
    digitalWrite(HEAT_PAD, LOW);
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
  Serial.println();
  /*
  Serial.print(latitude);
  Serial.print(",");
  Serial.print(longitude);
  Serial.print(",");
  Serial.print(altitude);*/

  delay(DELAY * SEC); // Delay in seconds between updates
}

/* Calculate resistance of thermistor from voltage reading
@param int v, voltage reading
@param float r1, value of resistor in series
returns float r2
*/
float calculate_resistance(int v, float r1) {
  float r2 = r1 * (1023.0 / float(v) - 1.0);
  return r2;
}

/* Calculate temperature using Steinhart-Hart equation and coefficients for a 10k ohm resistor
@param float r, resistance of thermistor
returns float temperature in degrees celcius
*/
float calculate_temperature(float r) {
  float logr = log(r);
  float temperature = (1.00 / (SH_A + SH_B*logr + SH_C*logr*logr*logr)); // Temperature in Kelvin
  temperature -= 273.15; // Temperature in celcius
  return temperature;
}
