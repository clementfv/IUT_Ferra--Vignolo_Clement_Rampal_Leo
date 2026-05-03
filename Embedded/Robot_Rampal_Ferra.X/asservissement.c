#include "asservissement.h"
#include "UART_Protocol.h"

PidCorrector PidX;
PidCorrector PidTheta;

void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double pro){
PidCorr->Kp = Kp;
PidCorr->erreurProportionelleMax = proportionelleMax; 
PidCorr->Ki = Ki;
PidCorr->erreurIntegraleMax = integralMax; 
PidCorr->Kd = Kd;
PidCorr->erreurDeriveeMax = deriveeMax;
UartEncodeAndSendMessage(0x67,)
}
