#include <WiFi.h>
#include <FirebaseESP32.h>

const char* ssid = "TP-Link_88C8";
const char* password = "11797869";

FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;
FirebaseData firebaseData;

const int buzzerPin = 23;
const int ldrDigitalPin = 14;
#define TRIG_PIN 2
#define ECHO_PIN 4
const int led1Pin = 5;
const int led2Pin = 18;
// bool sirenEnabled = false;
bool sensorsEnabled = false;
float lastDistance = 0;
// bool sosEnabled = false;

// void sosSignal() {
//   for (int i = 0; i < 3; i++) {
//     digitalWrite(led1Pin, HIGH);
//     digitalWrite(led2Pin, HIGH);
//     delay(100);
//     digitalWrite(led1Pin, LOW);
//     digitalWrite(led2Pin, LOW);
//     delay(100);
//   }
//   delay(200);

//   for (int i = 0; i < 3; i++) {
//     digitalWrite(led1Pin, HIGH);
//     digitalWrite(led2Pin, HIGH);
//     delay(500);
//     digitalWrite(led1Pin, LOW);
//     digitalWrite(led2Pin, LOW);
//     delay(500);
//   }
//   delay(200);

//   for (int i = 0; i < 3; i++) {
//     digitalWrite(led1Pin, HIGH);
//     digitalWrite(led2Pin, HIGH);
//     delay(100);
//     digitalWrite(led1Pin, LOW);
//     digitalWrite(led2Pin, LOW);
//     delay(100);
//   }
//   delay(500);
// }

// void policeSiren() {
//   digitalWrite(led1Pin, HIGH);
//   digitalWrite(led2Pin, LOW);
//   delay(250);
//   digitalWrite(led1Pin, LOW);
//   digitalWrite(led2Pin, HIGH);
//   delay(250); 
// }

void setup() {
  Serial.begin(115200);

  pinMode(buzzerPin, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ldrDigitalPin, INPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  firebaseConfig.host = "https://esp32-security-67b5b-default-rtdb.europe-west1.firebasedatabase.app";
  firebaseConfig.api_key = "AIzaSyCgtEy05JPROdpvGLe0hfQu_Z5_FZUxSxI";
  firebaseAuth.user.email = "wonidw@gmail.com";
  firebaseAuth.user.password = "iimmaaDD120";

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  if (Firebase.ready()) {
    Serial.println("Firebase connected.");
  } else {
    Serial.println("Firebase initialization failed.");
  }

  if (Firebase.setString(firebaseData, "/sensor/state", "off")) {
    Serial.println("Default state set to 'off'.");
  } else {
    Serial.print("Failed to set default state: ");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/sensor/data/distance", -1)) {
    Serial.println("Initial distance set.");
  } else {
    Serial.print("Failed to set initial distance: ");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.setString(firebaseData, "/sensor/data/ldr", "Dark")) {
    Serial.println("Initial LDR state set.");
  } else {
    Serial.print("Failed to set initial LDR state: ");
    Serial.println(firebaseData.errorReason());
  }

  // if (Firebase.getBool(firebaseData, "/sos")) {
  //   sosEnabled = firebaseData.boolData();
  //   Serial.print("SOS Enabled (Initial): ");
  //   Serial.println(sosEnabled);
  // } else {
  //   Serial.print("Failed to get initial SOS state: ");
  //   Serial.println(firebaseData.errorReason());
  // }

  //  if (Firebase.getBool(firebaseData, "/siren")) { 
  //   sirenEnabled = firebaseData.boolData();
  //   Serial.print("Siren Enabled (Initial): ");
  //   Serial.println(sirenEnabled);
  // } else {
  //   Serial.print("Failed to get initial siren state: ");
  //   Serial.println(firebaseData.errorReason());
  // }

}


void measureDistance(float &distance) {
  long duration;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) {
    distance = lastDistance;
    Serial.println("Distance measurement timeout.");
  } else {
    distance = (duration / 2.0) * 0.0343;
    lastDistance = distance;
  }
}

void loop() {
  if (Firebase.ready()) {
    int ldrValue = digitalRead(ldrDigitalPin);

    if (Firebase.getString(firebaseData, "/sensor/state")) {
      String sensorState = firebaseData.stringData();
      sensorsEnabled = (sensorState == "on");
      Serial.print("Sensor state: ");
      Serial.println(sensorState);
    } else {
      Serial.print("Failed to get sensor state: ");
      Serial.println(firebaseData.errorReason());
    }

    // if (Firebase.getBool(firebaseData, "/sos")) {
    //   sosEnabled = firebaseData.boolData();
    //   Serial.print("SOS Enabled (from Firebase): ");
    //   Serial.println(sosEnabled);
    // } else {
    //   Serial.print("Failed to get SOS state: ");
    //   Serial.println(firebaseData.errorReason());
    // }

    // if (Firebase.getBool(firebaseData, "/siren")) { 
    //   sirenEnabled = firebaseData.boolData();
    //   Serial.print("Siren Enabled (from Firebase): ");
    //   Serial.println(sirenEnabled);
    // } else {
    //   Serial.print("Failed to get siren state: ");
    //   Serial.println(firebaseData.errorReason());
    // }

    if (sensorsEnabled) {
      Serial.println("Sensors are enabled.");

      float distance;
      measureDistance(distance);
      Serial.print("Measured distance: ");
      Serial.println(distance);

      if (Firebase.setFloat(firebaseData, "/sensor/data/distance", distance)) {
        Serial.println("Distance updated in Firebase.");
      } else {
        Serial.print("Failed to update distance: ");
        Serial.println(firebaseData.errorReason());
      }

      String ldrState = ldrValue == HIGH ? "Dark" : "Bright";
      if (Firebase.setString(firebaseData, "/sensor/data/ldr", ldrState)) {
        Serial.println("LDR state updated in Firebase.");
      } else {
        Serial.print("Failed to update LDR state: ");
        Serial.println(firebaseData.errorReason());
      }

      Serial.print("LDR Value: ");
      Serial.println(ldrValue);

      if (ldrValue == LOW) {  
        tone(buzzerPin, 450); 
        digitalWrite(led1Pin, HIGH);
        digitalWrite(led2Pin, HIGH);
        // if (sosEnabled) { 
        //   sosSignal();
        // } else {
        //   digitalWrite(led1Pin, LOW); 
        //   digitalWrite(led2Pin, LOW);
        // }

      //   if (sirenEnabled) { // Check the sirenEnabled boolean
      //   Serial.println("Siren is enabled. Activating police siren.");
      //   policeSiren();
      // } else {
      //   digitalWrite(led1Pin, LOW);
      //   digitalWrite(led2Pin, LOW);
      // }

      } else {
        noTone(buzzerPin);
        digitalWrite(led1Pin, LOW);
        digitalWrite(led2Pin, LOW);
      }

    } else {
      Serial.println("Sensors are disabled.");
      noTone(buzzerPin);
      digitalWrite(led1Pin, LOW);
      digitalWrite(led2Pin, LOW);
    }
  } else {
    Serial.println("Firebase is not ready.");
  }

  delay(1000);
}