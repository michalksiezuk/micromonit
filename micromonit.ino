// LCD pin configuration
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Custom LCD characters
byte DEG[8] = {0b01100, 0b10010, 0b10010, 0b01100, 0b00000, 0b00000, 0b00000, 0b00000};
byte CPU[8] = {0b00000, 0b10101, 0b11111, 0b10001, 0b10001, 0b11111, 0b10101, 0b00000};
byte GPU[8] = {0b00000, 0b01110, 0b11111, 0b10101, 0b11111, 0b11111, 0b10101, 0b00000};

// Arduino pin configuration
const int buttonPin = 13;
const int echoPin = 9;
const int triggerPin = 8;

// Distance sensor
const int cmConversionDenominator = 58;
const int rangeThreshold = 5;
bool isInRange = false;
long echo;

// Push button
bool isButtonPressed = false;
int buttonState;
int currentState = LOW;
int previousState = LOW;
long debounce = 500;
long timeElapsed = 0;

// Serial readout
const byte numChars = 128;
char receivedChars[numChars];
char tempChars[numChars];
bool newData = false;

// CPU data
char cpuName[numChars] = {0};
int cpuTemperature = 0;
float cpuLoad = 0.0;
int cpuPower = 0;

// GPU data
char gpuName[numChars] = {0};
int gpuTemperature = 0;
float gpuLoad = 0.0;
int gpuPower = 0;


void setup() {
  // Set pin modes
  pinMode(buttonPin, INPUT);
  pinMode(echoPin, INPUT);
  pinMode(triggerPin, OUTPUT);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.createChar(0, DEG);
  lcd.createChar(1, CPU);
  lcd.createChar(2, GPU);
  lcd.clear();

  // Set data rate
  Serial.begin(9600);
}


void loop() {
  // Check UI toggle triggers
  checkPushButton();
  checkDistanceSensor();

  // Check incoming data
  receiveData();

  if (newData == true) {
    strcpy(tempChars, receivedChars);

    parseData();
    displayData();

    newData = false;
  }
}


void receiveData() {
  static bool receiveInProgress = false;
  static byte index = 0;
  char startMarker = "<";
  char endMarker = ">";
  char received;

  while (Serial.available() > 0 && newData == false) {
    received = Serial.read();

    if (receiveInProgress == true) {
      if (received != endMarker) {
        receivedChars[index] = received;
        index++;

        if (index >= numChars) {
          index = numChars - 1;
        }
      }
      else {
        receivedChars[index] = "\0";
        receiveInProgress = false;
        newData = true;
        index = 0;
      }
    }
    else if (received == startMarker) {
      receiveInProgress = true;
    }
  }
}


void parseData() {
  char * strtokIndex;

  strtokIndex = strtok(tempChars, ",");
  strcpy(cpuName, strtokIndex);

  strtokIndex = strtok(NULL, ",");
  cpuTemperature = atoi(strtokIndex);

  strtokIndex = strtok(NULL, ",");
  cpuLoad = atof(strtokIndex);

  strtokIndex = strtok(NULL, ",");
  cpuPower = atoi(strtokIndex);

  strtokIndex = strtok(NULL, ",");
  strcpy(gpuName, strtokIndex);

  strtokIndex = strtok(NULL, ",");
  gpuTemperature = atoi(strtokIndex);

  strtokIndex = strtok(NULL, ",");
  gpuLoad = atof(strtokIndex);

  strtokIndex = strtok(NULL, ",");
  gpuPower = atoi(strtokIndex);
}


void displayData() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(currentState == LOW ? byte(1) : byte(2));
  lcd.print(" ");
  lcd.print(currentState == LOW ? cpuName : gpuName);
  lcd.setCursor(0, 1);
  lcd.print("  ");
  lcd.print(currentState == LOW ? cpuTemperature : gpuTemperature);
  lcd.write(byte(0));
  lcd.print(" ");
  lcd.print(currentState == LOW ? cpuLoad : gpuLoad);
  lcd.print("%");
  lcd.print(" ");
  lcd.print(currentState == LOW ? cpuPower : gpuPower);
  lcd.print("W");
}


void toggleUIState() {
  currentState = currentState == HIGH ? LOW : HIGH;
  timeElapsed = millis();
}


void checkPushButton() {
  buttonState = digitalRead(buttonPin);
  isButtonPressed = buttonState == HIGH && previousState == LOW;

  if (isButtonPressed && millis() - timeElapsed > debounce) {
    toggleUIState();
  }

  previousState = buttonState;
}


void checkDistanceSensor() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(15);

  digitalWrite(triggerPin, LOW);
  digitalWrite(echoPin, HIGH);

  echo = pulseIn(echoPin, HIGH);
  isInRange = echo / cmConversionDenominator < rangeThreshold;

  if (isInRange && millis() - timeElapsed > debounce) {
    toggleUIState();
  }
}
