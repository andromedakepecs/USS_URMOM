#include <SparkFunDS1307RTC.h>
#include <Wire.h>
#include <Wire.h> //Needed for I2C to GNSS

#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;

/*
Arduino A
Internal sensing
Monitors internal temperature to trigger heating pads (9600 baud ok),
Read and print gps and pressure data (baud??)
Sync with real time clock (9600 baud)
TODO Pressure code

By USS Urmom
@author Andromeda Kepecs
*/

// Digital output pins
const int HEAT_PAD = 10;
// RTC uses arbitrary digital pin

// Analog input pins
const int THERM_PIN = 0;

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
const int DELAY = 10; // How many seconds the sensor takes a reading
const int SEC = 1000; // 1 sec = 1000 ms
const int BAUD_RATE = 9600;

void setup() {
  rtc.begin();
  rtc.autotime(); // May need to be manually changed to launch time

  pinMode(HEAT_PAD, OUTPUT);

  Serial.begin(BAUD_RATE);

  Wire.begin();

  if (myGNSS.begin() == false) // Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); // Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save (only) the communications port settings to flash and BBR
}

void loop() {
  rtc.update(); // Update time

  // Read time:
  int second = rtc.second();
  int minute = rtc.minute();
  int hour = rtc.hour()

  // Read GPS data
  // Query module only every second. Doing it more often will just cause I2C traffic.
  // The module only responds when a new position is available
  long latitude = myGNSS.getLatitude();
  long longitude = myGNSS.getLongitude();
  long altitude = myGNSS.getAltitude();
  byte SIV = myGNSS.getSIV();

  // Read temperature
  int Vo = analogRead(THERM_PIN); // Read in voltage from thermistor pin
  float R2 = calculate_resistance(Vo, R1);
  int temperature = int(calculate_temperature(R2));  

  // Trigger heating pad if below min temperature
  heatpad_on = false
  if (temperature < MIN_TEMP) {
    digitalWrite(HEAT_PAD, HIGH);
    heatpad_on = true
  }
  else if (temperature > MAX_TEMP) {
    digitalWrite(HEAT_PAD, LOW);
    heatpad_on = false
  }

  // Print all data in csv format
  print("Time,Temperature(C),Heatpad_on?,Latitude,Longitude,Altitude")

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
