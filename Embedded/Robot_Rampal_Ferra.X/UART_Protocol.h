#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H
void UartEncodeAndSendMessage(int msgFunction,
        int msgPayloadLength, unsigned char* msgPayload);
unsigned char UartCalculateChecksum(int msgFunction,
        int msgPayloadLength, unsigned char* msgPayload);
#endif	/* UART_PROTOCOL_H */

