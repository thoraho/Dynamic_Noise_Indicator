#include <Adafruit_NeoPixel.h>

int sensorPin = A0;
int motorPin = 3;
#define LED_PIN 6
#define LED_COUNT 64

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

float dBArray[100] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


const float V_REF = 5;          
const float DB_FLOOR = 23.0;      
const float DB_CEILING = 60.0;   
const float V_SILENCE = 0.005;     
const float V_MAX = 5;          

void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(motorPin, OUTPUT);
  strip.begin();
  strip.show();
  strip.setBrightness(100);
  Serial.begin(9600);
}

void addValueToArray(float newValue) {
  for (int i = 0; i < 9; i++) {
    dBArray[i] = dBArray[i + 1];
  }
  dBArray[9] = newValue;
}

float findAmplitudeVoltage(int duration) {
  unsigned long startMillis = millis();
  int maxVal = 0;
  int minVal = 1023;

  while (millis() - startMillis < duration) {
    int reading = analogRead(sensorPin);
    if (reading > maxVal) maxVal = reading;
    if (reading < minVal) minVal = reading;
  }


  float voltage = (maxVal - minVal) * (V_REF / 1023.0);
  return voltage;
}

float voltageTodB(float voltage) {
  if (voltage < V_SILENCE) voltage = V_SILENCE; 


  float dB = DB_FLOOR + 20.0 * log10(voltage / V_SILENCE);
  dB = constrain(dB, DB_FLOOR, DB_CEILING);
  return dB;
}

float getAverage() {
  float sum = 0;
  for (int i = 0; i < 100; i++) {
    sum += dBArray[i];
  }
  return sum / 10.0;
}

void loop() {
  float voltage = findAmplitudeVoltage(30);
  float dB = voltageTodB(voltage);
  addValueToArray(abs(dB));

  float avgdB = getAverage();

  avgdB = abs(avgdB);


  int maxSpeed = 255;
  int minSpeed = 90;
  int motorValue = map((int)avgdB, 48, 60, minSpeed, maxSpeed);
  motorValue = constrain(motorValue, minSpeed, maxSpeed);
  analogWrite(motorPin, motorValue);


  int colorShift = map((int)avgdB, 48, 60, 0, 255);
  colorShift = constrain(colorShift, 0, 255);
  int red = colorShift;
  int green = 255 - colorShift;

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(red, green, 0));
  }
  strip.show();

  Serial.print("Voltage: "); Serial.print(voltage, 3);
  Serial.print("\t dB: ");    Serial.print(dB, 1);
  Serial.print("\t Avg dB: "); Serial.print(avgdB, 1);
  Serial.print("\t Red: ");   Serial.print(red);
  Serial.print("\t Green: "); Serial.println(green);

  // delay(5);
}
