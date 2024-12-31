//This code is for the arduino nano
#include "SPI.h"
#include "Adafruit_GC9A01A.h"
#include "DHT.h"

// Define pins for TFT display
#define TFT_CS    10
#define TFT_DC    9
#define TFT_RST   8  // Reset pin (optional, but needed in some cases)

// Initialize TFT display with the appropriate pins
Adafruit_GC9A01A tft = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);

// Define sensor pins
#define MOISTURE_PIN A0
#define DHT_PIN 2    // Digital pin connected to the DHT sensor
#define DHT_TYPE DHT11 // Change to DHT22 if you're using DHT22

// Initialize the DHT sensor
DHT dht(DHT_PIN, DHT_TYPE);

// Define center and radius of the progress circle
#define CENTER_X 120
#define CENTER_Y 120
#define RADIUS   120

// Variables to store the current and previous moisture levels
int prevMoistureLevel = -1;  // Initially set to an invalid value

// Function prototypes
void drawHumidityBar(int humidityLevel);
void updateProgressBar(int oldLevel, int newLevel);

void setup() {
  // Initialize TFT display
  tft.begin();
  tft.setRotation(-1);            // Set orientation
  tft.fillScreen(GC9A01A_BLACK); // Set background to black

  // Initialize the DHT sensor
  dht.begin();
}

void loop() {
  // Read the soil moisture level
  int sensorValue = analogRead(MOISTURE_PIN);
  int moisturePercent = map(sensorValue, 0, 1023, 100, 0);  // Map to percentage (100% = wet, 0% = dry)

  // Read the humidity value from the sensor
  float humidityValue = dht.readHumidity();

  // Check if the humidity value is valid
  if (isnan(humidityValue)) {
    humidityValue = -1;  // Set to -1 if the reading fails
  }

  // Update the humidity bar
  drawHumidityBar((int)humidityValue);

  // Only update the progress bar if the moisture level has changed significantly
  if (moisturePercent != prevMoistureLevel) {
    updateProgressBar(prevMoistureLevel, moisturePercent);
    prevMoistureLevel = moisturePercent;
  }

  // Delay to refresh the display (update interval)
  delay(100);  // Update every 100ms (adjust as necessary)
}

// Function to draw the humidity bar inside the moisture bar
void drawHumidityBar(int humidityLevel) {
  if (humidityLevel < 0 || humidityLevel > 100) {
    return; // Skip invalid humidity values
  }

  int humidityAngle = map(humidityLevel, 0, 100, 3600, 0);
  float step = 2.0; // Smaller increments for smoother display

  for (float angle = 3600; angle > humidityAngle; angle -= step) {
    int x1 = CENTER_X + (RADIUS - 30) * cos(radians(angle / 10.0));  // Inner radius for humidity
    int y1 = CENTER_Y + (RADIUS - 30) * sin(radians(angle / 10.0));
    int x2 = CENTER_X + (RADIUS - 40) * cos(radians(angle / 10.0));  // Smaller inner radius
    int y2 = CENTER_Y + (RADIUS - 40) * sin(radians(angle / 10.0));

    // Draw the humidity bar segments
    tft.drawLine(x1, y1, x2, y2, GC9A01A_BLUE);
  }

  // Display the numerical humidity value at the center
  tft.fillCircle(CENTER_X, CENTER_Y, 25, GC9A01A_BLACK);  // Clear the center
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(2);

  tft.setCursor(CENTER_X - 30, CENTER_Y - 10);
  tft.print(humidityLevel);
  tft.print("%");

  // Draw the word "Humidity" below the percentage
  tft.setTextSize(1);
  tft.setCursor(CENTER_X - 30, CENTER_Y + 10);
  tft.print("Humidity");
}

// Function to update the moisture progress bar with smooth transitions
void updateProgressBar(int oldLevel, int newLevel) {
  int oldAngle = map(oldLevel, 0, 100, 3600, 0); // Reversed mapping
  int newAngle = map(newLevel, 0, 100, 3600, 0); // Reversed mapping

  int reversedPercentage = 100 - newLevel; // Reverse the percentage for display

  float step = 2.0; // Smaller increments for smoother animation

  // If the new angle is greater, draw the newly added segments
  if (oldAngle < newAngle) {
    for (float angle = oldAngle; angle < newAngle; angle += step) {
      int x1 = CENTER_X + RADIUS * cos(radians(angle / 10.0));
      int y1 = CENTER_Y + RADIUS * sin(radians(angle / 10.0));
      int x2 = CENTER_X + (RADIUS - 10) * cos(radians(angle / 10.0));
      int y2 = CENTER_Y + (RADIUS - 10) * sin(radians(angle / 10.0));

      // Draw small segments for smooth transition
      tft.drawLine(x1, y1, x2, y2, GC9A01A_GREEN);
    }
  }
  // If the new angle is smaller, erase the previous segments counterclockwise
  else if (oldAngle > newAngle) {
    for (float angle = oldAngle; angle > newAngle; angle -= step) {
      int x1 = CENTER_X + RADIUS * cos(radians(angle / 10.0));
      int y1 = CENTER_Y + RADIUS * sin(radians(angle / 10.0));
      int x2 = CENTER_X + (RADIUS - 10) * cos(radians(angle / 10.0));
      int y2 = CENTER_Y + (RADIUS - 10) * sin(radians(angle / 10.0));

      // Use black to erase the previous segments
      tft.drawLine(x1, y1, x2, y2, GC9A01A_BLACK);
    }
  }

  // Clear the center area for updated text
  tft.fillCircle(CENTER_X, CENTER_Y, 30, GC9A01A_BLACK);

  // Draw the reversed numerical value at the center
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(3);

  // Centering the text dynamically based on the number of digits
  int textWidth = reversedPercentage < 10 ? 6 : (reversedPercentage < 100 ? 12 : 18);
  tft.setCursor(CENTER_X - textWidth, CENTER_Y - 9);
  tft.print(reversedPercentage);
  tft.print("%");

  // Draw the word "Moisture" below the percentage
  tft.setTextSize(2);
  tft.setCursor(CENTER_X - 40, CENTER_Y + 20);
  tft.print("Moisture");
}
