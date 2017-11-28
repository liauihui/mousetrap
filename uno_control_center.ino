#include <SoftwareSerial.h>
#include <Servo.h>
#include <Wire.h>
#include "Ultrasonic.h"
#include "funcgroup.h"

//IDE 序列視窗下腳 e (沒有行結尾) 選項
//bluetoothSerial 配對密碼:1234
//HC-06
//AT               // OK
//AT+NAMEJOY2      // Name=JOY2
//AT+VERSION       // panfpunr
//AT+BAUD4         // Baud=9600
//AT+BAUD8         // Baud=115200
//AT+ROLE          // not support
//AT+ADDR          // not support
//AT+PSWD          // not support
//AT+UART          // not support

const int UnoRx = 2;            //bluetoothSerialTx
const int UnoTx = 3;            //bluetoothSerialRx

const int pnUltraSndTrig = 4; // 發送 40KHz 超聲波給物體
const int pnUltraSndEcho = 5; // 接收 trig 碰撞物體反射回來的超聲波
const int pnServoMotorCtrl = 6;
const int pnGotItAlert = 7;
const int PN_LED_ADDITIONAL = 8;
const int pnBuzzer = 9;
const int ACTIVE_DISTANCE=12; // MouseTrap ActiveServo 

// '0': LED OFF
// '1': LED ON
// 'A': ACTIVE SERVO MOTOR

Ultrasonic ultra(pnUltraSndTrig, pnUltraSndEcho);
SoftwareSerial bluetoothSerial(UnoRx, UnoTx);
Servo servoMotor;

void setup() {
  Serial.begin(9600);
  bluetoothSerial.begin(9600);

  pinMode(PN_LED_ADDITIONAL, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pnBuzzer, OUTPUT);
  
  pinMode(pnGotItAlert, INPUT);
  digitalWrite(pnGotItAlert, LOW);

  servoMotor.attach(pnServoMotorCtrl);
  servoMotor.write(0); //Position Zero
}
//-----------------------
void loop() {
  static bool isSplashLed=false;   
 
  //Serial.println("loop....");
  //delay(1000);

  int iSize = 0;
  if ( (iSize = bluetoothSerial.available()) <= 0 ) {
    //Serial.println(String("isSplashLed=")+String(isSplashLed));

    if( isSplashLed ){ 
      FlipFlopLed();
      delay(250);
    }

    long ultTiming = ultra.timing();  // ultra Object
    float calCentimeter = ultra.convert(ultTiming, Ultrasonic::CM); // 計算距離，單位: 公分
    Serial.println(String("Centimeter=")+String(calCentimeter));
    delay(1000);

    static int staPreviousDistance=1024; // cm
    if( calCentimeter >= ACTIVE_DISTANCE || staPreviousDistance >= ACTIVE_DISTANCE) { 
      staPreviousDistance=calCentimeter;  //not yet.
    }
    else { // ACTIVE
      staPreviousDistance=1024;
      //Serial.println("ActiveByUltrasonic");
      ServoMotorControl();
      bluetoothSerial.println("Mousetrap:Got It!");
      delay(100);
    }
   
    return; // reLoop
  }

  String sCtrlMsg = "";
  for ( int x = 0; x < iSize; x++ ) {
    char c = bluetoothSerial.read();
    sCtrlMsg = sCtrlMsg + String(c);
  }
  Serial.println(sCtrlMsg);

  if( sCtrlMsg.equalsIgnoreCase("1") ){ 
    isSplashLed=true;
  }
  
  if( sCtrlMsg.equalsIgnoreCase("0") ){ 
    isSplashLed=false;
  }

  if ( sCtrlMsg.equalsIgnoreCase("A") ) { //ACTIVE SERVO MOTOR
    //Serial.println("ActiveByPhone");
    ServoMotorControl();
    bluetoothSerial.println("Servo motor active thru phone.");
    delay(100);
    return;
  }
 
}//LOOP
//------------------------------------------------------------
void ServoMotorControl()
{
  for ( int i = 0; i <= 180; i++) {
    servoMotor.write(i);
    delay(10);
  }

  for ( int j = 180; j >= 0; j--) {
    servoMotor.write(j);
    delay(10);
  }

}
//-----------------------------------

