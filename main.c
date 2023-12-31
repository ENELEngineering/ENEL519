/* 
 * File:   main.c
 * Author: rv
 *
 * Created on January 9, 2017, 5:26 PM
 * 
 * Modified by John Santos and Anhela Francees on 
 * September 22, 2023.
 */


#include "xc.h"
#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include "ChangeClk.h"
#include "UART2.h"

//// CONFIGURATION BITS ////

// Code protection 
#pragma config BSS = OFF // Boot segment code protect disabled
#pragma config BWRP = OFF // Boot sengment flash write protection off
#pragma config GCP = OFF // general segment code protecion off
#pragma config GWRP = OFF

// CLOCK CONTROL 
#pragma config IESO = OFF    // 2 Speed Startup disabled
#pragma config FNOSC = FRC  // Start up CLK = 8 MHz
#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled
#pragma config SOSCSEL = SOSCLP // Secondary oscillator for Low Power Operation
#pragma config POSCFREQ = MS  //Primary Oscillator/External clk freq betwn 100kHz and 8 MHz. Options: LS, MS, HS
#pragma config OSCIOFNC = ON  //CLKO output disabled on pin 8, use as IO. 
#pragma config POSCMOD = NONE  // Primary oscillator mode is disabled

// WDT
#pragma config FWDTEN = OFF // WDT is off
#pragma config WINDIS = OFF // STANDARD WDT/. Applicable if WDT is on
#pragma config FWPSA = PR32 // WDT is selected uses prescaler of 32
#pragma config WDTPS = PS1 // WDT postscler is 1 if WDT selected

//MCLR/RA5 CONTROL
#pragma config MCLRE = OFF // RA5 pin configured as input, MCLR reset on RA5 diabled

//BOR  - FPOR Register
#pragma config BORV = LPBOR // LPBOR value=2V is BOR enabled
#pragma config BOREN = BOR0 // BOR controlled using SBOREN bit
#pragma config PWRTEN = OFF // Powerup timer disabled
#pragma config I2C1SEL = PRI // Default location for SCL1/SDA1 pin

//JTAG FICD Register
#pragma config BKBUG = OFF // Background Debugger functions disabled
#pragma config ICS = PGx2 // PGC2 (pin2) & PGD2 (pin3) are used to connect PICKIT3 debugger

// Deep Sleep RTCC WDT
#pragma config DSWDTEN = OFF // Deep Sleep WDT is disabled
#pragma config DSBOREN = OFF // Deep Sleep BOR is disabled
#pragma config RTCOSC = LPRC// RTCC uses LPRC 32kHz for clock
#pragma config DSWDTOSC = LPRC // DeepSleep WDT uses Lo Power RC clk
#pragma config DSWDTPS = DSWDTPS7 // DSWDT postscaler set to 32768 


// GLOBAL VARIABLES
unsigned int temp;
unsigned int i;

// MACROS
#define Nop() {__asm__ volatile ("nop");}
#define ClrWdt() {__asm__ volatile ("clrwdt");}
#define Sleep() {__asm__ volatile ("pwrsav #0");}   // set sleep mode
#define Idle() {__asm__ volatile ("pwrsav #1");}
#define dsen() {__asm__ volatile ("BSET DSCON, #15");}

/*
 * 
 */

int main(void) {
    //Clock output on REFO
    TRISBbits.TRISB15 = 0;  // Set RB15 as output for REFO
    REFOCONbits.ROEN = 1; // Ref oscillator is enabled
    REFOCONbits.ROSSLP = 0; // Ref oscillator is disabled in sleep
    REFOCONbits.ROSEL = 0; // Output base clk showing clock switching
    REFOCONbits.RODIV = 0b0000;
    OSCTUNbits.TUN = 0b111010;
    
    // LED on RB7
    //TRISBbits.TRISB7 = 0; // set RB7 as output for LED
    TRISBbits.TRISB8 = 0;
    // What is this for??
    AD1PCFG = 0xFFFF;
    
    // Configure push button pins (RA2, RA4, RB4) as inputs
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA4 = 1;
    TRISBbits.TRISB4 = 1;

    // Enable Pull-up on the push button pins with register
    CNPU2bits.CN30PUE = 1;
    CNPU1bits.CN0PUE = 1;
    CNPU1bits.CN1PUE = 1;
     
    // Switch clock: 32 for 32kHz, 500 for 500 kHz, 8 for 8MHz 
    NewClk(32); 
     
    while(1)
     {
        if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) {
            Disp2String("\n\r No presses\n");
        } else {
            
            LATBbits.LATB8 = 1;
            for (int i=0; i<32000;i++) { }
            LATBbits.LATB8 = 0;
            for (int i=0; i<32000;i++) { }
            
           if (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 0) {
               Disp2String("\n\r RA2, RB4, and RA4 pressed\n");
           }
           else if (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1) {
               Disp2String("\n\r RA2 and RA4 pressed\n");
           }
           else if (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0) {
               Disp2String("\n\r RA2 and RB4 pressed\n");
           }
           else if (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) {
               Disp2String("\n\r RA2 pressed\n");
           }
           else if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 0) {
               Disp2String("\n\r RA4 and RB4 pressed\n");
           }
           else if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1) {
               Disp2String("\n\r RA4 pressed\n");
           }
           else if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0) {
               Disp2String("\n\r RB4 pressed\n");
           }
        }
        
//        char str[10];
//        f= -15.5678;   
//        sprintf (str, "%1.3f", f);  // Converts -15.567 stored in f into an array of characters
//        Disp2String(str);	// Displays -15.567 on terminal	

//        d= -56;   
//        sprintf (str, "%1.0d", d); // Converts -56 stored in d into an array of characters
//        Disp2String(str);	// Displays -56 on terminal

//        char str[10]; 
//        f = -15.5678;   
//        sprintf (str, "%f", f);  // Converts -15.567 stored in f into an array of characters
//        // Disp2String(str);
//        Disp2String(str);// Displays -15.567 on terminal   
     }
    return 0;
}
