#ifndef CB_RX1_H
#define CB_RX1_H

#define CBRX1_BUFFER_SIZE 128

void SendMessage(unsigned char* message, int length);
void CB_RX1_Add(unsigned char value);
unsigned char CB_RX1_Get(void);

unsigned char CB_RX1_IsTransmitting(void);

int CB_RX1_GetDataSize(void);
int CB_RX1_GetRemainingSize(void);

void SendOne(void);

#endif
