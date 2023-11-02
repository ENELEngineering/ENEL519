/*
 * File:   TVIR.c
 * Author: johns
 *
 * Created on October 20, 2023, 4:34 PM
 */

#include <libpic30.h>
#include "xc.h"
#include "TVIR.h"
#include "Timer.h"
#include "ChangeClk.h"

#define FCY 4000000UL

// Global Variables
volatile uint16_t RB4_Flag, RA2_Flag, Print_Flag=1, Two_Press_Flag=0;
volatile uint16_t ninterrupt=0;

// Configured 0 as Channel Mode and 1 as Volume Mode.
uint16_t mode=0;

/* Create the ISR routine. Common interrupt routine for all CN inputs.
 * The interrupt is triggered by any change of state: hi to lo, lo to hi.
 * Interrupts will be triggered for debounces on push buttons too.
 * In interrupt routine, check the PORTs to determine 
 * which IO pin had a change of notification.
 */
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void) {
    IEC1bits.CNIE = 0; // Handle debounce effects by disabling the interrupts.
    delay_ms(50, 1); // Allow debounces to settle by setting a delay.
    Nop();
    
    if (IFS1bits.CNIF = 1) {
        ninterrupt += 1;
    }
    
    // Set flags depending on which push buttons are pressed.
    if (PORTBbits.RB4 == 0 && PORTAbits.RA2 == 0) {
        RB4_Flag = 1;
        RA2_Flag = 1;
        Two_Press_Flag = 1;
    }
    else if (PORTBbits.RB4 == 0 && PORTAbits.RA2 == 1) {
        RB4_Flag = 1;
        RA2_Flag = 0;
        Two_Press_Flag = 0;
    }
    else if (PORTBbits.RB4 == 1 && PORTAbits.RA2 == 0) {
        RB4_Flag = 0;
        RA2_Flag = 1;
        Two_Press_Flag = 0;
    }
    else if (PORTBbits.RB4 = 1 && PORTAbits.RA2 == 1) {
        RB4_Flag = 0;
        RA2_Flag = 0;
        Print_Flag = 1;
    }
    else{
        RB4_Flag = 0;       
        RA2_Flag = 0;
    }
    IFS1bits.CNIF = 0; // Clear the IF flag.
    IEC1bits.CNIE = 1; // Re-enable the interrupt.
    
    if (ninterrupt >= 2) {
        ninterrupt = 0;
    }
}

/* 
 * Function to initialize all the I/O ports and the CN interrupts.
 */
void CN_init(){ 
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital on RA2
    TRISAbits.TRISA2 = 1; // Set RA2 as input (Top button).
    TRISBbits.TRISB4 = 1; // Set RB4 as input (Buttom button).
    CNPU2bits.CN30PUE = 1; // Set pull up on RA2.
    CNPU1bits.CN1PUE = 1; // Set pull up on RB4.
    CNEN2bits.CN30IE = 1; // Enable the interrupt for RA2.
    CNEN1bits.CN1IE = 1; // Enable the interrupt for RB4.
    IPC4bits.CNIP = 0b111; // Set highest priority for interrupt.
    IEC1bits.CNIE = 1; // Interrupt request enabled.
    return;
}

/**
 * Check push button behaviour.
 * Power On/Off => Both buttons are pressed for more than 3 seconds.
 * Channel/Volume Mode switch => Both buttons are pressed for less than 3 seconds.
 * Channel/Volume Up => RA2 pressed.
 * Channel/Volume Down => RB4 pressed. 
 */
