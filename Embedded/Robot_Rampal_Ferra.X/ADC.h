#ifndef ADC_H
#define	ADC_H

void ADC1StartConversionSequence();
unsigned char ADCIsConversionFinished(void);
void ADCClearConversionFinishedFlag(void);
void InitADC1(void);
unsigned int * ADCGetResult(void);


#endif	/* ADC_H */
