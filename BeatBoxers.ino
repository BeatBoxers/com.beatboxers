#include <SoftwareSerial.h>

const boolean DEBUG = false;

SoftwareSerial btSerial(4, 5);

const int MUX_PIN_A = 9;
const int MUX_PIN_B = 10;
const int MUX_PIN_C = 11;
const int READ_PIN = A3;
const int LOW_BAT_READ_PIN = A1;
const int INDICATOR_LED_PIN = 7;

const int PAD_COUNT = 8;

const int PADS[PAD_COUNT] = {
  7, 5, 6, 2,
  3, 1, 4, 0
};

const int THRESHOLD = 300;
const int HIT_DELAY = 150;

unsigned long padsTimer[PAD_COUNT];
unsigned long lowBatteryLedBlinkTimer = 0;

char hitPads[(PAD_COUNT + 1)] = {
  '0', '0', '0', '0',
  '0', '0', '0', '0',
  '\0'
};
boolean hitReceived = false;
unsigned long currentTime = 0;
int reading = 0;
char padChar = '0';

void setup() {
  if (DEBUG) {
    Serial.begin(9600);
  }
  
  btSerial.begin(9600);
  
  //setup MUX pins
  pinMode(MUX_PIN_A, OUTPUT);
  pinMode(MUX_PIN_B, OUTPUT);
  pinMode(MUX_PIN_C, OUTPUT);
  
  //setup
  pinMode(INDICATOR_LED_PIN, OUTPUT);
  
  debugPrint("Start");
}

void loop() {
  //if we have received anything from the phone
  /*
  if (btSerial.available()) {
    String received = "";
    
    while (btSerial.available() > 0)  {
      received += char(btSerial.read());
      delay(2);
    }
    
    debugPrint("Received:");
    debugPrint(received);
  }
  */
  
  //clear our array first
  hitReceived = false;
  currentTime = millis();
   
  for (int i = 0; i < PAD_COUNT; i++) {
    setMuxReadPin(PADS[i]);
    
    reading = analogRead(READ_PIN);
    
    //since there are no really fast ways to convert an int to a char, and since we need a
    //char array to send to the BT device as our message, we have to do some things here to be
    //as fast as possible. The String class could solve this for us but it isn't as fast.
    if (i == 0) {
      padChar = '1';
    }
    else if (i == 1) {
      padChar = '2';
    }
    else if (i == 2) {
      padChar = '3';
    }
    else if (i == 3) {
      padChar = '4';
    }
    else if (i == 4) {
      padChar = '5';
    }
    else if (i == 5) {
      padChar = '6';
    }
    else if (i == 6) {
      padChar = '7';
    }
    else if (i == 7) {
      padChar = '8';
    }
    
    if (reading > THRESHOLD) {
      unsigned long timeSinceLastHit = currentTime - padsTimer[i];
     
      if (HIT_DELAY < timeSinceLastHit || currentTime < padsTimer[i]) {
        hitPads[i] = padChar;
        hitReceived = true;
        padsTimer[i] = currentTime;//reset this sensors timer
      }
    }
  }
  
  if (hitReceived) {
    btSerial.print(hitPads);
    
    //clear our hitPads char array
    for (int i = 0; i < PAD_COUNT; i++) {
      hitPads[i] = '0';
    }
  }
  
  int lowBatteryRead = analogRead(LOW_BAT_READ_PIN);
  
  if (lowBatteryRead < 150) {
    //the battery is low, blink
    unsigned long timeSinceLastBlink = currentTime - lowBatteryLedBlinkTimer;
    
    if (timeSinceLastBlink > 1000) {
      digitalWrite(INDICATOR_LED_PIN, !digitalRead(INDICATOR_LED_PIN));
      
      lowBatteryLedBlinkTimer = currentTime;
    }
  }
  else {
    digitalWrite(INDICATOR_LED_PIN, HIGH);
  }
}

void setMuxReadPin(int padPosition) {
  digitalWrite(MUX_PIN_A, bitRead(padPosition, 0));
  digitalWrite(MUX_PIN_B, bitRead(padPosition, 1));
  digitalWrite(MUX_PIN_C, bitRead(padPosition, 2));
}

void debugPrint(String str) {
  if (DEBUG) {
      Serial.println(str);
  }
}
