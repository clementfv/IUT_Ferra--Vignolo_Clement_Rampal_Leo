#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "Robot.h"
#include "main.h"
#include "UART.h"
#include "CB_TX1.h"
#include "CB_RX1.h"
#include "libpic30.h"
#include "UART_Protocol.h"
#include "QEI.h"
#include "ToolBox.h"
#include "Utilities.h"
//
int Capteur, etape;
//unsigned char payload[] = {'B', 'o', 'n', 'j', 'o', 'u', 'r'};
//unsigned char payload[3];
//unsigned char payload[]={0,0,0};

 float payload = 45.55;
 char buffer[4];
int counter = 0;

int main(void) {
    InitOscillator();
    InitIO();
    InitADC1();
    InitPWM();
    InitTimer1();
    InitTimer4();
    InitUART();
    InitTimer23();
    InitQEI1();
    InitQEI2();


    EN_PWM=1;



    while (1) {
        //***************QEI****************
        // SendMessageDirect((unsigned char*) "Bonjour", 7);
       //getBytesFromFloat(buffer,0,payload);
       
//        UartEncodeAndSendMessage(0x67, 8,  buffer );
//        __delay32(40000);
            
    

        //*******************************************TP3


        //****************************************
        /*
                 //Pas de IsdataAvailable sinon explosion
                    for (int i = 0; i < CB_RX1_GetDataSize(); i++) {
                         unsigned char c = CB_RX1_Get();
                         UartDecodeMessage(c); //D�codage foireux
                        // SendMessage(&c,1);
                
                    }*/
        int i;
        for (i = 0; i < CB_RX1_GetDataSize(); i++) {
            unsigned char c = CB_RX1_Get();
            SendMessage(&c, 1);
        }
   


        if (ADCIsConversionFinished()) { //fin de l'aquisition du convertisseur
            ADCClearConversionFinishedFlag(); // on enleve le flag pour pouvoir faire une nouvelle conversion
            unsigned int * result = ADCGetResult(); //cr�ation d'un pointeur pour r�cup�rer les donn�es 
            float volts = ((float) result [0])* 3.3 / 4096; //     n   olt
            robotState.distanceTelemetreGauche1 = 34 / volts - 5; //volt to distance
            volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5; //volt to distance
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5; // +  = �2
            volts = ((float) result [4])* 3.3 / 4096;
            robotState.distanceTelemetreDroit1 = 34 / volts - 5;


            
                        if (counter++ % 50 == 0) {
                            // UartEncodeAndSendMessage(0x80, 7, payload);
                            unsigned char payload2[3];


                            payload2[0] = robotState.distanceTelemetreGauche;
                            payload2[1] = robotState.distanceTelemetreCentre;
                            payload2[2] = robotState.distanceTelemetreDroit;




                            UartEncodeAndSendMessage(0x30, 3, payload2);
                        }  

            if (robotState.distanceTelemetreGauche1 < 30) {
                LED_BLANCHE_1 = 1;
                Capteur = Capteur | 0x10;
            } else {
                LED_BLANCHE_1 = 0;
                Capteur = Capteur & 0b01111;
            }
            if (robotState.distanceTelemetreGauche < 32) {
                LED_BLEUE_1 = 1;
                Capteur = Capteur | 0b01000;
            } else {
                LED_BLEUE_1 = 0;
                Capteur = Capteur & 0b10111;
            }
            if (robotState.distanceTelemetreCentre < 40) {
                LED_ORANGE_1 = 1;
                Capteur = Capteur | 0b00100;
            } else {
                LED_ORANGE_1 = 0;
                Capteur = Capteur & 0b11011;
            }
            if (robotState.distanceTelemetreDroit < 32) {
                LED_ROUGE_1 = 1;
                Capteur = Capteur | 0b00010;
            } else {
                LED_ROUGE_1 = 0;
                Capteur = Capteur & 0b11101;
            }
            if (robotState.distanceTelemetreDroit1 < 30) {
                LED_VERTE_1 = 1;
                Capteur = Capteur | 0b00001;
            } else {
                LED_VERTE_1 = 0;
                Capteur = Capteur & 0b11110;
            }
            if (robotState.distanceTelemetreCentre > 35 && robotState.distanceTelemetreGauche > 20 && robotState.distanceTelemetreDroit > 20 && robotState.distanceTelemetreDroit1 > 20 && robotState.distanceTelemetreGauche1 > 20) {
                Capteur = 0b10001;
            }
        }
    }

}


unsigned char stateRobot = 0;

void CompteurEtape(void) {


    unsigned char payload3[5];
    payload3[0] = stateRobot;
    payload3[1] = (timestamp >> 24)& 0xFF;
    payload3[2] = (timestamp >> 16)& 0xFF;
    payload3[3] = (timestamp >> 8)& 0xFF;
    payload3[4] = timestamp & 0xFF;


    UartEncodeAndSendMessage(0x50, 5, payload3);
}

