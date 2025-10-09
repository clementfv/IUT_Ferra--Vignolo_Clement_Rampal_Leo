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
int ADCValue3;
int ADCValue4;

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
   // OperatingSystemLoop();

    //LED_BLANCHE_1 = 1;
   // LED_BLEUE_1 = 1;
  //  LED_ORANGE_1 = 1;
   // LED_ROUGE_1 = 1;
   // LED_VERTE_1 = 1;
    LED_BLANCHE_2 = 1;
    LED_BLEUE_2 = 1;
    LED_ORANGE_2 = 1;
    LED_ROUGE_2 = 1;
    LED_VERTE_2 = 1;

    while (1) {
        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            volts = ((float) result [4])* 3.3 / 4096;
            robotState.distanceTelemetreUltraDroit = 34 / volts - 5;
            volts = ((float) result [0])* 3.3 / 4096;
            robotState.distanceTelemetreUltraGauche = 34 / volts - 5;
        }

        if (robotState.distanceTelemetreGauche >= 30) {
            LED_BLEUE_1 = 0;
        } else {
            LED_BLEUE_1 = 1;
        }
        if (robotState.distanceTelemetreCentre >= 25) {
            LED_ORANGE_1 = 0;
        } else {
            LED_ORANGE_1 = 1;
        }
        if (robotState.distanceTelemetreDroit >= 30) {
            LED_ROUGE_1 = 0;
        } else {
             LED_ROUGE_1 = 1;
        }
        if (robotState.distanceTelemetreUltraDroit >= 30) {
            LED_VERTE_1 = 0;
        } else {
            LED_VERTE_1 = 1;
        }
        if (robotState.distanceTelemetreUltraGauche >= 30) {
            LED_BLANCHE_1 = 0;
        } else {
            LED_BLANCHE_1 = 1;
        }
    }
}// fin main

unsigned char stateRobot;

    void OperatingSystemLoop(void) {
        switch (stateRobot) {
            case STATE_ATTENTE:
                timestamp = 0;
                PWMSetSpeedConsigne(0, MOTEUR_DROIT);
                PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
                stateRobot = STATE_ATTENTE_EN_COURS;
            case STATE_ATTENTE_EN_COURS:
                if (timestamp > 1000)
                    stateRobot = STATE_AVANCE;
                break;
            case STATE_AVANCE:
                PWMSetSpeedConsigne(30, MOTEUR_DROIT);
                PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
                stateRobot = STATE_AVANCE_EN_COURS;
                break;
                case STATE_RALENTIR:
                PWMSetSpeedConsigne(10, MOTEUR_DROIT);
                PWMSetSpeedConsigne(10, MOTEUR_GAUCHE);
                stateRobot = STATE_AVANCE;
                break;
            case STATE_AVANCE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;
            case STATE_TOURNE_GAUCHE:
                PWMSetSpeedConsigne(15, MOTEUR_DROIT);
                PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
                break;
                case STATE_TOURNE_GAUCHE_LOIN:
                PWMSetSpeedConsigne(10, MOTEUR_DROIT);
                PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
                break;
            case STATE_TOURNE_GAUCHE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;
            case STATE_TOURNE_DROITE:
                PWMSetSpeedConsigne(0, MOTEUR_DROIT);
                PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_DROITE_EN_COURS;
                break;
                case STATE_TOURNE_DROITE_LOIN:
                PWMSetSpeedConsigne(0, MOTEUR_DROIT);
                PWMSetSpeedConsigne(10, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_DROITE_EN_COURS;
                break;
            case STATE_TOURNE_DROITE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;
            case STATE_TOURNE_SUR_PLACE_GAUCHE:
                PWMSetSpeedConsigne(15, MOTEUR_DROIT);
                PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
                break;
            case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;
            case STATE_TOURNE_SUR_PLACE_DROITE:
                PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
                PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
                break;
            case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;
            default:
                stateRobot = STATE_ATTENTE;
                break;
        }
    }
    unsigned char nextStateRobot = 0;

    void SetNextRobotStateInAutomaticMode() {
        unsigned char positionObstacle = PAS_D_OBSTACLE;
        if (robotState.distanceTelemetreDroit < 30 &&
                robotState.distanceTelemetreCentre > 25 &&
                robotState.distanceTelemetreGauche > 30) //Obstacle àdroite
            positionObstacle = OBSTACLE_A_DROITE;
        else if (robotState.distanceTelemetreDroit > 30 &&
                robotState.distanceTelemetreCentre > 25 &&
                robotState.distanceTelemetreGauche < 30) //Obstacle àgauche
            positionObstacle = OBSTACLE_A_GAUCHE;
        else if (robotState.distanceTelemetreCentre < 25) //Obstacle en face
            positionObstacle = OBSTACLE_EN_FACE;
        else if (robotState.distanceTelemetreDroit > 30 &&
                robotState.distanceTelemetreCentre > 25 &&
                robotState.distanceTelemetreGauche > 30) //pas d?obstacle
            positionObstacle = PAS_D_OBSTACLE;
        else if (robotState.distanceTelemetreCentre > 25 &&
                robotState.distanceTelemetreUltraGauche < robotState.distanceTelemetreUltraDroit) //Obstacle àgauche
            positionObstacle = OBSTACLE_A_GAUCHE_LOIN;
        else if (robotState.distanceTelemetreUltraDroit < robotState.distanceTelemetreUltraGauche)
            positionObstacle = OBSTACLE_A_DROITE_LOIN;
        else if ( (robotState.distanceTelemetreUltraDroit - robotState.distanceTelemetreUltraGauche)<10 || (robotState.distanceTelemetreUltraGauche - robotState.distanceTelemetreUltraDroit)<10 && robotState.distanceTelemetreCentre > 25) 
            positionObstacle = PAS_D_OBSTACLE;
         else if ( (robotState.distanceTelemetreUltraDroit || robotState.distanceTelemetreUltraGauche || robotState.distanceTelemetreCentre)<10 ) 
            positionObstacle = OBSTACLE_AUTOUR;
        //éDtermination de lé?tat àvenir du robot
        if (positionObstacle == PAS_D_OBSTACLE)
            nextStateRobot = STATE_AVANCE;
        else if (positionObstacle == OBSTACLE_A_DROITE)
            nextStateRobot = STATE_TOURNE_GAUCHE;
        else if (positionObstacle == OBSTACLE_A_GAUCHE)
            nextStateRobot = STATE_TOURNE_DROITE;
        else if (positionObstacle == OBSTACLE_EN_FACE)
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        else if (positionObstacle == OBSTACLE_A_GAUCHE_LOIN)
            nextStateRobot = STATE_TOURNE_DROITE_LOIN;
        else if (positionObstacle == OBSTACLE_A_DROITE_LOIN)
            nextStateRobot = STATE_TOURNE_GAUCHE_LOIN;
        else if (positionObstacle == OBSTACLE_AUTOUR)
            nextStateRobot = STATE_RALENTIR;
        //Si l?on n?est pas dans la transition de lé?tape en cours
        if (nextStateRobot != stateRobot - 1)
            stateRobot = nextStateRobot;
    }
