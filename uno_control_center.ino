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

const boolean IS_ULTRASONIC_WRITE_SERIAL_MSG=true;  

const int UnoRx = 2;              // bluetoothSerialTx
const int UnoTx = 3;              // bluetoothSerialRx
const int pnUltraSndTrig = 4;     // 發送 40KHz 超聲波給物體
const int pnUltraSndEcho = 5;     // 接收 trig 碰撞物體反射回來的超聲波
const int pnServoMotorCtrl = 6;   //
const int pnGotItAlert = 7;       // Reserved
const int PN_LED_ADDITIONAL = 8;  // LED onboard
const int pnBuzzer = 9;
const int ACTIVE_DISTANCE = 36;   //cm, MouseTrap ActiveServo

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
bool AtCommandProcess(const int& iSerialSize)
{
  //Serial.println("Got SerialSize");
  String sAtc ="";
  char cCum=0x00;
  for( int r = 0;  r < iSerialSize; r++ ) {
    cCum=Serial.read(); // ERROR:sAtc=sAtc+String( Serial.read() );
    sAtc=sAtc+String( cCum );  
  }
  Serial.println(sAtc);
  if( IsAtCommand(sAtc) ){ 
    //Serial.println("IsAtCommand:true");
    bluetoothSerial.print(sAtc); // print but NOT printLn
    return true; 
  }
    
  return false; 
}
//----------------------------------------------
void loop() {
  static bool isSplashLed = false;
  //Serial.println("loop....");

  int iSerialSize = Serial.available();
  if( iSerialSize > 0 ) { // ATCommand From SerialWindow?
    AtCommandProcess(iSerialSize);
    return;  // anyway
  }

  int iSize = 0;
  if ( (iSize = bluetoothSerial.available()) <= 0 ) { // BtSerial<0
    //Serial.println(String("isSplashLed=")+String(isSplashLed));

    if ( isSplashLed ) {
      FlipFlopLed();
      delay(100);
    }

    UltraCentimeterMayActiveMotor(ultra, IS_ULTRASONIC_WRITE_SERIAL_MSG); // boolean:IsWriteSerial

    return; // return Loop(), reloop
  }// BtSerial<0


  // Bluetooth data coming, BT available
  String sCtrlMsg = "";
  for ( int x = 0; x < iSize; x++ ) {
    char c = bluetoothSerial.read();
    sCtrlMsg = sCtrlMsg + String(c);
  }
  Serial.println(sCtrlMsg);

  if ( sCtrlMsg.equalsIgnoreCase("1") ) { //LED ON
    isSplashLed = true;
    bluetoothSerial.println("LED START");
  }

  if ( sCtrlMsg.equalsIgnoreCase("0") ) { // LED OFF
    isSplashLed = false;
    bluetoothSerial.println("LED STOP.");
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

