#ifndef BREATHE_CONTROL_H
#define BREATHE_CONTROL_H

// update very 40ms
float PEAK_PRESSURE_SETPOINT = 50;            // pressure in cmH2O
float TIDAL_VOLUME_SETPOINT = 600;            // volume in ml;
float BREATHING_SENSIVITY_TRIGGER = -3;       // trigger in cmH2O
float BREATHING_RESPIRATION_RATIO = 10;       // in # per second

float current_inhale_pressure = 0;
float Vlung = 0;
bool is_blocking = false;

float MAX_DISPLACEMENT=500;
float offset = 250;
int exhale_speed = 50;

unsigned long current_time = 0;
unsigned long previous_exhale_time = 0;
float bpm = 0;

//----------------------------------
float Kp = 10;
float Ki = 0.0;
float Kd = 0.01;
float r = 0.5;  // forgetting factor for D-action
//-----------------------------------
float error = 0; 
float error_t_m_1 = 0;
float dError = 0;
float dError_t_m_1 = 0;
float dError_unfiltered = 0;

//----------------------------------
float PID_value = 0;
float PID_value_P = 0;
float PID_value_I = 0;
float PID_value_D = 0;
//----------------------------------
float PLUNGER_POSITION = 0;

enum BREATHE_PHASE{INHALE, EXHALE, BLOCK}; 
BREATHE_PHASE Breathe_mode = INHALE;

int EXHALE_TIME = 0;
//------------------------------------------------------------------------------
void BREATHE_CONTROL_setPEAKPressure(float setting)
{
  PEAK_PRESSURE_SETPOINT = setting;
}
//------------------------------------------------------------------------------
float BREATHE_CONTROL_getPEAKPressure()
{
  return PEAK_PRESSURE_SETPOINT;
}
//------------------------------------------------------------------------------
void BREATHE_CONTROL_setTIDALVolume(float setting)
{
  TIDAL_VOLUME_SETPOINT = setting;
}
//------------------------------------------------------------------------------
float BREATHE_CONTROL_getTIDALVolume()
{
  return TIDAL_VOLUME_SETPOINT;
}
//------------------------------------------------------------------------------
void BREATHE_CONTROL_setBreathingSensitivity(float setting)
{
  BREATHING_SENSIVITY_TRIGGER = setting;
}
//------------------------------------------------------------------------------
float BREATHE_CONTROL_getBreathingSensitivity()
{
  return BREATHING_SENSIVITY_TRIGGER;
}
//------------------------------------------------------------------------------
void BREATHE_CONTROL_setRespirationRatio(float setting)
{
  BREATHING_RESPIRATION_RATIO = setting;
}
//------------------------------------------------------------------------------
float BREATHE_CONTROL_getRespirationRatio()
{
  return BREATHING_RESPIRATION_RATIO;
}
//------------------------------------------------------------------------------
void BREATHE_CONTROL_setCurrentPressure(float setting)
{
  current_inhale_pressure = setting;
}
//------------------------------------------------------------------------------
float BREATHE_CONTROL_getInhalePressure()
{
  return current_inhale_pressure;
}
//------------------------------------------------------------------------------
float BREATHE_getPID()
{
  return PID_value;  
}
//------------------------------------------------------------------------------
void BREATHE_setCurrentTime(unsigned long t)
{
  current_time = t;
}
//------------------------------------------------------------------------------
float BREATHE_getBPM()
{
  return bpm;
}
//------------------------------------------------------------------------------
void BREATHE_setToEXHALE(int end_switch)
{
  if ((end_switch==1)&&(Breathe_mode==INHALE))
  {
    PID_value_I=0;
    PID_value_P=0;
    Breathe_mode=EXHALE; 
    //-- compute exhale time
    unsigned long time_diff = current_time-previous_exhale_time;
    bpm = 60000.0/time_diff;
    previous_exhale_time = current_time;
  } 
}
//------------------------------------------------------------------------------
void BREATHE_setToINHALE(int end_switch)
{  
    if (end_switch==1)
    {
      Breathe_mode=INHALE;   
    }    
}
//------------------------------------------------------------------------------
int BREATHE_CONTROL_Regulate()
{
    float error = current_inhale_pressure-PEAK_PRESSURE_SETPOINT;
    if (Breathe_mode==INHALE)
    {
      dError = r*dError_t_m_1 + 1000.0*(1-r)*(error-error_t_m_1)/time_diff;
      dError_unfiltered =  1000.0*(error-error_t_m_1)/time_diff;
      PID_value_D = Kd*-dError;
      
      dError_t_m_1 = dError;
      error_t_m_1 = error;
      
      PID_value_P = Kp*error; 
      PID_value_I += Ki*error;
      PID_value_I = (PID_value_I>offset)?offset:PID_value_I;
      PID_value_I = (PID_value_I<-offset)?-offset:PID_value_I;
      PID_value = PID_value_P + PID_value_I + PID_value_D;
   
      if (PID_value>0) PID_value=0;
      return (int)PID_value;
    }
    else if (Breathe_mode==EXHALE)
    {       
      PID_value_I=0;
      PID_value_P=0;    
      PID_value_D=0; 
      return exhale_speed;
    }    
}
//------------------------------------------------------------------------------
#endif
