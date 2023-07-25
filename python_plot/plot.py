#!/usr/bin/env python

from threading import Thread
import serial
import time
import collections
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.widgets import Button
import struct
import pandas as pd
import numpy as np


class serialPlot:
    def __init__(self, serialPort = 'COM6', serialBaud = 9600, plotLength = 100, dataNumBytes = 2):
        self.port = serialPort
        self.baud = serialBaud
        self.plotMaxLength = plotLength
        self.dataNumBytes = dataNumBytes
        self.rawData = bytearray(dataNumBytes)
        self.data = collections.deque([0] * plotLength, maxlen=plotLength)
        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.plotTimer = 0
        self.previousTimer = 0
        self.my_value = 0
        self.my_tuple = ()
        # self.csvData = []

        print('Trying to connect to: ' + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
        try:
            self.serialConnection = serial.Serial(serialPort, serialBaud, timeout=4)
            print('Connected to ' + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
        except:
            print("Failed to connect with " + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
            
    def writeSerial(self, frame):
        self.serialConnection.write(str.encode(frame))
        print(frame)
        
    def readSerialStart(self):
        if self.thread == None:
            self.thread = Thread(target=self.backgroundThread)
            self.thread.start()
            # Block till we start receiving values
            while self.isReceiving != True:
                time.sleep(0.1)

    def getSerialData(self, frame, lines, lineValueText, lineLabel, timeText):
        currentTimer = time.perf_counter()
        self.plotTimer = int((currentTimer - self.previousTimer) * 1000)     # the first reading will be erroneous
        self.previousTimer = currentTimer
        timeText.set_text('Plot Interval = ' + str(self.plotTimer) + 'ms')
        value,  = self.my_tuple    # use 'h' for a 2 byte integer
        # value = int.from_bytes(self.rawData, 'little')
        # print(value)
        
        self.data.append(value)    # we get the latest data point and append it to our array
        lines.set_data(range(self.plotMaxLength), self.data)
        lineValueText.set_text('[' + lineLabel + '] = ' + str(value))
        # self.csvData.append(self.data[-1])

    def backgroundThread(self):    # retrieve data
        time.sleep(1.0)  # give some buffer time for retrieving data
        self.serialConnection.reset_input_buffer()
        while (self.isRun):
            self.serialConnection.readinto(self.rawData)
            self.isReceiving = True
            self.my_value = 0
            for x in range(len(self.rawData)):  
                self.my_value += (self.rawData[x]-48)*10**(len(self.rawData)-(x+1))
                # print("My value starts from ", self.rawData[x], "getting to ", my_value, "for x:", x)                              
            # print("final ",my_value)
            self.my_tuple = (self.my_value,)
            print(self.rawData)

    def close(self):
        self.isRun = False
        self.thread.join()
        self.serialConnection.close()
        print('Disconnected...')
        # df = pd.DataFrame(self.csvData)
        # df.to_csv('/home/rikisenia/Desktop/data.csv')

# creating data
# defining function to add line plot

portName = 'COM6'     # for windows users
baudRate = 9600
maxPlotLength = 100
dataNumBytes = 6        # number of bytes of 1 data point
s = serialPlot(portName, baudRate, maxPlotLength, dataNumBytes)   # initializes all required variables

def bt_stop(val):
    s.writeSerial('stop\n')
def bt_start(val):
    s.writeSerial('start\n') 
def bt_conf1(val):
    s.writeSerial('config1\n') 
def bt_conf2(val):
    s.writeSerial('config2\n') 
def bt_conf3(val):
    s.writeSerial('config3\n') 
def bt_conf4(val):
    s.writeSerial('config4\n')             
           
    
def main():
    # portName = '/dev/ttyUSB0'

    s.readSerialStart()                                               # starts background thread

    # plotting starts below
    pltInterval = 18    # Period at which the plot animation updates [ms]
    xmin = 0
    xmax = maxPlotLength
    ymin = -(1)
    ymax = 20000
    fig = plt.figure()
    ax = fig.subplots()
    ax = plt.axes(xlim=(xmin, xmax), ylim=(float(ymin - (ymax - ymin) / 10), float(ymax + (ymax - ymin) / 10)))
    ax.set_title('INA219 AVR-Current profiler')
    ax.set_xlabel("time")
    ax.set_ylabel("Current mA")

    lineLabel = 'Current Value'
    timeText = ax.text(0.50, 0.95, '', transform=ax.transAxes)
    lines = ax.plot([], [], label=lineLabel)[0]
    lineValueText = ax.text(0.50, 0.90, '', transform=ax.transAxes)
    anim = animation.FuncAnimation(fig, s.getSerialData, fargs=(lines, lineValueText, lineLabel, timeText), interval=pltInterval)    # fargs has to be a tuple

    plt.legend(loc="upper left")

    # defining button and add its functionality
    plt.subplots_adjust(left = 0.3, bottom = 0.25)
    axes = plt.axes([0.71, 0.05, 0.1, 0.075])
    bn_stop = Button(axes, 'STOP',color="yellow")
    bn_stop.on_clicked(bt_stop)

    axes = plt.axes([0.41, 0.05, 0.1, 0.075])
    bn_start = Button(axes, 'START',color="yellow")
    bn_start.on_clicked(bt_start)
    
    axes = plt.axes([0.02, 0.8, 0.15, 0.075])
    bn_confg1 = Button(axes, '1 Sample\ne/ 532uS',color="yellow")
    bn_confg1.on_clicked(bt_conf1)    

    axes = plt.axes([0.02, 0.6, 0.15, 0.075])
    bn_confg2 = Button(axes, '4 Sample\ne/ 213uS',color="yellow")
    bn_confg2.on_clicked(bt_conf2)    
    
    axes = plt.axes([0.02, 0.4, 0.15, 0.075])
    bn_confg3 = Button(axes, '16 Sample\ne/ 851uS',color="yellow")
    bn_confg3.on_clicked(bt_conf3) 

    axes = plt.axes([0.02, 0.2, 0.15, 0.075])
    bn_confg4 = Button(axes, '32 Sample\ne/ 17mS',color="yellow")
    bn_confg4.on_clicked(bt_conf4)        

    plt.show()

    s.close()


if __name__ == '__main__':
    main()