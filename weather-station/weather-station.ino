#include <neotimer.h>
#include <Servo.h>
#include <DHT.h>
// #include <Adafruit_Sensor.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define SERVO 5 
#define WATERPIN 6
#define ANALOGPIN A0
#define DHTPIN 7
#define DHTTYPE DHT11

#define TIMER 500
#define NULL 0

Servo microServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

Neotimer sensorWater;
Neotimer sensorDHT;
Neotimer motorServo;
Neotimer lcdFreq;

float humidity = 0;
float temperature = 0;
bool rain = false;

void setup () {
  pinMode(WATERPIN, INPUT);
  pinMode(ANALOGPIN, INPUT);
  
  microServo.attach(SERVO);
  microServo.write(NULL);

  dht.begin();
  lcd.init();
  lcd.clear();
  lcd.backlight();

  sensorWater.set(TIMER);
  sensorDHT.set(TIMER);
  motorServo.set(TIMER);
  lcdFreq.set(TIMER);

  Serial.begin(9600);
}

void loop () {
  if(sensorDHT.repeat()) readDataDHT();
  if(sensorWater.repeat(2)) readRain();
  if(motorServo.repeat()) moveToServo();
}

void readDataDHT() {
  float currentHumidity = dht.readHumidity();
  float currentTemperature = dht.readTemperature(); 

  if(isnan(humidity) || isnan(temperature)) {
    if(lcdFreq.repeat()) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.setCursor(6, 0);
      lcd.print("Erro NaN");

      lcd.setCursor(0, 1);
      lcd.print("Hum: ");
      lcd.setCursor(5, 0);
      lcd.print("Erro NaN");
    }
  } else {
    humidity = currentHumidity;
    temperature = currentTemperature;
    if(lcdFreq.repeat()) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.setCursor(6, 0);
      lcd.print(temperature);

      lcd.setCursor(0, 1);
      lcd.print("Hum: ");
      lcd.setCursor(5, 0);
      lcd.print(humidity);
    }
  }
}

void readRain () {
  if(digitalRead(WATERPIN)) rain = true;
  else rain = false;
}

void moveToServo() {
  if(rain) {
    if(lcdFreq.repeat()) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Esta chovendo!");
      lcd.setCursor(0, 1);
      lcd.print("Ligando toldo.");
    }
    for(int index=0; index <= 180; index++) {
      microServo.write(index);
    }
  }
  else {
    if(lcdFreq.repeat()) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Parou a chuva!");
      lcd.setCursor(0, 1);
      lcd.print("Desligando toldo");
    }
    for(int index=180; index >= 0; index--) {
      microServo.write(index);
    }
  } 
}
