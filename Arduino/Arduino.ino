#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins
const int smokePin = A0;
const int buzzer = 8;
const int led = 9;
const int relay = 7;

// Smoke threshold
const int SMOKE_THRESHOLD = 300;

// AI signal
char aiSignal = 'S';

// Stability counter
int smokeCounter = 0;
const int SMOKE_CONFIRM = 4;

void setup() {

  Serial.begin(9600);

  // Set outputs BEFORE anything else
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(relay, OUTPUT);

  // Everything OFF at startup
  digitalWrite(buzzer, LOW);
  digitalWrite(led, LOW);
  digitalWrite(relay, LOW);

  // LCD
  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FIRE DETECTOR");
  lcd.setCursor(0, 1);
  lcd.print("SYSTEM READY");

  delay(2000);

  lcd.clear();
}

void loop() {

  // -------------------------
  // 1. RECEIVE AI SIGNAL
  // -------------------------
  while (Serial.available() > 0) {

    char received = Serial.read();

    if (received == 'F') {
      aiSignal = 'F';
    }

    if (received == 'S') {
      aiSignal = 'S';
    }
  }


  // -------------------------
  // 2. READ SMOKE SENSOR
  // -------------------------
  int smokeVal = analogRead(smokePin);

  bool smokeDetected = false;

  if (smokeVal > SMOKE_THRESHOLD) {
    smokeCounter++;

    if (smokeCounter >= SMOKE_CONFIRM) {
      smokeDetected = true;
    }

  } else {
    smokeCounter = 0;
  }


  // -------------------------
  // 3. FIRE DECISION
  // -------------------------
  bool fireDetected = (aiSignal == 'F' || smokeDetected);


  // -------------------------
  // 4. FIRE MODE
  // -------------------------
  if (fireDetected) {

    digitalWrite(buzzer, HIGH);
    digitalWrite(led, HIGH);
    digitalWrite(relay, HIGH);

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("!! FIRE ALERT !!");

    lcd.setCursor(0, 1);

    if (aiSignal == 'F') {
      lcd.print("AI FIRE");
    }
    else {
      lcd.print("SMOKE DETECTED");
    }

  }


  // -------------------------
  // 5. SAFE MODE
  // -------------------------
  else {

    digitalWrite(buzzer, LOW);
    digitalWrite(led, LOW);
    digitalWrite(relay, LOW);

    lcd.setCursor(0, 0);
    lcd.print("STATUS: SAFE   ");

    lcd.setCursor(0, 1);
    lcd.print("Smoke: ");
    lcd.print(smokeVal);
    lcd.print("    ");
  }


  delay(200);
}