#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <SoftwareSerial.h>


#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define FIREBASE_HOST "iot-sensors-b0a63-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "Z892N8dhQA1Agzqhh6Er4AWfq4pebhelYZlwwdsE"

#define Relay D4
#define Relayhole D5

FirebaseData firebaseData1;
FirebaseData firebaseData2;
FirebaseData firebaseData3;
FirebaseData firebaseData4;
FirebaseData firebaseData5;
SoftwareSerial mySerial(D2, D3); // RX, TX

String myString;
int firstVal, secondVal, thirdVal, fourthVal, fifthVal, sixthVal;


void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(Relay, OUTPUT);
  pinMode(Relayhole, OUTPUT);
  digitalWrite(Relay, HIGH);
  digitalWrite(Relayhole, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  delay(200);
}

void loop() {
  // Read serial input
  if (mySerial.available()) {
    myString = mySerial.readStringUntil('\n');

    // Extract values
    firstVal = getValue(myString, ',', 0).toInt();
    secondVal = getValue(myString, ',', 1).toInt();
    thirdVal = getValue(myString, ',', 2).toInt();
    fourthVal = getValue(myString, ',', 3).toInt();
    fifthVal = getValue(myString, ',', 4).toInt();
    sixthVal = getValue(myString, ',', 5).toInt();

    // Print values
    Serial.print("First Value: ");
    Serial.println(firstVal);
    Serial.print("Second Value: ");
    Serial.println(secondVal);
    Serial.print("Third Value: ");
    Serial.println(thirdVal);
    Serial.print("Fourth Value: ");
    Serial.println(fourthVal);
    Serial.print("Fifth Value: ");
    Serial.println(fifthVal);
    Serial.print("Sixth Value: ");
    Serial.println(sixthVal);

    //-------------------------MQ-6-------------------------------------------
    if (fourthVal < 200) {
      Firebase.setString(firebaseData1, "/MQ6", "Normal");
    } else if ((fourthVal > 200) && (fourthVal < 5000)) {
      Firebase.setString(firebaseData1, "/MQ6", "Warning");
    } else if (fourthVal > 5000) {
      Firebase.setString(firebaseData1, "/MQ6", "Danger");
    }

    //-------------------------MQ-135----------------------------------------------
    if (fifthVal < 800) {
      Firebase.setString(firebaseData1, "/MQ135", "Normal");
    } else if ((fifthVal > 800) && (fifthVal < 1200)) {
      Firebase.setString(firebaseData1, "/MQ135", "Warning");
    } else if (fifthVal > 1200) {
      Firebase.setString(firebaseData1, "/MQ135", "Danger");
    }

    //--------------------------MQ-9-------------------------------------------------
    if (sixthVal < 9) {
      Firebase.setString(firebaseData1, "/MQ9", "Normal");
    } else if ((sixthVal > 10) && (sixthVal < 34)) {
      Firebase.setString(firebaseData1, "/MQ9", "Warning");
    } else if (sixthVal > 35) {
      Firebase.setString(firebaseData1, "/MQ9", "Danger");
    }

    //------------------------Other sensor data---------------------------------------
    Firebase.setInt(firebaseData1, "/humidity", firstVal);
    Firebase.setInt(firebaseData1, "/Temperature", secondVal);
    Firebase.setInt(firebaseData1, "/Lm35", thirdVal);
    //     Firebase.setInt(firebaseData1, "/Gas Lekage", fourthVal);
    //     Firebase.setInt(firebaseData1, "/co2_ppm", fifthVal);
    //     Firebase.setInt(firebaseData1, "/CO Concentration:", sixthVal);
  }

  if (Firebase.getBool(firebaseData1, "/manual_control")) {
    bool Value = firebaseData1.boolData();
    if (Value == true) {
      if (Firebase.getBool(firebaseData1, "/fan")) {
        bool Value1 = firebaseData1.boolData();
        if (Value1 == true) {
          digitalWrite(Relay, LOW);
          Serial.println("fan Relay is ON");
        }
        if (Value1 == false) {
          digitalWrite(Relay, HIGH);
            Serial.println("fan Relay is Off");
        }
      }
      if (Firebase.getBool(firebaseData1, "/curtain")) {
        bool Value2 = firebaseData1.boolData();
        if (Value2 == true) {
          digitalWrite(Relayhole, LOW);
          Serial.println("Hole Relay is ON");
        }
        if (Value2 == false) {
          digitalWrite(Relayhole, HIGH);
          Serial.println("Hole Relay is OFF");
        }
      }
    }
    if (Value == false) {
      Firebase.getString(firebaseData3, "/MQ6");
      Firebase.getString(firebaseData4, "/MQ135");
      Firebase.getString(firebaseData5, "/MQ9");
      String  mq6Status = firebaseData3.stringData();
      String  mq135Status = firebaseData4.stringData();
      String  mq9Status = firebaseData5.stringData();
      if (mq6Status == "Danger" && mq135Status == "Danger" && mq9Status == "Danger") {
        digitalWrite(Relay, LOW);
        digitalWrite(Relayhole, LOW);
        Serial.println("dangerzone");
      }
      if (mq6Status == "Normal" && mq135Status == "Normal" && mq9Status == "Normal") {
        digitalWrite(Relay, HIGH);
        digitalWrite(Relayhole, HIGH);
        Serial.println("Normal Zone");
      }
    }
  }
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
