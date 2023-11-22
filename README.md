# Driver Project 8
This branch contains the source code that satisfies the requirements presented
for driver project #8 concerning CTMU.

# Deliverables
* Create function CTMUinit() and RSense() in ZSense.c to measure resistance on pin 16/AN11/RB13 using CTMU on the PIC.
* CTMUinit(): Initializes the CTMU settings
* RSense(): Measures the resistance of resistor connected measurements.
    - Use the CTMU current source at 5.5uA and known 100 k-ohm resistor (Measured exactly with multi-meter) to measure voltage across resistor using ADC. Display ADC voltage and calculated resistance value on Terminal - Determine CTMU current value if resistor value is known.
    - Use the CTMU current source at 55uA and 100 k-ohm resistor (Measured with multi-meter) to measure voltage across resistor using ADC. Display ADC voltage and calculated resistance value on Terminal - Determine exact/measured CTMU current value from previous 2 steps. Use this value in App project 2.