void CN_check() {  
    NewClk(500);
    
    if (ninterrupt <= 1) {
        // Detect both push buttons are pressed.
        if (RB4_Flag == 1 && RA2_Flag == 1) {
            T3CONbits.TON = 1; // Start the 16 bit timer 3.
            TMR3 = 0; // Clear timer 3 at the start.
        } 
        // Detect top button RA2 is pressed.
        if (RB4_Flag == 0 && RA2_Flag == 1) {
            process_top_press();
        }
        // Detect bottom button RB4 is pressed.
        else if (RB4_Flag == 1 && RA2_Flag == 0) {
            process_bottom_press();
        }
        // Detect both push buttons are released.
        else if (RB4_Flag == 0 && RA2_Flag == 0) {
            process_two_press();
        }
    }
    
    // This signifies that CN interrupts is disabled, should be re-enabled.
    if (ninterrupt == 0) {
        IEC1bits.CNIE = 1; // Re-enable the interrupt.    
    }
    
}

/**
 * This function determines if the button presses are based on powering on/off
 * or changing modes to volume or channel changes.
 */
void process_two_press() {
    // This flag indicates that both pushbuttons were pressed. This is 
    // used to avoid single press and release to enter this code block.
    if (Two_Press_Flag == 1) {
        if (T3CONbits.TON == 1) {
            T3CONbits.TON == 0; // Stop the 16 bit timer 3.
            // 2930 translates to 3 seconds at 500KHz clock with 256 prescaler.
            if (TMR3 >= 2930) {
                TMR3 = 0; // Clear timer 3 at the start.
                Disp2String("\n\r Power ON/OFF\n");
                start_command(0xE0E040BF);
                delay_ms(50, 1);
            } else {
                mode = !mode;
                if (mode == 1) {
                    Disp2String("\n\r Selected Volume Mode\n");       
                } else {
                    Disp2String("\n\r Selected Channel Mode\n");
                } 
            }
            TMR3 = 0; // Clear timer 3 at the start.
        }
    }
}

/**
 * This function determines if the top button is intended for volume up
 * or channel up based on the selected mode.
 */
void process_top_press() {
    if (mode == 1) {
        if (Print_Flag == 1) {
           Disp2String("\n\r Volume Up\n");
           Print_Flag = 0;
        }  
        start_command(0xE0E0E01F);
        delay_ms(50, 1);
    } else {
        if (Print_Flag == 1) {
            Disp2String("\n\r Channel Up\n");  
            Print_Flag = 0;
        }
        start_command(0xE0E048B7);
        delay_ms(50, 1);
    }
}

/**
 * This function determines if the bottom button is intended for volume 
 * down or channel down depending on the selected mode.
 */
void process_bottom_press() {
    if (mode == 1) {
        if (Print_Flag == 1) {
            Disp2String("\n\r Volume Down\n");  
            Print_Flag = 0;
        }
        start_command(0xE0E0D02F);
        delay_ms(50, 1);
    } else {
        if (Print_Flag == 1) {
            Disp2String("\n\r Channel Down\n");  
            Print_Flag = 0;
        }
        start_command(0xE0E008F7);
        delay_ms(50, 1);
    }
}

/**
 * Carrier signal is created with a 38KHz frequency which means a period of
 * 26.32us, thus on bit of 13.16us and off bit of 13.16us.
 */
void carrier_signal() {
    LATBbits.LATB9 = 1;
    __delay32(43); // Change to 45 if called by itself, more accurate to 38KHz.
    LATBbits.LATB9 = 0;
    __delay32(43);
}

/**
 * The start bit signal is created when producing the carrier signal for 
 * 4.5ms and then no signal for 4.5ms.
 */
void start_bit_signal() {
    T1CONbits.TON = 1; // Start the 16 bit timer 1.
    TMR1 = 0; // Clear timer 1 at the start.
    // 18000 represents 4.5ms on a 8MHz clock with 1:1 prescaler.
    while (TMR1 <= 18000) {
        carrier_signal();
    }
    T1CONbits.TON == 0; // Stop the 16 bit timer 1.
    
    T1CONbits.TON = 1; // Start the 16 bit timer 1.
    TMR1 = 0; // Clear timer 1 at the start.
    while(TMR1 <= 18000) {
        LATBbits.LATB9 = 0;
    }
    T1CONbits.TON == 0; // Stop the 16 bit timer 1.
    TMR1 = 0; // Clear timer 1.
    return;
}

