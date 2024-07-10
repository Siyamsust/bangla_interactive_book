#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

DFRobotDFPlayerMini myDFPlayer;

#define VOLUME_UP_PIN 32
#define VOLUME_DOWN_PIN 35
#define START_PIN 34

// Define pin numbers for page select buttons
#define PAGE_SELECT_PINS {33, 23, 19}

// Define pin numbers for the 12 buttons
#define BUTTON_PINS {26, 25, 14, 27, 12, 13, 21, 22, 5, 15, 2, 4}

int currentPage = 0;
int volume = 20;
int fileMap[3][12] = {
  {35, 36, 34,33,32, 31, 30, 29, 28, 27, 26,37 },   // Page 0
  {25,24, 23, 22, 21, 20, 18, 19, 17, 16, 15, 14}, // Page 1
  { 13,12, 1, 2, 6, 4, 5, 11, 10, 9, 8, 7}  // Page 2
};

// Store the last state of each button to detect state changes
bool lastButtonState[12] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};//when high means 0 

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 17, 16);

  if (!myDFPlayer.begin(Serial2)) {
    Serial.println(F("Unable to begin DFPlayer Mini. Please check the connection and SD card."));
    while (true);
  }

  myDFPlayer.setTimeOut(15000);
  myDFPlayer.volume(volume);

  // Initialize volume control buttons with internal pull-up resistors
  pinMode(VOLUME_UP_PIN, INPUT_PULLUP);
  pinMode(VOLUME_DOWN_PIN, INPUT_PULLUP);
  pinMode(START_PIN, INPUT_PULLUP);

  // Initialize page select buttons with internal pull-up resistors
  int pageSelectPins[] = PAGE_SELECT_PINS;
  for (int i = 0; i < 3; i++) {
    pinMode(pageSelectPins[i], INPUT_PULLUP);
  }

  // Initialize the 12 buttons with internal pull-up resistors
  int buttonPins[] = BUTTON_PINS;
  for (int i = 0; i < 12; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void loop() {
  int pageSelectPins[] = PAGE_SELECT_PINS;
  int buttonPins[] = BUTTON_PINS;

  // Handle volume up button press
  if (digitalRead(VOLUME_UP_PIN) == LOW) {
    volume = min(volume + 1, 30);
    myDFPlayer.volume(volume);
    Serial.print("Volume up: ");
    Serial.println(volume);
    delay(200);  // debounce delay
  }

  // Handle volume down button press
  if (digitalRead(VOLUME_DOWN_PIN) == LOW) {
    volume = max(volume - 1, 0);
    myDFPlayer.volume(volume);
    Serial.print("Volume down: ");
    Serial.println(volume);
    delay(200);  // debounce delay
  }

  // Handle page select button presses
  for (int i = 0; i < 3; i++) {
    if (digitalRead(pageSelectPins[i]) == LOW) {
      currentPage = i;
      Serial.print("Page selected: ");
      Serial.println(currentPage);
      delay(200);  // debounce delay
    }
  }

  // Handle track selection button presses
  for (int i = 0; i < 12; i++) {
    int buttonState = digitalRead(buttonPins[i]);
    if (buttonState != lastButtonState[i]) {
      if (buttonState == LOW) {
        int fileToPlay = fileMap[currentPage][i];
        Serial.print("Playing file: ");
        Serial.println(fileToPlay);
        myDFPlayer.play(fileToPlay);
      }
      lastButtonState[i] = buttonState;
      delay(200);  // debounce delay
    }
  }
}
