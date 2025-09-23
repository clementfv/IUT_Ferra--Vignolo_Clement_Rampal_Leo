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

int main(void) {

    InitOscillator();
    InitIO();
    InitTimer1();
    InitTimer23();
    InitPWM();
    InitADC1();
    int ADCValue0;
    int ADCValue1;
    int ADCValue2;
 //   PWMSetSpeedConsigne(0, MOTEUR_DROIT);
 //  PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);

    LED_BLANCHE_1 = 1;
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 1;
    LED_VERTE_1 = 1;                                                
    LED_BLANCHE_2 = 1;
    LED_BLEUE_2 = 1;
    LED_ORANGE_2 = 1;
    LED_ROUGE_2 = 1;
    LED_VERTE_2 = 1;
    
    while (1) {
        if(ADCIsConversionFinished()){
            unsigned int * result = ADCGetResult();
           ADCValue0 = ADCGetResult(0); 
           ADCValue1 = ADCGetResult(1);
           ADCValue2 = ADCGetResult(2);
           ADCClearConversionFinishedFlag();
        }
    } // fin main
}