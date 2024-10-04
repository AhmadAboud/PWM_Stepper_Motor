#include <mbed.h>
#include "stm32h723xx.h"
#include "stm32h7xx.h"
#include "stm32h7xx_hal_tim_ex.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h" 
#include "system_stm32h7xx.h"

#include "unity.h"
#include "time.h"
#include "Timer.h"
#include "Motor.h"
#define GATE_TIME 1.0f

DigitalOut osc1(PB_4);
DigitalOut osc2(PC_9);

double Zeitposition_Ramp;
int PRESCALE;

DigitalOut led(LED1);
DigitalOut STEP_PIN(PA_9);
DigitalOut DIR_PIN(D14);     // vorher d8
DigitalIn _LichtSchrankenSensor(D6);   // Wenn Lichtschrankensensor 1, soll die Weiche an.
float LichtSchrankenSensor1;
float LichtSchrankenSensor2;
DigitalIn _SensorStart(D5);
DigitalIn _SensorStop(D4);
DigitalOut led2(LED2);

int RingTrgState = 0;
int CheckTrgRingState();
int CheckStopSensor();
void DriveMotor();

double angel_Alfa;
int Pos_Step;
InterruptIn Event_T1(D2);

double Soll_Gesch_HZ=0;           // soll Geschwindigkeit in hz
double Rampenzeit_MS=70;          // Rampenzeit in ms 
double RampenPos_ZeitPunkt =1;    // Rampenposition Zeit Punkt
double IstGeschw_HZ=0;            // Istgeschwindigkeit 

uint32_t time_value;

Motor m(PB_8, D14, D7);         // Hier D14 zu D8
float Freq;

//RampenZeit
double RampenZeit(int i)
{
  Zeitposition_Ramp = (1/(Freq*sin(i*3.14159/20)*sin(i*3.14159/20)))*1000000;                                   // Zeitposition innerhalb der Rampe in ms
   return Zeitposition_Ramp;
}

// Time Value als uint32_t saving
uint32_t time_Value;

int main() {
 
   // RCC_GetClocksFreq(&amp;RCC_Clock); 
    DIR_PIN = 0;
    m._fwd=0;  
    led=0;
    int i=1;

    while (1)
   {
   
   RingTrgState = 0;
   RingTrgState = CheckTrgRingState();
      
      if(RingTrgState)
       {
        if(CheckStopSensor()){
          DriveMotor();
        }        
    }
  } 
}

void DriveMotor()
{ int i;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::   
//_______________________________________________________________
double RampZeit = 1000; // Rampenzeit in ms
double Ist_Gesch_HZ = 0; // IST-Geschwindigkeit der Rampe
double Soll_Gesch_HZ =3000; // Soll Geschwindigkeit in HZ
int state = 0;
int starter =0;
int Zyklus = 1;
//______________________________________________________________

i=1;
Freq =600;
while (10-i)
{
m.Time = RampenZeit(i);
m.speed(+0.5);
ThisThread::sleep_for(28ms);
i++;
}

i=10;
while (i-1)
{
m.Time = RampenZeit(i);
m.speed(+0.5);
ThisThread::sleep_for(28ms);
i--;
}

m.speed(0);
ThisThread::sleep_for(200ms);

i=1;
// langsammer bei 400 Hz 360 grad/s   Hier muss seep_for(50ms) gewartet.
// 800 HZ ist die Geschwindigkeit 720 grad/s. In diesem Fall muss seep_for(25ms) gewartet.
Freq =600;

while (10-i)
{
m.Time =RampenZeit(i);
m.speed(-0.5);
ThisThread::sleep_for(28ms);
i++;
}

i=10;
while (i-1)
{
m.Time =RampenZeit(i);
m.speed(-0.5);
ThisThread::sleep_for(28ms);
i--;
}

m.speed(0);
ThisThread::sleep_for(200ms);

}

int CheckTrgRingState(){
// Prüfen ob der RingState ein oder aus (Flasche ist ok oder nicht)
// int i = _LichtSchrankenSensor.read();

_LichtSchrankenSensor.mode(PullUp);
_SensorStart.mode(PullUp);

if ((_LichtSchrankenSensor.read()>0.2)&&(_SensorStart.read()>0.2))
{
    RingTrgState = 1;
    led2 = 1;
} else {
    RingTrgState = 0;  
    led2 = 0;
}
return RingTrgState;
}

int CheckStopSensor()
{
  int value;
  _SensorStop.mode(PullUp);
  if(_SensorStop.read()>0.2)
  {
    led=1;
    value =1;
  } else {
    led=0;
    value=0;
  }
   return value;
}


//:::::::::::::::: Optokoppler und Lichtschrankensensor
// https://www.bing.com/videos/search?q=bucck+817+4+v1.0&view=detail&mid=29693F12CFA00E416F7F29693F12CFA00E416F7F&FORM=VIRE
// Lichtschrankensensor gibt 24 Voult, er kann nicht direkt zum Nucleo verbunden wird.
// https://www.bing.com/videos/search?q=bucck+817+4+v1.0&view=detail&mid=29693F12CFA00E416F7F29693F12CFA00E416F7F&FORM=VIRE
// https://www.youtube.com/watch?v=OYK1-t_oI6k&ab_channel=paoloaliverti
// Pullup/Pulldown: https://www.youtube.com/watch?v=GSU_UDfB0IE&ab_channel=HamedAdefuwa
// Nucleo Widerstand statt Externale Widerstand
