#include "Motor.h"

#include "mbed.h"
// Die Richtung wird durch - oder + Zeichnen bei der Geschwindigkeit eingestellt speed(-oder+ Wert)
Motor::Motor(PinName pwm, PinName fwd, PinName rev):
    _pwm(pwm), _fwd(fwd), _rev(rev) {

    // Set initial condition of PWM

    Time = 2000000;    // Zeit der Periode initialisieren in us 
     _pwm.period_us(Time);
     _pwm = 0;

    // Initial condition of output enables
    _fwd = 0;
    _rev = 0;
}
// Speed : float speed ist 0.5 Teilung jeder Pulse in zwei Teilen mit Werte 0 oder 1/ Teilung der Pulsperiode ist 50% oder 25% ....
void Motor::speed(float speed) {
    _pwm.period_us(Time);
    _fwd = (speed > 0.0);
    _rev = (speed < 0.0);
    _pwm = abs(speed);
       
}



