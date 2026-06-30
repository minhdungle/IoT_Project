#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

#define I2C_SDA 8
#define I2C_SCL 9
#define TEMP_PIN 5

LiquidCrystal_I2C lcd27(0x27, 16, 2);
LiquidCrystal_I2C lcd3F(0x3F, 16, 2);
LiquidCrystal_I2C *lcd = &lcd27;

bool isI2CFound(byte address) {
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;
}

void printLine(int row, String text) {
  lcd->setCursor(0, row);

  if (text.length() > 16) {
    text = text.substring(0, 16);
  }

  lcd->print(text);

  for (int i = text.length(); i < 16; i++) {
    lcd->print(" ");
  }
}

void setupLCD() {
  if (isI2CFound(0x27)) {
    lcd = &lcd27;
  } else if (isI2CFound(0x3F)) {
    lcd = &lcd3F;
  } else {
    lcd = &lcd27;
  }

  lcd->init();
  lcd->backlight();

  printLine(0, "Air Sensor");
  printLine(1, "LCD Starting");
  delay(1500);
  lcd->clear();
}

float readTemperatureC() {
  int raw = analogRead(TEMP_PIN);

  if (raw <= 0 || raw >= 4095) {
    return -999.0;
  }

  const float B = 4275.0;
  const float R0 = 100000.0;

  float resistance = 4095.0 / raw - 1.0;
  resistance = R0 * resistance;

  float tempC =
    1.0 / (log(resistance / R0) / B + 1.0 / 298.15) - 273.15;

  return tempC;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  analogReadResolution(12);
  analogSetPinAttenuation(TEMP_PIN, ADC_11db);

  Wire.begin(I2C_SDA, I2C_SCL);

  setupLCD();

  Serial.println("System started");
}

void loop() {
  float tempC = readTemperatureC();

  if (tempC > -100.0) {
    printLine(0, "Temp: " + String(tempC, 1) + " C");
    printLine(1, "APM: Not Ready");

    Serial.print("Temperature = ");
    Serial.print(tempC);
    Serial.println(" C");
  } else {
    printLine(0, "Temp: No Data");
    printLine(1, "Check sensor");

    Serial.println("Temperature read failed");
  }

  delay(1000);
}
