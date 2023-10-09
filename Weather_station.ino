#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>
#include <Wire.h>

// Define your Firebase credentials and Wi-Fi settings
#define FIREBASE_HOST "your-firebase-hostname.firebaseio.com"
#define FIREBASE_AUTH "your-firebase-authentication"

// Initialize Firebase
FirebaseData firebaseData;

// Initialize DHT sensor
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Initialize I2C Pressure Sensor (Add your specific library and setup here)

// Define encoder pins and variables
const int encoderPinA = 2;
const int encoderPinB = 3;
volatile int encoderPos = 0;
volatile int lastEncoded = 0;
volatile int lastMSB = 0;
volatile int lastLSB = 0;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin("your-ssid", "your-password");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, firebaseData);

  // Initialize DHT sensor
  dht.begin();

  // Initialize encoder pins
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), readEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), readEncoder, CHANGE);
}

void loop() {
  // Read data from sensors
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  // Read pressure from the pressure sensor (add your code here)

  // Upload data to Firebase
  Firebase.setFloat(firebaseData, "Temperature", temperature);
  Firebase.setFloat(firebaseData, "Humidity", humidity);
  // Upload pressure data to Firebase (add your code here)

  // Print data to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Wait for a while before updating data
  delay(60000); // 1 minute delay
}

void readEncoder() {
  int MSB = digitalRead(encoderPinA);
  int LSB = digitalRead(encoderPinB);
  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;
  
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    encoderPos++;
  }
  else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    encoderPos--;
  }
  
  lastEncoded = encoded;
}
