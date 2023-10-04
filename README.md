# CN Driver Project
This branch contains the source code that satisfies the requirements presented
for CN driver project concerning change of notifications.

# Deliverables
* Create C source files IO.c and IO.h containing the following function:
* void CN_init(void)
    - Wakes up the PIC from idle or sleep when push buttons tied to RB4/CN1, RA4/CN0,
    RA2/CN30 are pushed.
    - Displays the status of the push buttons on Teraterm window when one of more buttons
    are pushed.
        * i.e. "CN1/RB4 is pressed" or "CN0/RA4 is pressed" or "CN1/RB4 and CN0/RA4 are pressed".
    - Do this for all button press states.
* **CN_init(void) demo and source file due via Dropbox**