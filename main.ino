#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6-hVlrs5T"
#define BLYNK_TEMPLATE_NAME "Auto watering"
#define BLYNK_AUTH_TOKEN "yC4Yi_p3A4GTKVMvVeWWMIR8aDXDPIub"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define sensor A0
#define water_pump D5

char ssid[] = "AN-DX11";
char pass[] = "12345678";

BlynkTimer timer;
bool pumpState = false;
bool autoMode = true;

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);

  pinMode(water_pump, OUTPUT);
  digitalWrite(water_pump, LOW); // bơm OFF

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(1000L, readSensors);
}

// ================== MANUAL MODE ==================
BLYNK_WRITE(V1) { // nút bật/tắt bơm
  if (!autoMode) {
    pumpState = param.asInt();
    digitalWrite(water_pump, pumpState ? HIGH : LOW);
  }
}

// ================== MODE SELECT ==================
BLYNK_WRITE(V2) { // chọn AUTO / MANUAL
  int mode = param.asInt();   // 0 = AUTO | 1 = MANUAL
  autoMode = (mode == 0);

  if (autoMode) {
    readSensors();
  }
}

// ================== SENSOR ==================
void readSensors() {
  int rawValue = analogRead(sensor);
  int moisture = map(rawValue, 0, 1023, 100, 0);
  moisture = constrain(moisture, 0, 100);

  Blynk.virtualWrite(V0, moisture);

  Serial.print("Moisture: ");
  Serial.print(moisture);
  Serial.println("%");

  if (autoMode) {
    if (moisture <= 30) {
      digitalWrite(water_pump, HIGH);
      Blynk.virtualWrite(V1, 1);
    } else {
      digitalWrite(water_pump, LOW);
      Blynk.virtualWrite(V1, 0);
    }
  }
}

// ================== LOOP ==================
void loop() {
  Blynk.run();
  timer.run();
}
