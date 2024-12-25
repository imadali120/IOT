#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <FirebaseESP32.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const char* ssid = "Kokuzna Vremena";
const char* password = "necutireci";


FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;
FirebaseData firebaseData;


const int buzzerPin = 23;
const int ldrDigitalPin = 14;
#define TRIG_PIN 2
#define ECHO_PIN 4


bool sensorsEnabled = false;
float lastDistance = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin(18, 19);


    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
        Serial.println(F("OLED initialization failed"));
        while (true);
    }
    display.clearDisplay();
    display.display();


    pinMode(buzzerPin, OUTPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(ldrDigitalPin, INPUT);


    Serial.print("Connecting to Wi-Fi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());


    firebaseConfig.host = "https://esp32-security-67b5b-default-rtdb.europe-west1.firebasedatabase.app"; // Replace with your Firebase host
    firebaseConfig.api_key = "AIzaSyCgtEy05JPROdpvGLe0hfQu_Z5_FZUxSxI"; // Replace with your Firebase API key
    firebaseAuth.user.email = "wonidw@gmail.com"; // Replace with your Firebase email
    firebaseAuth.user.password = "iimmaaDD120"; // Replace with your Firebase password


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


            digitalWrite(buzzerPin, ldrValue == LOW ? HIGH : LOW);


            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);

            display.setCursor(0, 10);
            display.print("LDR:");
            display.println(ldrValue == HIGH ? "Dark" : "Bright");

            display.setCursor(0, 30);
            display.print("DST:");
            if (distance == -1) {
                display.print("N/A");
            } else {
                display.print(distance);
                display.println(" cm");
            }

            display.display();
        } else {
            Serial.println("Sensors are disabled.");


            digitalWrite(buzzerPin, LOW);
            display.clearDisplay();
            display.setCursor(0, 20);
            display.setTextSize(1);
            display.print("Sensors are OFF");
            display.display();
        }
    } else {
        Serial.println("Firebase is not ready.");
    }


    delay(1000);
}