void OperatingSystemLoop(void) {
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;

            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            if (autoControlActivated)
                stateRobot = STATE_ATTENTE_EN_COURS;
            break;
        case STATE_ATTENTE_EN_COURS:

            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;
        case STATE_AVANCE:
            CompteurEtape();
            PWMSetSpeedConsigne(30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            if (autoControlActivated)
                SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_GAUCHE:
            CompteurEtape();
            PWMSetSpeedConsigne(10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            if (autoControlActivated)
                SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_DROITE:
            CompteurEtape();
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(10, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            if (autoControlActivated)
                SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            CompteurEtape();
            PWMSetSpeedConsigne(12, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-12, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            if (autoControlActivated)
                SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE:
            CompteurEtape();
            PWMSetSpeedConsigne(-12, MOTEUR_DROIT);
            PWMSetSpeedConsigne(12, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            if (autoControlActivated)
                SetNextRobotStateInAutomaticMode();
            break;

        case RECULE:
            CompteurEtape();
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;

        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}
unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;
    //�Dtermination de la position des obstacles en fonction des ���tlmtres
    if (Capteur == 0b00001) //Obstacle �droite
        positionObstacle = OBSTACLE_A_DROITE_1;


    else if (Capteur == 0b00010) //Obstacle �droite
        positionObstacle = OBSTACLE_A_DROITE_1;

    else if (Capteur == 0b00100)
        positionObstacle = OBSTACLE_EN_FACE;


    else if (Capteur == 0b01000) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_GAUCHE;
    else if (Capteur == 0b10000) //Obstacle �gauche
        positionObstacle = OBSTACLE_EN_FACE; //




    else if (Capteur == 0b10001) //Obstacle �gauche
        positionObstacle = OBSTACLE_EN_FACE_1;
    else if (Capteur == 0b11011) //Obstacle �gauche
        positionObstacle = OBSTACLE_EN_FACE;
    else if (Capteur == 0b11000) //Obstacle �gauche
        positionObstacle = OBSTACLE_EN_FACE;
    else if (Capteur == 0b11100) //Obstacle �gauche
        positionObstacle = OBSTACLE_EN_FACE;
    else if (Capteur == 0b11110) //Obstacle �gauche
        positionObstacle = OBSTACLE_EN_FACE;
    else if (Capteur == 0b00011) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_DROITE_1;
    else if (Capteur == 0b00111) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_DROITE_1;
    else if (Capteur == 0b01111) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_DROITE_1;
    else if (Capteur == 0b11111) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_DROITE_1;

    else if (Capteur == 0b00110) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_DROITE_1;
    else if (Capteur == 0b01100) //Obstacle �gauche
        positionObstacle = OBSTACLE_EN_FACE;

    else if (Capteur == 0b01001) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_DROITE;
    else if (Capteur == 0b10010) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_GAUCHE;

    else if (Capteur == 0b01011) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_DROITE_1;

    else if (Capteur == 0b01010) //Obstacle �gauche
        positionObstacle = OBSTACLE_EN_FACE;

    else if (Capteur == 0b10011) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_DROITE;

    else if (Capteur == 0b11001) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_GAUCHE;

    else if (Capteur == 0b10101) //Obstacle �gauche
        positionObstacle = OBSTACLE_EN_FACE;

    else if (Capteur == 0b11101) //Obstacle �gauche
        positionObstacle = OBSTACLE_EN_FACE;
    else if (Capteur == 0b10111) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_GAUCHE;

    else if (Capteur == 0b11010) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_DROITE_1;
    else if (Capteur == 0b01110) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_DROITE_1;


    else if (Capteur == 0b01101) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_DROITE_1;
    else if (Capteur == 0b10110) //Obstacle �gauche
        positionObstacle = OBSTACLE_EN_FACE;



    else if (Capteur == 0b00101) //Obstacle �gauche
        positionObstacle = OBSTACLE_A_DROITE_1;
    else if (Capteur == 0b10100) //Obstacle �gauche
        positionObstacle = OBSTACLE_EN_FACE;

    else if (Capteur == 0b00000) //Obstacle �gauche
        positionObstacle = PAS_D_OBSTACLE;



    //�Dtermination de l�?tat �venir du robot
    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;

    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;

    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;


    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;


    else if (positionObstacle == OBSTACLE_A_DROITE_1)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;

    else if (positionObstacle == OBSTACLE_EN_FACE_1)
        nextStateRobot = STATE_AVANCE;


    else if (positionObstacle == OBSTACLE_EN_FACE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    //Si l?on n?est pas dans la transition de l�?tape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
}

