#include <Arduino.h>
#include <FastCRC.h>

FastCRC8 CRC8;

int buzzer = A0;
int ledPin = PD3;
int buttonPin = 2;

int lastButtonState = 1;
long unsigned int lastPress;
volatile int buttonFlag;
int debounceTime = 20;
int counter = 0;
bool change;

bool Turbo = 1; //1 for on, 0 for off
bool seconddigit = 1; //Still unknown function
bool fastAcceleration = 1; //1 for on, 0 for off
bool KPH = 1; //1 for KPH, 0 for MPH
bool fifthdigit = 0; //Still unknown function
bool Light = 1; //1 for on, 0 for off
bool LightBlink = 0; //1 for on, 0 for off
bool ESCOn = 1; //1 for on, 0 for off
int SpeedLimit = 255; //Beetwen 0 and 255
int forth;
byte code[6] = {0xA6, 0x12, 0x02};

void calculateforth() {
  forth = 0;
  if (Turbo == 1){
    forth = forth + 128;
      }
  if (seconddigit == 1){
    forth = forth + 64;
      }
  if (fastAcceleration == 1){
    forth = forth + 32;
      }   
  if (KPH == 1){
    forth = forth + 16;
      }
  if (fifthdigit == 1){
    forth = forth + 8;
      }
  if (Light == 1){
    forth = forth + 4;
      }
  if (LightBlink == 1){
    forth = forth + 2;
      }
  if (ESCOn == 1){
    forth++;
      }
  code[3] = forth;
  code[4] = SpeedLimit;
  code[5] = CRC8.maxim(code, 5);
  change = 1;
}

void ISR_button()
{
  buttonFlag = 1;
}

void setup() {
  Serial.begin(9600);
  calculateforth();
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin,LOW);
  delay(500);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), ISR_button, CHANGE);
  tone(buzzer, 440, 40);
  digitalWrite(ledPin,HIGH);
}

void loop() {
if((millis() - lastPress) > debounceTime && buttonFlag)
  {
    lastPress = millis();   //update lastPress                                                     
    if(digitalRead(buttonPin) == 0 && lastButtonState == 1)    //if button is pressed and was released last change
    {
      Light = !Light;
      calculateforth();
      if(Light == 1){tone(buzzer, 880, 40);}else{tone(buzzer, 440, 40);}
      lastButtonState = 0;    //record the lastButtonState
    }
    
    else if(digitalRead(buttonPin) == 1 && lastButtonState == 0)    //if button is not pressed, and was pressed last change
    {
      lastButtonState = 1;    //record the lastButtonState
    }
    buttonFlag = 0;
  }
  if (counter == 500 || change == 1){
    Serial.write(code, sizeof(code));
    counter = 0;
    change = 0;
  }
delay(1);
counter++;
}
