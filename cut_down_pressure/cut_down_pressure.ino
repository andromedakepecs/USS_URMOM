// @author Maya Juilan Kwong

const int RELAY = 13;
const int PRESSURE = A2;
bool cutDownTriggered = false; //acts as safeguard so that cutdown code isn't triggered more than once
int pressureSensorValue = 0;
int pressureSensorTotal = 0;
int pressureSensorAverage;
int time = 0;

const int BAUD_RATE = 9600; // Baud rate
const int SEC = 1000; // 1 SEC = 1000 ms

const int DELAY = 5; // Serial print every amount of seconds
const int THERM_PIN = 3; // Thermsistor is connected to this pin

const float R1 = 10000; // Value of resistor

int Vo;
float R2, temperature;

// Steinhart-Hart Coefficients
const float SH_A = 0.001125308852122;
const float SH_B = 0.000234711863267; 
const float SH_C = 0.000000085663516;

void setup() {
  // put your setup code here, to run once:
  pinMode(RELAY, OUTPUT);
  Serial.begin(BAUD_RATE);
}

void loop() {
  pressureSensorTotal = 0;
  temperature = 0;

  Vo = analogRead(THERM_PIN);
  R2 = R1 * (1023.0 / (float)Vo - 1.0); // TODO Changing / to * causes it to increase appropriately but reading is still off
  float logR2 = log(R2);

  // Calculate temperature using Steinhart-Hart equation and coefficients
  temperature = (1.0000 / (SH_A + SH_B*logR2 + SH_C*logR2*logR2*logR2));
  temperature -= 273.15; // Temperature celcius
  // temperature = (temperature * 9.0)/ 5.0 + 32.0; // Celcius to fahrenheit

  // Print temperature
  Serial.print("Temperature: "); 
  Serial.print(temperature);
  Serial.println(" C"); 

  delay(DELAY * SEC);

  for (int i = 0; i < 10; i++)
  {
    pressureSensorValue = analogRead(PRESSURE);
    time = time + 1;
    Serial.print("time = ");
    Serial.print(time);
    Serial.print("    Pressure = ");
    Serial.println(pressureSensorValue);
    pressureSensorTotal = pressureSensorTotal + pressureSensorValue;
  }
  pressureSensorAverage = pressureSensorTotal / 10;
  if (pressureSensorAverage < 218 && cutDownTriggered == false)
  // && temperature > -56
    {
      digitalWrite(RELAY, HIGH);
      delay(5000);
      delay(5000);
      digitalWrite(RELAY, LOW);
      cutDownTriggered = true; //program should end here
    }
}