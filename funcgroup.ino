#include "funcgroup.h"
#include <SoftwareSerial.h>

void FlipFlopLed()
{
  if( HIGH == digitalRead(PN_LED_ADDITIONAL) ) {
        digitalWrite(PN_LED_ADDITIONAL,LOW);
        digitalWrite(LED_BUILTIN, HIGH);  
  }
  else { 
        digitalWrite(PN_LED_ADDITIONAL, HIGH);  
        digitalWrite(LED_BUILTIN, LOW);  
  }
  
}
//---------------------------------------------------------
bool IsAtCommand(const String& s)
{
  const String sAtPluse="AT+";

  if( s.equals("AT") ) { 
    return true; 
  }

  if( 0 == s.indexOf("AT+") ) { //AT+ or AT+NAME.
    return true; 
  }

  return false; 
}

//-------------------------------------------------------------
float GetCentimeterUltrasonic(const Ultrasonic& ult, const bool& isSerialPrint)
{
  long ultTiming = ultra.timing();  // ultra Object
  float Cm = ultra.convert(ultTiming, Ultrasonic::CM); // 計算距離，單位: 公分
  //float In = ultra.convert(ultTiming, Ultrasonic::IN); // 計算距離，單位: 英吋

  if( isSerialPrint )  {
    Serial.print("Timing: ");
    Serial.print(ultTiming);
    Serial.print("\tCm: ");
    Serial.print(Cm);
    //Serial.print("\tIn: ");
    //Serial.print(In);
    Serial.print("\n");
  //delay(300);
  }

  return Cm;
}
//-----------------------
bool UltraCentimeterMayActiveMotor(const Ultrasonic& ultra, const bool& isWriteSerial )
{
    long ultTiming = ultra.timing();  // ultra Object
    float calCentimeter = ultra.convert(ultTiming, Ultrasonic::CM); // 計算距離，單位: 公分
    if( isWriteSerial ) { 
      if( 0.0 != calCentimeter ){ 
        Serial.println(String("Centimeter=")+String(calCentimeter));
        delay(1000);
      }
    }

    if( 0 == calCentimeter ) { // POWER OFF, Voltage=0 
      return false; //MOTOR INACTIVE    
    }  
    
    static int staPreviousDistance=1024; // cm
    if( calCentimeter > ACTIVE_DISTANCE || staPreviousDistance > ACTIVE_DISTANCE) { 
      staPreviousDistance=calCentimeter;  //not yet.
      return false; //inactive
    }
  
    // ACTIVE MOTOR 
    staPreviousDistance=1024;
    //Serial.println("ActiveByUltrasonic");
    ServoMotorControl();
    bluetoothSerial.println("Mousetrap:Got It!");
    delay(100);
    
    return true; //active
}
//---------------------------

