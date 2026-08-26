#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>

// --- WI-FI SETTINGS ---

const char* ssid = ""; // wifi name
const char* password = ""; // wifi password

// --- LARAVEL API URLs ---
// Route 1: For the 5-second water data
const char* sensorServerName = "";//put https API url

// Route 2: For the 30-second GPS tracking
const char* gpsServerName = "";//put https API url

// --- SENSOR PINS ---
const int trigPin = 5;
const int echoPin = 18;
const int flowPin = 13;

// --- DRAINAGE CALIBRATION ---
const float DRAIN_DEPTH_CM = 6.1;      
const float DRY_DRAIN_THRESHOLD_PCT = 10.0; 
const float LEVEL_THRESHOLD_PCT = 50.0;    
const float FLOW_MIN_THRESHOLD = 1.0;      

// --- VARIABLES & TIMERS ---
volatile long pulseCount = 0;
float flowRate = 0.0;
unsigned long lastSensorTime = 0;
unsigned long lastGpsTime = 0;
String currentStatus = ""; 

// --- GPS SETUP ---
TinyGPSPlus gps;
HardwareSerial gpsSerial(2); 

// Interrupt Service Routine for Flow Sensor
void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(flowPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(flowPin), pulseCounter, RISING); 

  // --- CONNECT TO WI-FI ---
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nCONNECTED! IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("System Initialized. Starting Sensors...");
}

void loop() {
  // 1. CONSTANTLY READ GPS MODULE (Must run continuously)
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // 2. SENSOR TIMER: Every 5 seconds
  if ((millis() - lastSensorTime) > 5000) { 
    
    // Calculate Flow Rate
    detachInterrupt(digitalPinToInterrupt(flowPin)); 
    flowRate = ((1000.0 / (millis() - lastSensorTime)) * pulseCount) / 7.5;
    pulseCount = 0; 
    attachInterrupt(digitalPinToInterrupt(flowPin), pulseCounter, RISING); 

    // Calculate Water Level
    digitalWrite(trigPin, LOW); delayMicroseconds(2);
    digitalWrite(trigPin, HIGH); delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH);
    float distanceCm = duration * 0.034 / 2;
    
    float waterLevelPct = 0;
    if (distanceCm < DRAIN_DEPTH_CM) {
      waterLevelPct = ((DRAIN_DEPTH_CM - distanceCm) / DRAIN_DEPTH_CM) * 100.0;
    }
    if (waterLevelPct < 0) waterLevelPct = 0;

    // Algorithm Logic
    if (waterLevelPct < DRY_DRAIN_THRESHOLD_PCT && flowRate <= FLOW_MIN_THRESHOLD) {
      currentStatus = "SAFE";
    } else if (waterLevelPct <= LEVEL_THRESHOLD_PCT && flowRate >= FLOW_MIN_THRESHOLD) {
      currentStatus = "SAFE";
    } else if (waterLevelPct >= LEVEL_THRESHOLD_PCT && flowRate >= FLOW_MIN_THRESHOLD) {
      currentStatus = "FLOOD ALERT";
    } else if (waterLevelPct <= LEVEL_THRESHOLD_PCT && flowRate < FLOW_MIN_THRESHOLD) {
      currentStatus = "BLOCKAGE ALERT";
    } else if (waterLevelPct > LEVEL_THRESHOLD_PCT && flowRate < FLOW_MIN_THRESHOLD) {
      currentStatus = "CRITICAL ALERT";
    }

    // Print & Send Sensor Data
    Serial.println("\n[SENSOR] Lvl: " + String(waterLevelPct) + "% | Flow: " + String(flowRate) + " L/min | Status: " + currentStatus);
    sendSensorData(waterLevelPct, flowRate, currentStatus);

    lastSensorTime = millis();
  }

  // 3. GPS TIMER: Every 30 seconds
  if ((millis() - lastGpsTime) > 30000) {
    
    if (gps.location.isValid()) {
      float lat = gps.location.lat();
      float lng = gps.location.lng();
      
      Serial.println("[GPS] Location Fixed! Lat: " + String(lat, 6) + " Lng: " + String(lng, 6));
      sendGpsData(lat, lng);
    } else {
      Serial.println("[GPS] Searching for satellites... (Module needs clear view of the sky)");
    }

    lastGpsTime = millis();
  }
}

// ==========================================
// HELPER FUNCTIONS TO SEND DATA TO LARAVEL
// ==========================================

void sendSensorData(float water, float flow, String status) {
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(sensorServerName);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"device_id\": 1, \"water_level\": " + String(water) + 
                         ", \"water_flow\": " + String(flow) + 
                         ", \"status\": \"" + status + "\"}";
                         
    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) Serial.println("[SENSOR] Sent! Server Reply Code: " + String(httpResponseCode));
    else Serial.println("[SENSOR] Error sending data: " + String(httpResponseCode));
    
    http.end(); 
  } else {
    // NEW: Tell us it disconnected and try to fix it!
    Serial.println("[SENSOR] Error: Wi-Fi Disconnected! Attempting to reconnect...");
    WiFi.reconnect(); 
  }
}

void sendGpsData(float lat, float lng) {
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(gpsServerName);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"device_id\": 1, \"latitude\": " + String(lat, 6) + 
                         ", \"longitude\": " + String(lng, 6) + "}";
                         
    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) Serial.println("[GPS] Sent! Server Reply Code: " + String(httpResponseCode));
    else Serial.println("[GPS] Error sending data: " + String(httpResponseCode));
    
    http.end(); 
  } else {
    // NEW: Tell us it disconnected and try to fix it!
    Serial.println("[GPS] Error: Wi-Fi Disconnected! Attempting to reconnect...");
    WiFi.reconnect();
  }
}