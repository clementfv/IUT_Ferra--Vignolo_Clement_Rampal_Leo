#include <xc.h>
#include "UART_Protocol.h"

unsigned char UartCalculateChecksum(int msgFunction,
int msgPayloadLength, unsigned char* msgPayload)
{
//Fonction prenant entree la trame et sa longueur pour calculer le checksum
  byte checksum = 0;
  checksum ^= 0xFE;
  checksum ^= (byte)(msgFunction >> 8);
  checksum ^= (byte)(msgFunction >> 0);
  checksum ^= (byte)(msgPayloadLength >> 8);
  checksum ^= (byte)(msgPayloadLength >> 0);
  for (int i = 0; i < msgPayloadLength; i++)
  {
      checksum ^= msgPayload[i];
  }

  return checksum;
}
void UartEncodeAndSendMessage(int msgFunction,
int msgPayloadLength, unsigned char* msgPayload)
{
//Fonction d?encodage et d?envoi d?un message
int totalLength = 1 + 2 + 2 + msgPayloadLength + 1;
byte[] frame = new byte[totalLength];

int index = 0;
frame[index++] = 0xFE;


frame[index++] = (byte)(msgFunction >> 8);
frame[index++] = (byte)(msgFunction >> 0);


frame[index++] = (byte)(msgPayloadLength >> 8);
frame[index++] = (byte)(msgPayloadLength >> 0);

for (int i = 0; i < msgPayloadLength; i++)
    frame[index++] = msgPayload[i];


frame[index++] = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);


SendMessage(frame, frame.Length);
}
//int msgDecodedFunction = 0;
//int msgDecodedPayloadLength = 0;
//unsigned char msgDecodedPayload[128];
//int msgDecodedPayloadIndex = 0;
//void UartDecodeMessage(unsigned char c)
//{
////Fonction prenant en entree un octet et servant a reconstituer les trames
//}
//void UartProcessDecodedMessage(int function,
//int payloadLength, unsigned char* payload)
//{
////Fonction appelee apres le decodage pour executer l?action
////correspondant au message recu
//...
//}
//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/
