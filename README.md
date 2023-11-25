# App Project 2
This branch contains the source code that satisfies the requirements presented
for app project #2 concerning capacitance measurements.

# Deliverables
* Implement a capacitance measuring app on pin 16/AN11/RB13 using the CTMU on the PIC. Name source files ZSense.C and ZSense.h
    - App should be capable of measuring the capacitance of between 10pF and 1mF.
    - App should display the value of the capacitance on a single line of PC terminal window including units (in pF, nF, uF).
    - App should display the value of the capacitance vs. time over a minute on a Python Graph including units (in pF, nF, uF).
    - Verify the working of your code by measuring the capacitance between 10pF and 1mF.

# CTMU Mathematical Estimates
* pF - nF Caps: 1uA
* uF Caps: 10s of uA
* mF Caps: 10s of uA
* Capacitance values vs Rise Time to 2.5V?
* Smallest Capacitance measured?
    - Constraints: Timer
* Largest Capacitance measured?
    - Constraints: Timer and leakage current.