/* 
 
  HHO-ECU-Project (OLED display with Arduino UNO)
  (Analog pressure sensor, DH11 module, IRF520 module, ACS712 module,and buzzer module)
 
  modified on 19 June 2023 
  by MBDK Siriwardena 
  https://github.com/dasun1997/HHO-ECU-Project.git
 
*/ 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define ACS712_SENSITIVITY 0.185 // Sensitivity of ACS712-05B in millivolts per ampere

#define DHTPIN A1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); // Create DHT object for temperature and humidity measurement

#define PWM_PIN 5
#define BUZZER_PIN 2

const int pressureInput = A2;               // Select the analog input pin for the pressure transducer
const int pressureZero = 102.4;            // Analog reading of pressure transducer at 0 psi
const int pressureMax = 921.6;             // Analog reading of pressure transducer at 100 psi
const int pressuretransducermaxPSI = 100;  // PSI value of the transducer being used
const int baudRate = 9600;                 // Constant integer to set the baud rate for serial monitor
const int sensorreadDelay = 250;           // Constant integer to set the sensor read delay in milliseconds

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
  pinMode(PWM_PIN, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true); // Don't proceed, loop forever
  }

  tone(BUZZER_PIN, 2000); // Send 1KHz sound signal...
  delay(500);             // ...for 500ms
  noTone(BUZZER_PIN);     // Stop sound...

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println(" HHO Cell");
  display.println("Automation");
  display.print("  PROJECT");
  display.display();
  delay(5000);
  display.clearDisplay();

  tone(BUZZER_PIN, 1000); // Send 1KHz sound signal...
  delay(500);             // ...for 500ms
  noTone(BUZZER_PIN);     // Stop sound...
}

void loop() {
  float pressureValue = analogRead(pressureInput); // Read value from input pin
  pressureValue = map(pressureValue, pressureZero, pressureMax, 0, pressuretransducermaxPSI); // Convert analog reading to psi
  float pressure_bar = pressureValue * 0.068948;
  Serial.print("Pressure = ");
  Serial.print(pressure_bar, 2);
  Serial.println(" bars");

  display.print("Pressure = ");
  display.print(pressure_bar, 2);
  display.println(" bars");
  display.print("           ");
  display.print(pressureValue, 2);
  display.println(" psi");
  display.println(" ");

  float voltageValue = analogRead(A0);
  float vOUT = (voltageValue * 5.0) / 1024.0;
  float R1 = 30000;
  float R2 = 7500;
  float vIN = vOUT / (R2 / (R1 + R2));
  Serial.print("Voltage IN = ");
  Serial.println(vIN, 2);
  Serial.print("Voltage OUT = ");
  Serial.println(vOUT, 2);
  display.print("Voltage = ");
  display.print(vIN, 2);
  display.println(" V");

  float current_mA = (voltageValue - 512) * ACS712_SENSITIVITY; // Calculate current in milliamperes
  float current_A = current_mA / 1000; // Convert milliamperes to amperes
  Serial.print("Current = ");
  Serial.print(current_A, 2);
  Serial.println(" A");
  display.print("Current = ");
  display.print(current_A, 2);
  display.println(" A");
  display.println(" ");

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.print("%, Temperature = ");
  Serial.print(temperature);
  Serial.println("°C");
  display.print("T = ");
  display.print(temperature, 0);
  display.print(" ");
  display.write(176); // Degree symbol
  display.print("C");
  display.print("   ");
  display.print("H = ");
  display.print(humidity, 0);
  display.print(" %");

  int out = map(pressureValue, 0, 100, 68, 48);
  analogWrite(PWM_PIN, out);
  Serial.print("output_P Value: ");
  Serial.println(out);
  Serial.println(" ");

  display.display();
  delay(1000);
}
