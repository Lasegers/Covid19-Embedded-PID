//#include <AS5040.h>
#include "TimerThree.h"

unsigned long timestamp; 
unsigned long time_diff = 5; // try to loop every 5 ms​
#define ENDSWITCH_EXHALE_PIN 2 //Bovenste Switch (Start)
#define ENDSWITCH_INHALE_PIN 3 //Onderste switch (Stop)
unsigned long motor_time;

volatile float CurrentPressurePatient = 0;
volatile bool isPatientPressureCorrect = false;
volatile int END_SWITCH_VALUE_INHALED = 0;
volatile int END_SWITCH_VALUE_EXHALED = 0;
volatile bool isFlow2PatientRead = false;
volatile float Flow2Patient = 0;
volatile int duty_output = 0;

//#define CSpin   10
//#define CLKpin  11
//#define DOpin   12

//AS5040 enc (CLKpin, CSpin, DOpin);

//----------------------------------------------------------------------------------------------------
//-----------------------------------------    BEGIN OF SETUP ----------------------------------------
void setup()
{
  Serial.begin(115200); 
  Serial.println("Starting setup");
  //timestamp = millis();
  //--- set up the pressure sensors here
  Serial.println("Setting up pressure sensors");
  /*while(!BME280_Setup()) // must start, if not, do not continue
  {
    delay(100);  
  }*/
  //--- set up flow sensors here, if init fails, we can continue
  Serial.print("Setting up flow sensor: ");
  if (FLOW_SENSOR_INIT()) Serial.println(" OK");
  else Serial.println(" Failed");
  //--- set up the pins for the end switches
  Serial.println("Setting up endswitches");
  pinMode(ENDSWITCH_INHALE_PIN,INPUT_PULLUP);
  pinMode(ENDSWITCH_EXHALE_PIN,INPUT_PULLUP);
  
  //Serial.println("Setting up encoder");
  //if (!enc.begin ()) Serial.println ("Error setting up AS5040") ; 
  MOTOR_CONTROL_setp();
  // init timer 3 
  //--- start the timer 3 to make sure our control loops runs at a constant interval
  Timer3.initialize(25000);         // initialize timer3 in us, set 25 ms timing
  Timer3.attachInterrupt(controller);  // attaches callback() as a timer overflow interrupt
  //---------------------------------------------------------------------------------------------------------
  // read all sensors here to make sure readings are correct
  Serial.println("Setting desired pressure to 50 cmH2O");
  BREATHE_CONTROL_setPEAKPressure(50);
  BREATHE_CONTROL_setTIDALVolume(450);
  Serial.println("Reading all pressure sensors available");
  Serial.print("Pressure sensor 1 = ");
  Serial.println(BME280_readpressure1_cmH2O());
  Serial.print("Pressure sensor 2 = ");
  Serial.println(BME280_readpressure2_cmH2O());
  Serial.print("Pressure sensor ATM = ");
  Serial.println(BME280_readpressure_ref_cmH2O());  
  Serial.print("Flow sensor = ");
  float flow = 0;
  if (FLOW_SENSOR_Measure(&flow))   Serial.println(flow);
  else Serial.println("Failed");
}
//----------------------------------------------------------------------------------------------------
//-----------------------------------------    END OF SETUP ------------------------------------------
void loop()
{     
  Serial.print("Patientpressure: ");
  Serial.print(CurrentPressurePatient);
  Serial.print(";PatientAirFlow: ");
  Serial.print(Flow2Patient);
  Serial.print(";Inhale SW=");
  Serial.print(END_SWITCH_VALUE_INHALED);
  Serial.print(";Exhale SW=");
  Serial.print(END_SWITCH_VALUE_EXHALED);  
  Serial.print(";PWM value = ");
  Serial.println(duty_output);
  
  //Hall_sensor_position=enc.read ();
  //Serial.println(Hall_sensor_position,DEC);
  //Serial.println (enc.read (), DEC) ;
  //Serial.print ("   ") ;
  // set the desired pressure to the control loop
      
  //long new_time = millis();
  //--- read buttons here
  //int END_SWITCH_VALUE_STOP = digitalRead(ENDSIWTCH_FULL_PIN);
  //int END_SWITCH_VALUE_START = digitalRead(ENDSWITCH_PUSH_PIN);

  /*if (timestamp+time_diff<=new_time)
  {
    if (BME280_readPressurePatient(&CurrentPressurePatient))
    {   
      BREATHE_setCurrentTime(new_time);      
      BREATHE_CONTROL_setInhalePressure(CurrentPressurePatient);
      
      
      float duty = BREATHE_CONTROL_Regulate();  
      
      BREATHE_setToEXHALE(END_SWITCH_VALUE_STOP);
      BREATHE_setToINHALE(END_SWITCH_VALUE_START);

      MOTOR_CONTROL_setValue((int)duty);

      
      /*if (Breathe_mode == INHALE)
      {
        if (duty>225){duty=255;}
        else if (duty<0){duty=0;}
        
        motorController.TurnLeft(duty); //0->255
        /*duty = 122 - Speed;
        if (duty>235)
        {
          duty=235;
        }
        else if (duty<122)
        {
          duty=122;
        }
        analogWrite(PWM_Pin, duty );  
      }
      else
      {
        motorController.TurnRight(duty);
      }
      
      Serial.print("P=");
      Serial.println(CurrentPressurePatient);       
      //Serial.print(";PID=");
      //Serial.print(angle);
      //Serial.print(",bpm=");
      //Serial.println(BREATHE_getBPM());
      //Serial.print(";SW"); 
      //Serial.print(END_SWITCH_VALUE_STOP);
      //Serial.print(";SW2");
      //Serial.println(END_SWITCH_VALUE_START);
      //Serial.println(duty);
    }
    else
    {
      Serial.println("Pressure sensors failing");
    }
    timestamp=new_time;
  }*/  
}
//----------------------------------------------------------------------------------
void controller()
{
  // Handle main control loop  
  // read sensors here, this consumes a lot of time
  END_SWITCH_VALUE_INHALED = digitalRead(ENDSWITCH_INHALE_PIN);
  END_SWITCH_VALUE_EXHALED = digitalRead(ENDSWITCH_EXHALE_PIN);

  isPatientPressureCorrect = BME280_readPressurePatient(&CurrentPressurePatient);
  isFlow2PatientRead = FLOW_SENSOR_Measure(&Flow2Patient);
  
  if (isPatientPressureCorrect)
  {   
    BREATHE_setCurrentTime(millis());      
    BREATHE_CONTROL_setCurrentPressure(CurrentPressurePatient);      
      
    duty_output = BREATHE_CONTROL_Regulate();  
      
    BREATHE_setToEXHALE(END_SWITCH_VALUE_INHALED);
    BREATHE_setToINHALE(END_SWITCH_VALUE_EXHALED);

    MOTOR_CONTROL_setValue(duty_output);
  }
}
//----------------------------------------------------------------------------------
