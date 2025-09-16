#ifndef ADC_H
#define	ADC_H

void ADC1StartConversionSequence();
unsigned char ADCIsConversionFinished(void);
void ADCClearConversionFinishedFlag(void);

#endif	/* ADC_H */
