#define MOTEUR_DROIT 0
#define MOTEUR_GAUCHE 1
#ifndef	PWM_H
 // void PWMSetSpeed(float vitesseEnPourcents, float moteur);
 void InitPWM(void);
 void PWMUpdateSpeed();
 void PWMSetSpeedConsigne(float vitesseEnPourcents, float moteur);
#endif