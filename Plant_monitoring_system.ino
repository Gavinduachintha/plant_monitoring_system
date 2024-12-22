#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wi-Fi credentials
#define WIFI_SSID "your-SSID"
#define WIFI_PASSWORD "your-PASSWORD"

// Firebase project credentials
#define FIREBASE_HOST "your-project-id.firebaseio.com"
#define FIREBASE_AUTH "your-database-secret"

// Pin Definitions
#define DHTPIN D4         // DHT sensor pin
#define DHTTYPE DHT11     // DHT sensor type (DHT11 or DHT22)
#define SOIL_SENSOR_PIN A0 // Soil moisture sensor pin
#define RELAY_PIN D1      // Relay module pin
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin not used

// Thresholds
#define MOISTURE_THRESHOLD 400 // Adjust based on your soil sensor's readings

// Global objects
FirebaseData firebaseData;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Turn off relay initially (active LOW)
  
  dht.begin();

  // Initialize OLED display
  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println(F("OLED initialization failed"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWi-Fi connected");

  // Connect to Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Serial.println("Connected to Firebase");
}

void loop() {
  // Read sensor data
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int soilMoisture = analogRead(SOIL_SENSOR_PIN);

  // Check if readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor");
    return;
  }

  // Print readings
  Serial.print("Humidity: "); Serial.println(humidity);
  Serial.print("Temperature: "); Serial.println(temperature);
  Serial.print("Soil Moisture: "); Serial.println(soilMoisture);

  // Send data to Firebase
  if (Firebase.setFloat(firebaseData, "/humidity", humidity)) {
    Serial.println("Humidity sent to Firebase");
  } else {
    Serial.print("Firebase error: "); Serial.println(firebaseData.errorReason());
  }

  if (Firebase.setInt(firebaseData, "/soil_moisture", soilMoisture)) {
    Serial.println("Soil moisture sent to Firebase");
  } else {
    Serial.print("Firebase error: "); Serial.println(firebaseData.errorReason());
  }

  // Control relay based on soil moisture
  bool relayStatus;
  if (soilMoisture < MOISTURE_THRESHOLD) {
    digitalWrite(RELAY_PIN, LOW); // Activate relay (active LOW)
    relayStatus = true;
    Firebase.setBool(firebaseData, "/relay_status", true);
    Serial.println("Relay ON: Soil moisture low");
  } else {
    digitalWrite(RELAY_PIN, HIGH); // Deactivate relay
    relayStatus = false;
    Firebase.setBool(firebaseData, "/relay_status", false);
    Serial.println("Relay OFF: Soil moisture sufficient");
  }

  // Display values on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Humidity: ");
  display.print(humidity);
  display.println("%");

  display.print("Temp: ");
  display.print(temperature);
  display.println(" C");

  display.print("Soil Moist: ");
  display.print(soilMoisture);

  display.setCursor(0, 40);
  display.print("Relay: ");
  display.println(relayStatus ? "ON" : "OFF");

  display.display();

  // Delay for a while before next reading
  delay(2000);
}
