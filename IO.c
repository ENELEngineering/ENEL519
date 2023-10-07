/*
 * File:   IO.c
 * Author: johns
 *
 * Created on October 2, 2023, 2:48 PM
 */


#include "xc.h"
#include "IO.h"
#include "Timer.h"

/* Create the ISR routine. Common interrupt routine for all CN inputs.
 * The interrupt is triggered by any change of state: hi to lo, lo to hi.
 * Interrupts will be triggered for debounces on push buttons too.
 * Clear the interrupt flag
 * In interrupt routine, check the PORTs to determine 
 * which IO pin had a change of notification.
 */

// Global Variables
uint16_t previous_RB4_status = 1;
uint16_t previous_RA4_status = 1;
uint16_t previous_RA2_status = 1;

uint16_t RB4_INTR_Flag = 0;
uint16_t RA4_INTR_Flag = 0;
uint16_t RA2_INTR_Flag = 0;

void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void) {
    if (IFS1bits.CNIF == 1) {
        // Should handle debounce effects.
        delay_ms(220, 1);
        
        if (PORTBbits.RB4 == 0) {
            RB4_INTR_Flag = 1;
            RA4_INTR_Flag = 0;
            RA2_INTR_Flag = 0;
            // If RB4 press is interrupting.
            if (PORTBbits.RB4 != previous_RB4_status) {
                check_flags();
                uint16_t RA4_status = check_RA4();
                uint16_t RA2_status = check_RA2();
                
                if (RA4_status == 1 && RA2_status == 1) {
                    Disp2String("\n\r CN1/RB4, CN0/RA4, CN30/RA2 are pressed\n");
                }
                else if (RA4_status == 1 && RA2_status == 0) {
                    Disp2String("\n\r CN1/RB4, CN0/RA4 are pressed\n");
                }
                else if (RA4_status == 0 && RA2_status == 1) {
                    Disp2String("\n\r CN1/RB4, CN30/RA2 are pressed\n");
                }
                else if (RA4_status == 0 && RA2_status == 0) {
                    Disp2String("\n\r CN1/RB4 is pressed\n");
                }
            }
        } 
        else if (PORTAbits.RA4 == 0) {
            RB4_INTR_Flag = 0;
            RA4_INTR_Flag = 1;
            RA2_INTR_Flag = 0;
            // If RA4 press is interrupting.
            if (PORTAbits.RA4 != previous_RA4_status) {
                check_flags();
                uint16_t RB4_status = check_RB4();
                uint16_t RA2_status = check_RA2();
                
                if (RB4_status == 1 && RA2_status == 1) {
                    Disp2String("\n\r CN1/RB4, CN0/RA4, CN30/RA2 are pressed\n");
                }
                else if (RB4_status == 1 && RA2_status == 0) {
                    Disp2String("\n\r CN1/RB4, CN0/RA4 are pressed\n");
                }
                else if (RB4_status == 0 && RA2_status == 1) {
                    Disp2String("\n\r CN0/RA4, CN30/RA2 are pressed\n");
                }
                else if (RB4_status == 0 && RA2_status == 0) {
                    Disp2String("\n\r CN0/RA4 is pressed\n");
                }
            }
        }
        else if (PORTAbits.RA2 == 0) {
            // If RA2 press is interrupting.
            RB4_INTR_Flag = 0;
            RA4_INTR_Flag = 0;
            RA2_INTR_Flag = 1;
            
            if (PORTAbits.RA2 != previous_RA2_status) {
                check_flags();
                uint16_t RB4_status = check_RB4();
                uint16_t RA4_status = check_RA4();
                
                if (RB4_status == 1 && RA4_status == 1) {
                    Disp2String("\n\r CN1/RB4, CN0/RA4, CN30/RA2 are pressed\n");
                }
                else if (RB4_status == 1 && RA4_status == 0) {
                    Disp2String("\n\r CN1/RB4, CN30/RA2 are pressed\n");
                }
                else if (RB4_status == 0 && RA4_status == 1) {
                    Disp2String("\n\r CN0/RA4, CN30/RA2 are pressed\n");
                }
                else if (RB4_status == 0 && RA4_status == 0) {
                    Disp2String("\n\r CN30/RA2 is pressed\n");
                }
            }
        }
    }
    previous_RB4_status = PORTBbits.RB4;
    previous_RA4_status = PORTAbits.RA4;
    previous_RA2_status = PORTAbits.RA2;
  
    // Clear the IF flag.
    IFS1bits.CNIF = 0;
    Nop();
}

