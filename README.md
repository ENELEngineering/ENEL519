# Driver Project 7
This branch contains the source code that satisfies the requirements presented
for driver project #7 concerning comparators.

# Deliverables
* Create function: void ComparatorInit(void) in C-source file comparator.c and comparator.h 
* Uses internal clock
* When called in main(), it should be used to compare voltages between CVREF (1V) and external voltage connected to C2INC/RA3/pin8 using a potentiometer or external DC supply. Verify working by probing C2out on a scope and outputting "C2out hi" or "C2out lo" on PC terminal for the voltage on C2INC exceeding or going below the CVREF voltage.
* Implement a frequency Divide-by-N Circuit using the PIC24F's comparator. **(OPTIONAL)**
    - Use PB1 to increment counter N
    - Use PB2 to enter N and divide a pulse signal for varying frequency (using function generator) connected to C2INC/RA3/pin8. Output the pulse with the divided frequency to pin16/RB3.