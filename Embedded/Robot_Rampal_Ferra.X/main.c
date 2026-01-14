#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <libpic30.h>

#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "Toolbox.h"
#include "robot.h"
#include "PWM.h"
#include "ADC.h"
#include "main.h"
#include "UART.h"
#include "CB_TX1.h"
#include "CB_RX1.h"
#include "UART_Protocol.h"

int ADCValue0;
int ADCValue1;
int ADCValue2;
int ADCValue3;
int ADCValue4;

/* Payload UART */
unsigned char payload[] = { 'B', 'o', 'n', 'j', 'o', 'u', 'r' };

unsigned char stateRobot;
unsigned char nextStateRobot = 0;

int main(void) {

    InitOscillator();
    InitIO();
    InitTimer1();
    InitTimer4();
    InitTimer23();
    InitPWM();
    InitADC1();
    InitUART();

    LED_BLANCHE_2 = 1;
    LED_BLEUE_2   = 1;
    LED_ORANGE_2  = 1;
    LED_ROUGE_2   = 1;
    LED_VERTE_2   = 1;

    while (1) {

        /* ===== ADC ===== */
        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();

            float volts = ((float) result[1]) * 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;

            volts = ((float) result[2]) * 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;

            volts = ((float) result[3]) * 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;

            volts = ((float) result[4]) * 3.3 / 4096;
            robotState.distanceTelemetreUltraDroit = 34 / volts - 5;

            volts = ((float) result[0]) * 3.3 / 4096;
            robotState.distanceTelemetreUltraGauche = 34 / volts - 5;
        }

        /* ===== ENVOI UART ===== */
        UartEncodeAndSendMessage(
            0x0080,
            sizeof(payload),
            payload
        );

        __delay32(40000000);

        /* ===== ECHO RX ===== */
        int i;
        for (i = 0; i < CB_RX1_GetDataSize(); i++) {
            unsigned char c = CB_RX1_Get();
            SendMessage(&c, 1);
        }

        __delay32(10);

        /* ===== LEDS ===== */
        LED_BLEUE_1   = (robotState.distanceTelemetreGauche < 30);
        LED_ORANGE_1 = (robotState.distanceTelemetreCentre < 25);
        LED_ROUGE_1  = (robotState.distanceTelemetreDroit < 30);
        LED_VERTE_1  = (robotState.distanceTelemetreUltraDroit < 30);
        LED_BLANCHE_1= (robotState.distanceTelemetreUltraGauche < 30);
    }
}

/* ================= SYSTEME D?ETATS ROBOT ================= */

void OperatingSystemLoop(void) {
    switch (stateRobot) {

        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
            break;

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

void SetNextRobotStateInAutomaticMode() {

    unsigned char positionObstacle = PAS_D_OBSTACLE;

    if (robotState.distanceTelemetreDroit < 45 &&
        robotState.distanceTelemetreCentre > 40 &&
        robotState.distanceTelemetreGauche > 45)
        positionObstacle = OBSTACLE_A_DROITE;

    else if (robotState.distanceTelemetreDroit > 45 &&
             robotState.distanceTelemetreCentre > 40 &&
             robotState.distanceTelemetreGauche < 45)
        positionObstacle = OBSTACLE_A_GAUCHE;

    else if (robotState.distanceTelemetreCentre < 35)
        positionObstacle = OBSTACLE_EN_FACE;

    else if (robotState.distanceTelemetreUltraGauche <
             robotState.distanceTelemetreUltraDroit)
        positionObstacle = OBSTACLE_A_GAUCHE_LOIN;

    else if (robotState.distanceTelemetreUltraDroit <
             robotState.distanceTelemetreUltraGauche)
        positionObstacle = OBSTACLE_A_DROITE_LOIN;

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

    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
}