void check_flags(void) {
    if (RB4_INTR_Flag == 1) {
        CNEN1bits.CN11IE = 1;
        CNEN1bits.CN0IE = 0;
        CNEN2bits.CN30IE = 0;
    }
    
    if (RA4_INTR_Flag == 1) {
        CNEN1bits.CN0IE = 1;
        CNEN1bits.CN11IE = 0;
        CNEN2bits.CN30IE = 0;
    }
    
    if (RA2_INTR_Flag == 1) {
        CNEN2bits.CN30IE = 1;
        CNEN1bits.CN0IE = 0;
        CNEN1bits.CN11IE = 0;
    }
}

void display_message(void) {
    delay_sec(1);
    uint16_t RB4_status = check_RB4();
    uint16_t RA4_status = check_RA4();
    uint16_t RA2_status = check_RA2();
    
    if (RB4_status == 1 && RA4_status == 1 && RA2_status == 1) {
        Disp2String("\n\r CN1/RB4, CN0/RA4, CN30/RA2 are pressed\n");
    }
    else if (RB4_status == 1 && RA4_status == 1 && RA2_status == 0) {
        Disp2String("\n\r CN1/RB4, CN0/RA4 are pressed\n");
    }
    else if (RB4_status == 1 && RA4_status == 0 && RA2_status == 1) {
        Disp2String("\n\r CN1/RB4, CN30/RA2 are pressed\n");
    }
    else if (RB4_status == 1 && RA4_status == 0 && RA2_status == 0) {
        Disp2String("\n\r CN1/RB4 is pressed\n");
    }
    else if (RB4_status == 0 && RA4_status == 1 && RA2_status == 1) {
        Disp2String("\n\r CN0/RA4, CN30/RA2 are pressed\n");
    }
    else if (RB4_status == 0 && RA4_status == 1 && RA2_status == 0) {
        Disp2String("\n\r CN0/RA4 is pressed\n");
    }
    else if (RB4_status == 0 && RA4_status == 0 && RA2_status == 1) {
        Disp2String("\n\r CN30/RA2 is pressed\n");
    }
    previous_RB4_status = PORTBbits.RB4;
    previous_RA4_status = PORTAbits.RA4;
    previous_RA2_status = PORTAbits.RA2;
}

uint16_t check_RB4(void) {
    if (PORTBbits.RB4 != previous_RB4_status && PORTBbits.RB4 == 0) {
        return 1;
    }
    return 0;
}

uint16_t check_RA4(void) {
    if (PORTAbits.RA4 != previous_RA4_status && PORTAbits.RA4 == 0) {
        return 1;
    }
    return 0;
}

uint16_t check_RA2(void) {
    if (PORTAbits.RA2 != previous_RA2_status && PORTAbits.RA2 == 0) {
        return 1;
    }
    return 0;
}


/* Peripheral Configuration
 * Configure IO pins as input: TRISxBITS.TRISx# = 1;
 * Configure IO pin to be either pulled up or pulled low using 
 * internal resistors in PIC.
 */
void configure_peripherals(void) {
    // Configure push button pins (RA2, RA4, RB4) as inputs.
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA4 = 1;
    TRISBbits.TRISB4 = 1;

    // Enable Pull-up on the push button pins with register.
    CNPU2bits.CN30PUE = 1;
    CNPU1bits.CN0PUE = 1;
    CNPU1bits.CN1PUE = 1;
}

/* Wakes up the PIC from idle or sleep when push buttons tied to
 * RB4/CN1, RA4/CN0, and RA2/CN30 are pushed.
 * Displays the status of the push buttons on Teraterm window when 
 * one of more buttons are pushed. "CN1/RB4 is pressed", 
 * "CN1/RB4 and CN0/RA4 are pressed."
 */
void CN_init(void) {
    // Set the interrupt priority to highest priority.
    IPC4bits.CNIP = 0b111;

    // To handle debounce effects, disable the interrupt for
    // a certain period of time once a push button is pressed
    // and then enable.
    // Enable the interrupt.
    IEC1bits.CNIE = 1;
    CNEN1bits.CN0IE = 1;
    CNEN1bits.CN11IE = 1;
    CNEN2bits.CN30IE = 1;
}