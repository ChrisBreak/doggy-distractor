#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "Servo.h"

#define SOUND_SENSOR A0
#define LED 12              // the number of the LED pin  
#define THRESHOLD_VALUE 450 //The threshold to turn the led on 400.00*5/1024 = 1.95v 

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
Servo servoMotor;

void setup()
{
  pinMode(7, INPUT);            //for the touch sensor input
  pinMode(LED, OUTPUT);         //for LED input
  pinMode(SOUND_SENSOR, INPUT); //for sound sensor input
  servoMotor.attach(9);         //for servo motor pin
  
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  Serial.println();
  Serial.println(("DFRobot DFPlayer Mini Demo"));
  Serial.println(("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println("Unable to begin:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
    while(true);
  }
  Serial.println("DFPlayer Mini online.");

  servoMotor.write(80);
}

void loop()
{
  static unsigned long timer = millis();
  int sensorValue = analogRead(SOUND_SENSOR);//use A0 to read the electrical signal
  Serial.print("sensorValue: "); 
  Serial.println(sensorValue);

  //if the touch sensor is pressed, then run the motor
  if(digitalRead(7) == HIGH)
  {
  for (int pos = 80; pos >= 0; pos -= 1) 
    { // goes from 0 degrees to 80 degrees in steps of 1 degree
      servoMotor.write(pos);              // tell servo to go to position in variable 'pos'
      delay(2);                       // waits 2ms for the servo to reach the position
    }
    delay(10);
    for (int pos = 0; pos <= 80; pos += 1) { // goes from 180 degrees to 0 degrees
      servoMotor.write(pos);              // tell servo to go to position in variable 'pos'
      delay(2);                       // waits 2ms for the servo to reach the position
    }
  delay(500);
  }
  
  //check if it is loud enough and if it is, then play the sound
  if(sensorValue > THRESHOLD_VALUE){
    int randSound = random(1, 10);
    if ((randSound % 2) == 0) randSound += 1;
    int soundVol = 30;
    if ((randSound == 3) || (randSound == 9)) soundVol = 20;
    
    myDFPlayer.volume(soundVol);  //Set volume value. From 0 to 30
    myDFPlayer.play(randSound);  //Play the first mp3
    for(int i = 0; i < 30; i++)
    {
      turnOnLED();
      delay(70);
      turnOffLED();
      delay(70);
    }
    delay(4000);
  }
  
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}
void turnOnLED() 
{ 
 digitalWrite(LED,HIGH); 
} 
void turnOffLED() 
{ 
 digitalWrite(LED,LOW); 
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
