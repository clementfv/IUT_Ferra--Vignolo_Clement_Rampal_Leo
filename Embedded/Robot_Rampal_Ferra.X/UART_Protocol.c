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

    frame[index++] = 0xFE;
    frame[index++] = (unsigned char)(msgFunction >> 8);
    frame[index++] = (unsigned char)(msgFunction & 0xFF);
    frame[index++] = (unsigned char)(msgPayloadLength >> 8);
    frame[index++] = (unsigned char)(msgPayloadLength & 0xFF);
    for (i=0; i<msgPayloadLength; i++)
        frame[index++] = msgPayload[i];
    frame[index++] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

    SendMessageDirect(frame, index); 
}

// ================= décodage =================
//int msgDecodedFunction = 0;
//int msgDecodedPayloadLength = 0;
//unsigned char msgDecodedPayload[128];
//int msgDecodedPayloadIndex = 0;

//void UartDecodeMessage(unsigned char c)
//{
//// Fonction prenant pour reconstituer les trames
//}

//void UartProcessDecodedMessage(int function,
//int payloadLength, unsigned char* payload)
//{ 
//}
