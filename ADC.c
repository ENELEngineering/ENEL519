/*
 * File:   ADC.c
 * Author: johns
 *
 * Created on October 14, 2023, 11:04 PM
 */


#include "xc.h"

unsigned int do_ADC(void) {
    uint16_t ADCvalue; // 16 bit register used to hold ADC converted digital output ADC1BUFO.
    
    /*---------- ADC INITIALIZATION ----------*/
    configure_ADC_AD1CON1();
    
    AD1CON2bits.VCFG = 0b000; // Selects AVDD, AVSS (supply voltage to PIC) as Vref.
    configure_ADC_AD1CON2();
    
    // TODO: Configure the ADC's sample time by setting bits in AD1CON3.
    // Ensure sample time is 1/10th of signal being sampled or as per application's speed and needs.
    configure_ADC_AD1CON3();
    
    // TODO: Select and configure ADC input.
    // Channel 0 negative input select for MUX A Multiplexer Setting bit.
    AD1CHSbits.CH0NA = 0; // Channel 0 negative input is VR-.
    // Channel 0 positive input select for MUX A multiplexer setting bits.
    AD1CHSbits.CH0SA = 0b0101; // Channel 0 positive input is AN5.
    // Analog input pin configuration control bits.
    AD1PCFG = AD1PCFG & 0b0000001111000111;
    // A/D input pin scan selection bits
    AD1CSSL = AD1CSSL & 0b0000001111000111;
    
    /*---------- ADC SAMPLING AND CONVERSION - CAN BE A DIFFERENT FUNCTION ----------*/
    
    AD1CON1bits.SAMP = 1; // Start Sampling, Conversion starts automatically after SSRC and SAMP settings.
    while(AD1CON1bits.DONE==0) {}
    
    ADCvalue = ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point.
    AD1CON1bits.SAMP = 0; // Stop sampling
    AD1CON1bits.ADON=0; // Turn off ADC, ADC value stored in ADC1BUFO.
    return (ADCvalue); // Returns 10 bit ADC output stored in ADC1BIFO to calling function.
}   
    
/*
 * Provide ADC Settings Configuration and Initializations.
 * Set bits in AD1CON1 register.
 */
void configure_ADC_AD1CON1(void) {
    // Powers the ADC module on.
    AD1CON1bits.ADON = 1;
    // Configure integer format.
    AD1CON1bits.FORM = 00;
    // For SW triggered ADC conversion.
    AD1CON1bits.SSRC = 111;
    // A/D Sample Auto-Start bit... Sampling begins when SAMP bit is set.
    AD1CON1bits.ASAM = 0;
    // Clear the A/D Conversion status bit.
    AD1CON1bits.DONE = 1; // A/D conversion is done.
}

/*
 * Provide ADC Settings Configuration and Initializations.
 * Set bits in AD1CON2 register.
 */
void configure_ADC_AD1CON2(void) {
    // Voltage reference configuration bits is set to AVdd and AVss.
    AD1CON2bits.VCFG = 0b000; 
    // Scan input selections for CH0+ S/H input for MUX A input multiplexer setting bit.
    AD1CON2bits.CSCNA = 0; // Do not scan inputs.
    // Sample/convert sequences per interrupt selection bits.
    AD1CON2bits.SMPI = 0b0000; // Interrupts at the completion of conversion for each sample/convert sequence.
    // Buffer Mode Select bit.
    AD1CON2bits.BUFM = 0; // Buffer configured as one 16-word buffer
    // Alternate Input Sample Mode Select bit.
    AD1CON2bits.ALTS = 0; // Always uses MUX A input multiplexer settings.
}

/*
 * Provide ADC Settings Configuration and Initializations.
 * Set bits in AD1CON3 register.
 */
void configure_ADC_AD1CON3(void) {
    // A/D Conversion Clock source bit. 
    AD1CON3bits.ADRC = 0; // Use the system clock, use ADC in sleep mode.
    // Auto Sample Time bits.
    AD1CON3bits.SAMC = 0b11111; // Slowest sampling time = 31*2/fclk.
    // A/D Conversion Clock Select bits
    AD1CON3bits.ADCS = 0b11111;
}