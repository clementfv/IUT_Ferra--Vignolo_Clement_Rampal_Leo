#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "Toolbox.h"
#include "robot.h"
#include "PWM.h"
#include "ADC.h"
#include "main.h"
int ADCValue0;
int ADCValue1;
int ADCValue2;

int main(void) {

    InitOscillator();
    InitIO();
    InitTimer1();
    InitTimer4();
    InitTimer23();
    InitPWM();
    InitADC1();

    //   PWMSetSpeedConsigne(0, MOTEUR_DROIT);
    //  PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);

    LED_BLANCHE_1 = 1;
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 1;
    LED_VERTE_1 = 1;
    // LED_BLANCHE_2 = 1;
    // LED_BLEUE_2 = 1;
    //LED_ORANGE_2 = 1;
    LED_ROUGE_2 = 1;
    LED_VERTE_2 = 1;

    while (1) {
        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [0])* 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
        }

        if (robotState.distanceTelemetreGauche >= 30) {
            LED_BLANCHE_2 = 0;
        } else {
            LED_BLANCHE_2 = 1;
        }
        if (robotState.distanceTelemetreCentre >= 30) {
            LED_BLEUE_2 = 0;
        } else {
            LED_BLEUE_2 = 1;
        }
        if (robotState.distanceTelemetreDroit >= 30) {
            LED_ORANGE_2 = 0;
        } else {
            LED_ORANGE_2 = 1;
        }
    }
}// fin main