/**
 * The one bit is created when sending the carrier signal for 0.56ms and 
 * no signal for 1.69ms.
 */
void one_bit_signal() {
    T1CONbits.TON = 1; // Start the 16 bit timer 1.
    TMR1 = 0; // Clear timer 1 at the start.
    // 2240 represents 0.56ms on a 8MHz clock with 1:1 prescaler.
    while (TMR1 <= 2240) {
        carrier_signal();
    }
    T1CONbits.TON == 0; // Stop the 16 bit timer 1.
    
    T1CONbits.TON = 1; // Start the 16 bit timer 1.
    TMR1 = 0; // Clear timer 1 at the start.
    // 6760 represents 1.69ms on a 8MHz clock with 1:1 prescaler.
    while(TMR1 <= 6760) {
        LATBbits.LATB9 = 0;
    }
    T1CONbits.TON == 0; // Stop the 16 bit timer 1.
    TMR1 = 0; // Clear timer 1.
    return;
}

/**
 * The zero signal is created when sending the carrier signal for 0.56ms and 
 * then no signal for 0.56ms.
 */
void zero_bit_signal() {
    T1CONbits.TON = 1; // Start the 16 bit timer 1.
    TMR1 = 0; // Clear timer 1 at the start.
    // 2240 represents 0.56ms on a 8MHz clock with 1:1 prescaler.
    while (TMR1 <= 2240) {
        carrier_signal();
    }
    T1CONbits.TON == 0; // Stop the 16 bit timer 1.
    
    T1CONbits.TON = 1; // Start the 16 bit timer 1.
    TMR1 = 0; // Clear timer 1 at the start.
    while(TMR1 <= 2240) {
        LATBbits.LATB9 = 0;
    }
    T1CONbits.TON == 0; // Stop the 16 bit timer 1.
    TMR1 = 0; // Clear timer 1.
    return;
}

/**
 * A stop bit signal is sending a high signal for atleast 46.82ms indicating
 * that the end of a command is reached.
 */
void stop_bit_signal() {
//    T3CONbits.TON = 1; // Start the 16 bit timer 3.
//    TMR3 = 0; // Clear timer 3 at the start.
//    // 782 represents 50ms on a 8MHz clock with 1:256 prescaler.
//    while (TMR3 <= 782) {
//        LATBbits.LATB9 = 0;
//    }
//    
//    T3CONbits.TON == 0; // Stop the 16 bit timer 3.
//    TMR3 = 0; // Clear timer 3 at the start.
    
    LATBbits.LATB9 = 0;
    __delay32(200000);
    return;
}

/**
 * This performs the commands to control a Samsung TV.
 * Power On/Off: startbit_0xE0E040BF
 * Volume Up: startbit_0xE0E0E01F
 * Volume Down: startbit_0xE0E0D02F
 * Channel Up: startbit_0xE0E048B7
 * Channel Down: startbit_0xE0E008F7
 * @param command: 32 bit value representing the command.
 */
void start_command(uint32_t command) {
    NewClk(8);
    // The start bit command indication.
    start_bit_signal();
    uint32_t check_MSB;
    // There are 32 bits in a given command.
    for (int i = 0; i<32; i++) {
        // The bit that is being analyzed starts from the most significant bit.
        check_MSB = command & 0x80000000;
        
        // If the MSB stores a value of 1, then run the 1-bit signal.
        if (check_MSB == 0x80000000) {
            one_bit_signal();
        }
        // If the MSB stores a value of 0, then run the 0-bit signal.
        else {
            zero_bit_signal();
        }
        // Perform one left bit shifting to analyze each command bit.
        command = command << 1;
    }
    
    // The following indicates a termination in the command. 
    zero_bit_signal();
    stop_bit_signal();
    return;
}