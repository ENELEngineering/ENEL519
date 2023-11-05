/*
 * File:   IR.c
 * Author: johns
 *
 * Created on November 4, 2023, 10:32 PM
 */

#include <libpic30.h>
#include "xc.h"
#include "IR.h"
#include "Timer.h"
#include "ChangeClk.h"

#define FCY 4000000UL

/* Create the ISR routine. Common interrupt routine for all CN inputs.
 * The interrupt is triggered by any change of state: hi to lo, lo to hi.
 * Interrupts will be triggered for debounces.
 * In interrupt routine, check the PORTs to determine 
 * which IO pin had a change of notification.
 */
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void) {
    
}

/* 
 * Function to initialize all the I/O ports and the CN interrupts.
 */
void CN_init(){ 
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital on RA2
    TRISBbits.TRISB2 = 1; // IR Reciever on RB2.
    CNEN1bits.CN6IE = 1 // Enable the interrupt for RB2.
    IPC4bits.CNIP = 0b111; // Set highest priority for interrupt.
    IEC1bits.CNIE = 1; // Interrupt request enabled.
    return;
}

/**
 * Check commands received by the IR receiver..
 * Power On/Off: startbit_0xE0E040BF
 * Volume Up: startbit_0xE0E0E01F
 * Volume Down: startbit_0xE0E0D02F
 * Channel Up: startbit_0xE0E048B7
 * Channel Down: startbit_0xE0E008F7
 */
void CN_check() {  
    
    Disp2String("\n\r I was interrupted\n");
    
}