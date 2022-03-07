int analogPin = A0;     // potentiometer wiper (middle terminal) connected to analog pin 3
int analogPin2 = A1;
int analogPin3 = A2;
int analogPin4 = A3;


const int led = 13;
int time = 0;
int val = 0;           // variable to store the value read
int val2 = 0;
int val3 = 0;
int val4 = 0;

void setup()

{

  Serial.begin(9600);          //  setup serial

}

void loop()

{
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);               // wait for a half second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(500);               // wait for a half second
  time = time + 1;
  Serial.print("time = ");
  Serial.println(time);
  val = analogRead(A0);    // read the input pin
  //Serial.println(val);   
  Serial.println("voltage1");         // debug value
  float voltage= val * (5.0 / 1023.0);
 // Vin = voltage/(0.333333);
  Serial.println(voltage);             // debug value

  val2 = analogRead(A1);    // read the input pin
  //Serial.println(valbb2); 
  Serial.println("voltage2");            // debug value
  float voltage2= val2 * (5.0 / 1023.0);
  Serial.println(voltage2);             // debug value

  val3 = analogRead(A2);    // read the input pin
  //Serial.println(val3); 
  Serial.println("voltage3");            // debug value
  float voltage3= val3 * (5.0 / 1023.0);
  Serial.println(voltage3);             // debug value

  val4 = analogRead(A3);    // read the input pin
  //Serial.println(val3); 
  Serial.println("voltage4");            // debug value
  float voltage4= val4 * (5.0 / 1023.0);
  Serial.println(voltage4);             // debug value

  delay(1000);
}