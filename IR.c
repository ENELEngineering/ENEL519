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

volatile uint16_t count_high = 0, count_low = 0;
// This is the hex command received.
uint32_t command = 0x00000000;
// This is the bit that was read depending on the timing. 
// 0 = 0-bit, 1 = 1-bit, 2 = start-bit
uint32_t decoded_bit;
uint32_t decoded_bits[32];

/* Create the ISR routine. Common interrupt routine for all CN inputs.
 * The interrupt is triggered by any change of state: hi to lo, lo to hi.
 * Interrupts will be triggered for debounces.
 * In interrupt routine, check the PORTs to determine 
 * which IO pin had a change of notification.
 */
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void) {
    
    IFS1bits.CNIF = 0; // Clear the IF flag.
    
    // Dedicate timer 2 to measuring high signals.
    
    if (PORTBbits.RB2 == 1) {
        count_high += 1;
        
        /* Start Timer 2 to measure length of the high signal */
        T2CONbits.TON = 1; // Start the 16 bit timer 2.
        TMR2 = 0; // Clear timer 2 at the start.
        
    } else {
        count_low += 1;
        
        /* Once the signal is low, high signal time is complete */
        if (T2CONbits.TON == 1) {
            T2CONbits.TON == 0; // Stop the 16 bit timer 2.
            // 20000 represents timing of 5ms common for the length of high signals in start bits.
            if (TMR2 <= 20000) {
                
                // 7000 represents timing of 1.80ms common for the high signals in 1-bit.
                if (TMR2 <= 7200) {
                    // 2400 represents timing of 0.80 ms common for the high signals of 0-bit.
                    if (TMR2 <= 3200) {
                        decoded_bit = 0; 
                    } else {
                        decoded_bit = 1;
                    }
                    
                    decoded_bits[count_high-2] = decoded_bit;
//                    if (count_high >= 1) {
//                        decoded_bit = decoded_bit << (32-count_high);
//                        command = command | decoded_bit;
//                    }
                    
                    
                } else{
                    decoded_bit = 2;
                }  
            }
            TMR2 = 0; // Clear timer 2.
        }
         
    }
    
    Nop();
}

/* 
 * Function to initialize all the I/O ports and the CN interrupts.
 */
void CN_init(){ 
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital on RA2
    TRISBbits.TRISB2 = 1; // IR Reciever on RB2.
    CNEN1bits.CN6IE = 1; // Enable the interrupt for RB2.
    CNPU1bits.CN6PUE = 1; // Set pull up on RB2. So that the first change of state is a 0.
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
    
    if (count_high >= 33 && count_low >= 34) {
        
        uint32_t mybit;
        for (int i=0; i<32; i++) {
            Disp2Dec(decoded_bits[i]);
            
            if (decoded_bits[i] == 1) {
                mybit = 1 << (31 - i);
            } else {
                mybit = 0 << (31 - i);
            }
            
            
            command = command | mybit;
        }
        Disp2Dec(command);
        command = 0x00000000;
        count_high = 0;
        count_low = 0;
        //Disp2Dec(count_low);
    }
    
    // Note that if a string is displayed it interrupts 3 times...
    // However we cant rely on this methodology to determine the final state. 
    // Should instead rely on the count of ones and zeros to be greater than something.
}