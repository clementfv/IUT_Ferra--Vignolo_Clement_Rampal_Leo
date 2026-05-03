/* 
 * File:   UART.h
 * Author: E306-PC2
 *
 * Created on 4 décembre 2025, 08:41
 */

#ifndef UART_H
#define	UART_H

void InitUART(void);
void SendMessageDirect(unsigned char* message, int length);
#endif	/* UART_H */

