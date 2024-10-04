#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Adafruit_GPS.h>

// Define pins for ultrasonic sensor
const int trigPin = 9;
const int echoPin = 10;

// GSM and GPS module pins
SoftwareSerial gsm(7, 8); // RX, TX
SoftwareSerial gpsSerial(4, 3); // RX, TX

// Create instances for the GPS and LCD
Adafruit_GPS GPS(&gpsSerial);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // Initialize the LCD
  lcd.begin(16, 2);
  lcd.print("Initializing...");

  // Setup pins for ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize GSM module
  gsm.begin(9600);
  delay(1000);
  gsm.println("AT+CMGF=1"); // Set GSM to text mode
  delay(1000);

  // Initialize GPS module
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  
  lcd.clear();
  lcd.print("System Ready");
}

void loop() {
  long duration;
  int distance;

  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the distance from the sensor
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Display the distance on the LCD
  lcd.setCursor(0, 1);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print(" cm");

  // If a crack is detected (e.g., distance exceeds threshold)
  if (distance > 50) { // Adjust the threshold as necessary
    lcd.clear();
    lcd.print("Crack Detected!");

    // Get GPS coordinates
    GPS.read();
    if (GPS.fix) {
      String latitude = String(GPS.latitudeDegrees, 6);
      String longitude = String(GPS.longitudeDegrees, 6);

      // Send SMS with location details
      String message = "Crack detected at Lat: " + latitude + " Lon: " + longitude;
      sendSMS(message);
    } else {
      lcd.setCursor(0, 1);
      lcd.print("GPS No Fix");
    }
    
    delay(5000); // Wait for 5 seconds before next reading
  }
}

void sendSMS(String message) {
  gsm.print("AT+CMGS=\"+1234567890\"\r"); // Replace with recipient's phone number
  delay(1000);
  gsm.print(message);
  delay(1000);
  gsm.write(26); // ASCII code for CTRL+Z to send the message
  delay(1000);
}
