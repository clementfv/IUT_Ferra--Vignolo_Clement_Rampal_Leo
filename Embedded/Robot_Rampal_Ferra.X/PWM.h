#define MOTEUR_DROIT 0
#define MOTEUR_GAUCHE 1
#ifndef	PWM_H
 void PWMSetSpeed(float vitesseEnPourcents, float moteur);
 void InitPWM(void);
#endif