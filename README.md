# Driver Project 6
This branch contains the source code that satisfies the requirements presented
for driver project #6 concerning CVRef.

# Deliverables
* Create C source files Comparator.c and Comparator.h containing the following function:
* void CVREFinit(vref)
    - Takes in the user voltage reference as a floating point number vref (note: float in pic is a 32 bit number)
    - Chooses CVRR based on vref
    - Calculates CVR3:0 to get the correct voltage references
    - Outputs the voltage reference CVRef (pin 17) on the PIC24F (verify with scope or multimeter)