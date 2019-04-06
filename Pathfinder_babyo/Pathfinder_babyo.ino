#include <Wire.h>
#include <avr/interrupt.h>

//I2C comm variables
int receiveData[3];
byte sendData[3] = {99, 0, 5};

//Data Buckets
int targetLinVel = 0;
int targetAngVel = 0;
int actualLinVel;
int actualAngVel;
int batteryTemp;
int batteryVolt;

//Pin Setup
const int battTempPin = A0;
const int battVoltPin = A6;
const int battStatPin = 7;
const int rightAPin = A1; //PC1-PCINT9
const int rightBPin = 9;  //PB1-PCINT1
const int leftAPin = 2;   //PD2-INT0
const int leftBPin = 4;   //PD4-PCINT20

//Interrupt Bucket Setup
volatile long rightMotor = 0;
volatile long leftMotor = 0;

//Speed Setup
unsigned long timeI = 0;
unsigned long timeF = 0;
int rightMotorSpd = 0;
int leftMotorSpd = 0;

void setup() {
  //Pin Change Interrupt Setup
  cli();                //turn off interrupts
  PCICR |= 0b00000111;  // Enables Ports B, C, and D Pin Change Interrupts
  PCMSK0 |= 0b00000010; // PCINT1
  PCMSK1 |= 0b00000010; // PCINT9
  PCMSK2 |= 0b00010000; // PCINT20
  sei();                //turn on interrupts

  //External Interrupt Setup
  attachInterrupt(digitalPinToInterrupt(leftAPin), leftAIntRoutine, CHANGE);

  //Pin Mode Setup
  pinMode(battStatPin, INPUT);
  pinMode(rightAPin, INPUT);
  pinMode(rightBPin, INPUT);
  pinMode(leftAPin, INPUT);
  pinMode(leftBPin, INPUT);
  
  //I2C communication setup
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //Timer initialization
  timeI = micros();
}

void loop() {
  
  delay(100);
  //when read battery temp convert to integer in degC
  //when read battery voltage convert to mV

}

// Port B, PCINT0 - PCINT7 - my application PCINT1, PB1, rightBPin
ISR(PCINT0_vect){
  if(digitalRead(rightBPin) == digitalRead(rightAPin)){
    rightMotor++;
  }
  else{
    rightMotor--;
  }
}

// Port C, PCINT8 - PCINT14 - my application PCINT9, PC1, rightAPin
ISR(PCINT1_vect){
  if(digitalRead(rightBPin) == digitalRead(rightAPin)){
    rightMotor--;
  }
  else{
    rightMotor++;
  }
}

// Port D, PCINT16 - PCINT23 - my application PCINT20, PD4, leftBPin
ISR(PCINT2_vect){
  if(digitalRead(leftBPin) == digitalRead(leftAPin)){
    leftMotor++;
  }
  else{
    leftMotor--;
  }
}

// External Interrupt Routine - my application INT0, PD2, leftAPin
void leftAIntRoutine(){
  if(digitalRead(leftBPin) == digitalRead(leftAPin)){
    leftMotor--;
  }
  else{
    leftMotor++;
  }
}

void receiveEvent(int byteNum){

  bool readFlag = false;
  bool taskFlag = false;  //false for Write status, true for Read status
  bool skipRead = false;

  if(byteNum > 3){
    sendData[0] = 99;
    sendData[1] = 0;
    sendData[2] = 4;
  }
  else{
    for(int i = 0; i < byteNum; i++){
      receiveData[i] = Wire.read(); //fill received data array with I2C data
    }
  
    if(bitRead(receiveData[0],7)){
      bitWrite(receiveData[0],7,0);
      readFlag = true;
      taskFlag = true;
    }
    do{
      switch(receiveData[0]){
        case 1:   //Target Linear Velocity
          if(!taskFlag){  //Write Task
            targetLinVel = (receiveData[1] << 8) + receiveData[2];
          }
          else{           //Read Task
            sendData[0] = 1;
            sendData[1] = highByte(targetLinVel);
            sendData[2] = lowByte(targetLinVel);
            if(readFlag){
              bitWrite(sendData[0],7,1);
              readFlag = false;
            }
          }
          break;
        case 2:   //Target Angular Velocity
          if(!taskFlag){  //Write Task
            targetAngVel = (receiveData[1] << 8) + receiveData[2];
          }
          else{           //Read Task
            sendData[0] = 2;
            sendData[1] = highByte(targetAngVel);
            sendData[2] = lowByte(targetAngVel);
            if(readFlag){
              bitWrite(sendData[0],7,1);
              readFlag = false;
            }
          }
          break;
        case 3:   //Actual Linear Velocity
          if(!taskFlag){  //Write Task - ILLEGAL!!! - ERROR
            sendData[0] = 99;
            sendData[1] = 0;
            sendData[2] = 1;
            skipRead = true;
          }
          else{           //Read Task
            if(!skipRead){  //Not an Error
              sendData[0] = 3;
              sendData[1] = highByte(actualLinVel);
              sendData[2] = lowByte(actualLinVel);
              if(readFlag){
                bitWrite(sendData[0],7,1);
                readFlag = false;
              }
            }
            skipRead = false;
          }
          break;
        case 4:   //Actual Angular Velocity
          if(!taskFlag){    //Write Task - ILLEGAL!! - ERROR
            sendData[0] = 99;
            sendData[1] = 0;
            sendData[2] = 1;
            skipRead = true;
          }
          else{
            if(!skipRead){  //Not an Error
              sendData[0] = 4;
              sendData[1] = highByte(actualAngVel);
              sendData[2] = lowByte(actualAngVel);
              if(readFlag){
                bitWrite(sendData[0],7,1);
                readFlag = false;
              }
            }
            skipRead = false;
          }
          break;
        case 5:   //Battery Temp
          if(!taskFlag){    //Write Task - ILLEGAL!! - ERROR
            sendData[0] = 99;
            sendData[1] = 0;
            sendData[2] = 1;
            skipRead = true;
          }
          else{
            if(!skipRead){  //Not an Error
              sendData[0] = 5;
              sendData[1] = highByte(batteryTemp);
              sendData[2] = lowByte(batteryTemp);
              if(readFlag){
                bitWrite(sendData[0],7,1);
                readFlag = false;
              }
            }
            skipRead = false;
          }
          break;
        case 6:   //Battery Voltage (mV)
          if(!taskFlag){    //Write Task - ILLEGAL!! - ERROR
            sendData[0] = 99;
            sendData[1] = 0;
            sendData[2] = 1;
            skipRead = true;
          }
          else{
            if(!skipRead){  //Not an Error
              sendData[0] = 6;
              sendData[1] = highByte(batteryVolt);
              sendData[2] = lowByte(batteryVolt);
              if(readFlag){
                bitWrite(sendData[0],7,1);
                readFlag = false;
              }
            }
            skipRead = false;
          }
          break;
        default:   //unknown variable
          if(!taskFlag){    //during write request
            sendData[0] = 99;
            sendData[1] = 0;
            sendData[2] = 2;
            taskFlag = true;
          }
          else{             //during read request
            sendData[0] = 99;
            sendData[1] = 0;
            sendData[2] = 3;
            if(readFlag){
              readFlag = false;
            }
          }
          break;
      }
      taskFlag = !taskFlag;
    } while (taskFlag);
  }
}

void requestEvent(){
  Wire.write(sendData,3);
  sendData[0] = 99;
  sendData[1] = 0;
  sendData[2] = 5;
}

