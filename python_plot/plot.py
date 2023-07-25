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
        self.data = []
        for i in range(2):   # give an array for each type of data and store them in a list
            self.data.append(collections.deque([i] * plotLength, maxlen=plotLength))        
        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.plotTimer = 0
        self.previousTimer = 0
        self.current_value = 0
        self.voltage_value = 0
        self.voltage_tuple = ()
        self.current_tuple = ()
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
                
    def parse_data(self):
        self.current_value = 0
        self.voltage_value = 0
        
        if chr(self.rawData[0]) == 'c':   
            for x in range(1,len(self.rawData)):  
                self.current_value += (self.rawData[x]-48)*10**(len(self.rawData)-(x+1))
            self.current_tuple = (self.current_value/1000,)
            # print("current", self.current_tuple)
        elif chr(self.rawData[0]) == 'v':
            for x in range(1,len(self.rawData)):  
                self.voltage_value += (self.rawData[x]-48)*10**(len(self.rawData)-(x+1))
            self.voltage_tuple = (self.voltage_value/1000,)
            # print("voltage", self.voltage_tuple)
            
    def getSerialData(self, frame, lines, lineValueText, lineLabel, timeText, pltNumber):       
        # print("My value starts from ", self.rawData[x], "getting to ", my_value, "for x:", x)                              
        # print("final ",my_value)
        # print(self.rawData)
        
        if pltNumber == 0:  # in order to make all the clocks show the same reading
            currentTimer = time.perf_counter()
            self.plotTimer = int((currentTimer - self.previousTimer) * 1000)     # the first reading will be erroneous
            self.previousTimer = currentTimer
            if not self.voltage_tuple:
                return
            else:     
                value,  = self.voltage_tuple       
        elif pltNumber == 1:
            if not self.current_tuple:
                return
            else:    
                value,  = self.current_tuple    

        timeText.set_text('Plot Interval = ' + str(self.plotTimer) + 'ms')
        
        self.data[pltNumber].append(value)    # we get the latest data point and append it to our array
        lines.set_data(range(self.plotMaxLength), self.data[pltNumber])
        lineValueText.set_text('[' + lineLabel + '] = ' + str(value))
        # self.csvData.append(self.data[-1])

    def backgroundThread(self):    # retrieve data
        time.sleep(1.0)  # give some buffer time for retrieving data
        self.serialConnection.reset_input_buffer()
        while (self.isRun):
            self.serialConnection.readinto(self.rawData)
            self.parse_data()
            self.isReceiving = True
        
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
dataNumBytes = 7        # number of bytes of 1 data point
s = serialPlot(portName, baudRate, maxPlotLength, dataNumBytes)   # initializes all required variables
anim = []

def bt_stop(val):
    s.writeSerial('stop\n')
def bt_pause(val):
    s.writeSerial('pause\n') 
    anim[0].event_source.stop()
    anim[1].event_source.stop()
def bt_start(val):
    s.writeSerial('start\n') 
    anim[0].event_source.start()
    anim[1].event_source.start()
def bt_conf1(val):
    s.writeSerial('config1\n') 
def bt_conf2(val):
    s.writeSerial('config2\n') 
def bt_conf3(val):
    s.writeSerial('config3\n') 
def bt_conf4(val):
    s.writeSerial('config4\n')             
       
def makeFigure(xLimit, yLimit, title):
    xmin, xmax = xLimit
    ymin, ymax = yLimit
    fig = plt.figure()
    ax = plt.axes(xlim=(xmin, xmax), ylim=(int(ymin - (ymax - ymin) / 10), int(ymax + (ymax - ymin) / 10)))
    ax.set_title(title)
    ax.set_xlabel("Time")
    ax.set_ylabel("Output")
    return fig, ax    
    
def main():
    # portName = '/dev/ttyUSB0'
    s.readSerialStart()                                               # starts background thread
    # plotting starts below
    pltInterval = 50    # Period at which the plot animation updates [ms]
    lineLabelText = ['V', 'C', 'Z']
    title = ['Voltage', ' Current', 'Z Acceleration']
    xLimit = [(0, maxPlotLength), (0, maxPlotLength), (0, maxPlotLength)]
    yLimit = [(0, 10), (0, 20), (-1, 1)]
    style = ['r-', 'g-', 'b-']    # linestyles for the different plots
    
        # defining button and add its functionality
    axes = plt.axes([0.45, 0.2, 0.1, 0.075])
    bn_stop = Button(axes, 'STOP',color="yellow")
    bn_stop.on_clicked(bt_stop)
    
    axes = plt.axes([0.25, 0.2, 0.1, 0.075])
    bn_pause = Button(axes, 'PAUSE',color="yellow")
    bn_pause.on_clicked(bt_pause)    

    axes = plt.axes([0.05, 0.2, 0.1, 0.075])
    bn_start = Button(axes, 'START',color="yellow")
    bn_start.on_clicked(bt_start)
    
    axes = plt.axes([0.45, 0.1, 0.15, 0.075])
    bn_confg1 = Button(axes, '1 Sample\ne/ 532uS',color="yellow")
    bn_confg1.on_clicked(bt_conf1)    

    axes = plt.axes([0.3, 0.1, 0.15, 0.075])
    bn_confg2 = Button(axes, '4 Sample\ne/ 213uS',color="yellow")
    bn_confg2.on_clicked(bt_conf2)    
    
    axes = plt.axes([0.15, 0.1, 0.15, 0.075])
    bn_confg3 = Button(axes, '16 Sample\ne/ 851uS',color="yellow")
    bn_confg3.on_clicked(bt_conf3) 

    axes = plt.axes([0, 0.1, 0.15, 0.075])
    bn_confg4 = Button(axes, '32 Sample\ne/ 17mS',color="yellow")
    bn_confg4.on_clicked(bt_conf4)
    
    plt.savefig('filename1')
    
    for i in range(0,2):
        fig, ax = makeFigure(xLimit[i], yLimit[i], title[i])
        lines = ax.plot([], [], style[i], label=lineLabelText[i])[0]
        timeText = ax.text(0.50, 0.95, '', transform=ax.transAxes)
        lineValueText = ax.text(0.50, 0.90, '', transform=ax.transAxes)
        anim.append(animation.FuncAnimation(fig, s.getSerialData, fargs=(lines, lineValueText, lineLabelText[i], timeText, i), interval=pltInterval))  # fargs has to be a tuple
        plt.legend(loc="upper left")

        
    plt.subplots_adjust(left = 0.3, bottom = 0.25)

    plt.show()

    s.close()


if __name__ == '__main__':
    main()