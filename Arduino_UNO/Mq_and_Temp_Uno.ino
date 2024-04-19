#include <MQ135.h>
#include "DHT.h"
#include <SoftwareSerial.h>
SoftwareSerial Arduino(2, 5);//RX tx
#include <Wire.h>
const int gasPin = A1; // Pin number of the gas sensor
int pin = A0;
int sensorValue;
MQ135 gasSensor = MQ135(pin);
float rzero = gasSensor.getRZero();
int ppm = gasSensor.getPPM();
float data;
float sdata1 = 0; // temperature data
float sdata2 = 0; // MQ135 data
float sdata3 = 0; // MQ6 data
float sdata4 = 0; // MQ9 data
float sdata5 = 0; // Temp data
float sdata6 = 0; // Humidity data
String cdata; // complete data
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  Serial.begin(9600);
  Arduino.begin(9600);
  dht.begin();
}

void loop() {
  //---------------------------------------------------------Temperature_And_Humidity------------------------------
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F("Humidity: "));
  Serial.println(h);
  Serial.print(F("%  Temperature: "));
  Serial.println(t);
  //  //__________________________________________mq_9___________________________________________
  float gasValue = analogRead(A1);
  Serial.print("CO Concentration: ");
  Serial.print(gasValue);
  Serial.println(" ppm");
  delay(1000); // Delay for stability
  ////-------------------------------------------mQ135--------------------------------------------
  sensorValue = analogRead(pin);
  Serial.print("MQ");
  Serial.println(sensorValue);
  float co2_ppm = gasSensor.getPPM();
  int ppm = co2_ppm / 4;
  Serial.print(" co2 ppm=");
  Serial.println(co2_ppm);
  delay(1000);
  //------------------------------------------MQ_6---------------------------------------------
  int initialMQ6Value = 215; // Initial value when no gas is detected
  int detectedMQ6Value = analogRead(A2); // Read the current value from sensor

  // Map the sensor readings from the range 219-500 to 0-100 percentage scale
  int percentage = map(detectedMQ6Value, initialMQ6Value, 500, 0, 100);
  if (percentage <= 0)
  {
    percentage = 0;
  }
  // Print the percentage
  Serial.println( detectedMQ6Value);
  Serial.print("Percentage of LPG gas: ");
  Serial.print(percentage);
  Serial.println("%");
  //----------------------------------------------------LM_35--------------------------------------------
  const int lm35_pin = A4;  /* LM35 O/P pin */
  int temp_adc_val;
  float temp_val;
  temp_adc_val = analogRead(lm35_pin);  /* Read Temperature */
  temp_val = (temp_adc_val * 4.88); /* Convert adc value to equivalent voltage */
  temp_val = (temp_val / 10); /* LM35 gives output of 10mv/°C */
  Serial.print("Temperature = ");
  Serial.println(temp_val);
  ////____________________________________________________________________________________________________
  ////----------------------------Data send to Node------------------------------
  sdata1 = h; //humidity
  sdata2 = t; //Temp
  sdata3 = temp_val; //Lm35 value
  sdata4 = percentage; //Gass Lekage value MQ 6
  sdata5 = co2_ppm; // mQ 135
  sdata6 = ppm  ; //MQ 9
  // sdata6=


  Serial.println("im in loop");
  cdata = cdata + sdata1 + "," + sdata2 + "," + sdata3 + "," + sdata4 + "," + sdata5 + "," +  sdata6;
  Serial.println(cdata);
  Arduino.println(cdata);
  cdata = " ";

  delay(1000);
  delay(500);
}
