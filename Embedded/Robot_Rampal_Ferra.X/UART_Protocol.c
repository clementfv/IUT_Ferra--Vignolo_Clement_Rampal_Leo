#include <xc.h>
#include "UART_Protocol.h"

#define MAX_FRAME_SIZE 128

unsigned char UartCalculateChecksum(
        int msgFunction,
        int msgPayloadLength,
        unsigned char* msgPayload) {
    unsigned char checksum = 0;
    int i;

    checksum ^= 0xFE;

    checksum ^= (unsigned char) (msgFunction >> 8);
    checksum ^= (unsigned char) (msgFunction & 0xFF);

    checksum ^= (unsigned char) (msgPayloadLength >> 8);
    checksum ^= (unsigned char) (msgPayloadLength & 0xFF);

    /* Payload */
    for (i = 0; i < msgPayloadLength; i++) {
        checksum ^= msgPayload[i];
    }

    return checksum;
}

void UartEncodeAndSendMessage(
        int msgFunction,
        int msgPayloadLength,
        unsigned char* msgPayload) {
    unsigned char frame[MAX_FRAME_SIZE];
    int index = 0;
    int i;
    unsigned char checksum;

    /* Start byte */
    frame[index++] = 0xFE;

    /* Function (2 octets) */
    frame[index++] = (unsigned char) (msgFunction >> 8);
    frame[index++] = (unsigned char) (msgFunction & 0xFF);

    /* Payload length (2 octets) */
    frame[index++] = (unsigned char) (msgPayloadLength >> 8);
    frame[index++] = (unsigned char) (msgPayloadLength & 0xFF);

    /* Payload */
    for (i = 0; i < msgPayloadLength; i++) {
        frame[index++] = msgPayload[i];
    }

    /* Checksum */
    checksum = UartCalculateChecksum(
            msgFunction,
            msgPayloadLength,
            msgPayload
            );

    frame[index++] = checksum;

    /* Envoi UART */
    int SendMessage(frame, index);
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
