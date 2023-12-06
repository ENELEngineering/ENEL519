# -*- coding: utf-8 -*-
"""
PYTHON DEMO -- PART 2
Created on Wed Nov  2 11:54:30 2022

CAPTURING 16 BIT INT AND FLOAT NUMBERS OVER A TIME INTERVAL
Time stamps received numbers separated by \n
Saves in Dataframe
Saves in csv
Plots received data versus time

@author: Rushi V

Modified by John Santos and Anhela Francees
"""

import plotly.express as px
import pandas as pd
import numpy as np
import decimal
import serial  # pip install pyserial
import time

## OPEN SERIAL PORT 
ser = serial.Serial(port = "COM7", baudrate = 9600, bytesize = 8, timeout = 2, stopbits = serial.STOPBITS_ONE)


## INITIALIZATIONS
rxNumsStr = ''      #string to store received uint16_t numbers 
rxNumsList = []      #List to store received uint16_t numbers in int form 
rxTimesList = []   #list to store time stamps of received uint16_t numbers
startTime = time.time()   

## CAPTURE UART DATA
limit = 15
while(time.time() - startTime < limit):  #record data for 1 sec
    
    time_now = time.time() - startTime
    time_now = round(time_now, 2)
    print(f"Time {time_now} of {limit}", end="\r")

    line = ser.readline() # reads uint16_t nums as single bytes till \n n stores in string
    
    try:
        line = line.strip()
        line = line.rstrip()
        line = rxNumsStr + line.decode('Ascii')
        line = line.replace("Capacitance:", "")
        line = line.replace("uF", "")
        line = line.replace("\x00", "")
        line = line.replace("\x02\x02", "")
        
        capacitance = float(line)
        rxNumsList.append(capacitance)

        timeMeas = time.time() - startTime # Time stamp received number
        rxTimesList.append(timeMeas) # save time stamps in a list

    except UnicodeDecodeError:
        continue
    
## CLOSE SERIAL PORT    
ser.close()  # close any open serial ports

# ### Rx DATA CLEANUP AND STRING TO FLOAT CONVERSION
# rxNumsStr = rxNumsStr.replace('\x00','')  #\x00 seems to be sent with Disp2String()
# rxNumsStr = rxNumsStr.strip() # remove unwanted chars and spaces 
# rxNumsList = rxNumsStr.split(' \n ')  # split string by \n n store in list
# print(rxNumsList)
# rxNumsList = [float(elem) for elem in rxNumsList]  # convert char in List into int
temp = decimal.Decimal(rxNumsList[1])
# if (abs(temp.as_tuple().exponent) > 12):
#     unit = 'pF' # add logic to extract the correct unit (pF, nF, or uF)
# elif (abs(temp.as_tuple().exponent) <= 12 & abs(temp.as_tuple().exponent) > 9):
#     unit = 'nF' # add logic to extract the correct unit (pF, nF, or uF)
# elif (abs(temp.as_tuple().exponent) <= 9 & abs(temp.as_tuple().exponent) > 6):
#     unit = 'uF' # add logic to extract the correct unit (pF, nF, or uF)
# elif (abs(temp.as_tuple().exponent) <= 6 & abs(temp.as_tuple().exponent) > 3):
#     unit = 'mF' # add logic to extract the correct unit (pF, nF, or uF)

unit = "uF"

# ### CONVERT Rx DATA INTO DATA FRAME
dF = pd.DataFrame()
dF['Time (sec)'] = rxTimesList
dF["Capacitance (f'{unit}')"] = rxNumsList

# ### DATA STATISTICS
print(dF.describe())

# ### COPY RX DATA AND RX TIME IN CSV AND XLS FILES
dF.to_csv('RxDataFloat.csv', index = True)

# ### PLOT Rx DATA VS Rx TIME
fig = px.line(dF, x='Time (sec)', y="Capacitance (f'{unit}')", title = 'Measured Capacitance vs Time')
fig.show()