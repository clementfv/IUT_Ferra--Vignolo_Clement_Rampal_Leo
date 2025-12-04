#ifndef CB_TX1_H
#define CB_TX1_H

#define CBTX1_BUFFER_SIZE 128

void SendMessage(unsigned char* message, int length);
void CB_TX1_Add(unsigned char value);
unsigned char CB_TX1_Get(void);

unsigned char CB_TX1_IsTransmitting(void);

int CB_TX1_GetDataSize(void);
int CB_TX1_GetRemainingSize(void);

void SendOne(void);

#endif
