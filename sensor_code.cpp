#include "DHT.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DHTpin 4 // D4 on the ESP32
#define DHTTYPE DHT11
DHT dht(DHTpin, DHTTYPE);
const char* ssid = "captain";
const char* password = "12345678";

const char* server = "api.thingspeak.com";
const String apiKey = "Your api key ";

int sensorPin = 34; // Analog pin connected to the air quality sensor
int ldrPin = 13;    // GPIO13 connected to the DO pin of the LDR module
#define POWER_PIN  16 // GPIO17 connected to the water sensor's VCC pin
#define SIGNAL_PIN 26 // GPIO36 (ADC0) connected to the water sensor's signal pin
#define ONE_WIRE_BUS 2 // GPIO2 connected to the Dallas Temperature sensor's data pin

OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); // Pass the oneWire reference to Dallas Temperature

// Function to convert MQ135 analog reading to CO2 ppm
float MQ135toCO2(int rawValue) {
  // Assuming a linear calibration curve: CO2_ppm = m * rawValue + b
  float m = 0.1; // Slope of the calibration curve (replace with your value)
  float b = 100; // Y-intercept of the calibration curve (replace with your value)
  return m * rawValue + b;
}

// Function to convert MQ135 analog reading to overall air quality ppm
float MQ135toAirQuality(int rawValue) {
  // Assuming a linear calibration curve: AirQuality_ppm = m * rawValue + b
  float m = 0.2; // Slope of the calibration curve (replace with your value)
  float b = 50; // Y-intercept of the calibration curve (replace with your value)
  return m * rawValue + b;
}

void setup() {
  Serial.begin(9600);
  delay(1500);

  dht.begin();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize LDR pin
  pinMode(ldrPin, INPUT);
  // Initialize water sensor pins
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF

  // Start up the Dallas Temperature sensor
  sensors.begin();
}

void loop() {
  delay(2000);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int airqualityvalue = analogRead(sensorPin);
  int lightState = digitalRead(ldrPin);

  // Read water sensor value
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  int waterValue = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  // Read Dallas Temperature sensor value
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  // Convert analog reading to CO2 ppm and overall air quality ppm
  float CO2_ppm = MQ135toCO2(airqualityvalue);
  float air_quality_ppm = MQ135toAirQuality(airqualityvalue);

  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("CO2 ppm: ");
  Serial.println(CO2_ppm);
  Serial.print("Overall Air Quality ppm: ");
  Serial.println(air_quality_ppm);

  // Light status
  if (lightState == HIGH)
    Serial.println("It is dark");
  else
    Serial.println("It is light");

  // Water sensor value
  Serial.print("The water sensor value: ");
  Serial.println(waterValue);

  // Dallas Temperature sensor value
  Serial.print("Temperature from Dallas Temperature sensor: ");
  Serial.println(tempC);

  // Create an instance of WiFiClient class to establish connection to ThingSpeak
  WiFiClient client;

  // Connect to ThingSpeak
  if (client.connect(server, 80)) {
    // Prepare the data to send
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(humidity);
    postStr += "&field2=";
    postStr += String(temperature);
    postStr += "&field3=";
    postStr += String(CO2_ppm);
    postStr += "&field4=";
    postStr += String(air_quality_ppm);
    postStr += "&field5=";
    postStr += String(lightState);
    postStr += "&field6=";
    postStr += String(waterValue);
    postStr += "&field7=";
    postStr += String(tempC);
    postStr += "\r\n\r\n";

    // Make a HTTP POST request to ThingSpeak
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    // Wait for server response
    delay(500);

    // Check for response from ThingSpeak
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
    Serial.println();

    // Close the connection
    client.stop();
  } else {
    // Unable to connect to ThingSpeak
    Serial.println("Failed to connect to ThingSpeak server");
  }

  // Delay before sending next set of data
  delay(2000); // 20 seconds delay between updates to ThingSpeak
}
