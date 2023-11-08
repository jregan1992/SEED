# import *
import cv2
import numpy as np
from time import sleep
import time
import math
import serial
import board
import struct
from threading import Thread
from threading import Lock
import signal
import sys
from playsound import playsound

# VERY IMPORTANT, KILLS THREADS
stop_threads = False

def Stop(sig, frame):
    global stop_threads
    
    print("... attempting to kill threads ...")
    # close other threads
    stop_threads = True
    # close random CV stuff
    cap.release()
    cv2.destroyAllWindows()
    # close program
    sys.exit(0);

signal.signal(signal.SIGINT, Stop)


# ========================
# Arduino comms
# ========================
# set up serial
ser = serial.Serial('/dev/ttyACM1', baudrate=9600)
ser.reset_input_buffer()
# globals
arduino_lock = False
linear_vel = 0
angular_vel = 0

# returns f as a list of 4 bytes, little endian
def floatToBits(f):
    s = struct.pack('>f', f)
    s = struct.unpack('>l', s)[0]
    # bit manipulation? in python? why would you want to do that??
    one = s % 256
    two = (s // 256) % 256
    three = (s // (256*256)) % 256
    four = (s // (256*256*256)) % 256
    return [one, two, three, four]

# returns b1-4 as a float, little endian
def bitsToFloat(b1, b2, b3, b4):
    val = b1 + (b2 * 256) + (b3 * 256*256) + (b4 * 256*256*256)
    s = struct.pack('>l', val)
    return struct.unpack('>f', s)[0]

# thread for sending/recieving
def ArduinoThread():
    # globals
    global stop_threads
    global arduino_lock
    arduino_lock = True
    global linear_vel
    global angular_vel
    # the actual thread code
    while True:
        # send speed information
        try:
            ser.write(struct.pack('<f', linear_vel))
            ser.write(struct.pack('<f', angular_vel))
        except:
            print("Arduino oopsies")
        # don't spam the poor little guy
        if stop_threads:
            return
        sleep(0.1)

    
# ========================
# Camera processing
# ========================
angle_deg = 0.0
distanceToMarker = 0
d = 0
Dm =  18 #This is the constant defining the size of the marker at 1 marker EDIT TO NEW MARKERS
x_max = 426
y_max = 240
actualCenterX = x_max/2
actualCenterY = y_max/2
center_x = 0
center_y = 0
pixels = 1
parameters = cv2.aruco.DetectorParameters()
parameters.adaptiveThreshWinSizeMin = 50  # You can adjust this parameter
parameters.adaptiveThreshWinSizeMax = 200
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, x_max)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, y_max)
aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_100)

def cameraProcessing():
    global markers_found
    global distanceToMarker
    global angle_deg
    ret, frame = cap.read()
    cv2.imshow('Aruco Marker Detection', frame)
    # Detect markers in the frame
    corners, ids, rejectedImgPoints = cv2.aruco.detectMarkers(frame, aruco_dict, parameters=parameters)
    if ids is not None:
        markers_found = True
        for i in range(len(ids)):
            # Get the center of the marker
            center_x = int(np.mean(corners[i][0][:, 0]))
            center_y = int(np.mean(corners[i][0][:, 1]))
            
            corner_x = (corners[i][0][:, 0])
            pixels = corner_x[1]-corner_x[0]

            # Draw a circle at the center of the marker
            cv2.circle(frame, (center_x, center_y), 5, (0, 255, 0), -1)

            # Display the X and Y coordinates
            cv2.putText(frame, f"X: {center_x}, Y: {center_y}", (center_x + 10, center_y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
            
        frame_center_x = frame.shape[1] / 2
        frame_center_y = frame.shape[0] / 2
        cv2.imshow('Aruco Marker Detection', frame)
        if ids is not None and len(ids) > 0:
            vector_x = center_x - frame_center_x
            angle_rad = np.arctan2(vector_x,  frame_center_x)
            angle_deg = 1*(120/180)*np.degrees(angle_rad)
        
    else:
        pixels = -1
        markers_found = False
        
    distanceToMarker = Dm/pixels;



# ========================
# Main thread
# ========================
markers_found = False
state = 'rotate'

while True:
    # potentially move this to it's own thread if we start having processing issues
    cameraProcessing()

    # check for reset signal from arduino
    if ser.in_waiting > 0:
        ch = ser.readline().decode('ascii').rstrip()
        if ch == 'r':
            # Arduino just reset, we should too
            state = 'rotate'
            print('resetting!! new state: rotate')
    

    targetAngle = 0
    targetDistance = .305

    Kp_L = 3; # tuning params
    Kp_A = 1; #

    # state machine    
    if state == 'rotate': # slowly spin until marker found
        linear_vel = 0
        angular_vel = 3.1415/16
        # state transition
        if (markers_found):
            state = 'forward'
            print('new state: forward')
    
    elif state == 'forward':
        # drive towards marker, stopping within 1 foot
        linear_vel = 25 * (distanceToMarker - targetDistance) * Kp_L
        angular_vel = (3.1415/180) * (angle_deg - targetAngle) * Kp_A
        # it gets confused when we don't see a marker
        if linear_vel < -5:
            linear_vel = -5.0
            angular_vel = 0.0
        # state transition
        if distanceToMarker <= targetDistance and distanceToMarker != -18:
            state = 'spin'
            print('new state: spin')
            
    elif state == 'spin':
        # arduino should recognize inf and take over
        linear_vel = float('NaN')
        angular_vel = float('NaN')
        # state transition
        state = 'done'
        print('new state: done!')
        playsound('./tada.wav')
        
    elif state == 'done':
        # let arduino control
        linear_vel = float('NaN')
        angular_vel = float('NaN')

 
    # start threads
    if not arduino_lock:
        print('starting arduino thread')
        ard_thread = Thread(target=ArduinoThread, args={})
        ard_thread.start()

    #print('state: ' + state + '     lin: ' + str(linear_vel) + '    ang: ' + str(angular_vel) + '      the marker distance is: ' + str(distanceToMarker))

    # end program using q key
    if cv2.waitKey(1) & 0xFF == ord('q'):
        Stop(0, 0) # pass it garbage? idk
        break


