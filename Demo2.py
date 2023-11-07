# import *
import cv2
import numpy as np
from smbus2 import SMBus
from time import sleep
import time
import math
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd
import board
import struct
from threading import Thread
from threading import Lock
import signal
import sys


# VERY IMPORTANT, KILLS THREADS
stop_threads = False

def Stop(sig, frame):
    global stop_threads
    
    print("... attempting to kill threads ...")
    stop_threads = True
    sys.exit(0);

signal.signal(signal.SIGINT, Stop)


# ========================
# Arduino comms
# ========================
# constants
ARD_ADDR = 8
I2C = SMBus(1)
# globals
arduino_lock = False
linear_vel = 0
angular_vel = 0
heading = 0

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
    global linear_vel
    global angular_vel
    # the actual thread code
    while True:
        arduino_lock = True
        # send speed information
        I2C.write_block_data(ARD_ADDR, 0, floatToBits(linear_vel) + floatToBits(angular_vel))
        # don't spam the poor little guy
        if stop_threads:
            return
        sleep(0.1)

    
# ========================
# Camera processing
# ========================





# ========================
# Main thread
# ========================
angle_deg = 0.0
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
# Load the ArUco
aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_100)


while True:
    # Read a frame from the camera
    ret, frame = cap.read()
    cv2.imshow('Aruco Marker Detection', frame)
    # Detect markers in the frame
    corners, ids, rejectedImgPoints = cv2.aruco.detectMarkers(frame, aruco_dict, parameters=parameters)
    if ids is not None:
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
            angle_deg = -1*(120/180)*np.degrees(angle_rad)
    else:
        pixels = -1
        
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    print('the marker distance is ' + str(Dm/pixels))
    distanceToMarker = Dm/pixels;
    
# vision code ends about here

    targetAngle = 10
    targetDistance = .305

    Kp_L = 4;
    Kp_A = 2;

    
    if (distanceToMarker <= .305):
        linear_vel = 0;
        angular_vel = (3.1415/180) * (angle_deg - targetAngle);
        #print('made it too marker')
    #if not within 1 foot
    elif(distanceToMarker > .305):
        linear_vel = 10 * (distanceToMarker - targetDistance) * Kp_L
        angular_vel = (3.1415/180) * (angle_deg - targetAngle)  
        #print('linear: ' + str(linear_vel) + ', angular: ' + str(angular_vel))

    # start threads
    if not arduino_lock:
        print('starting arduino thread')
        ard_thread = Thread(target=ArduinoThread, args={})
        ard_thread.start()

    print('heading: ' + str(heading))
    
    sleep(1)
 
cap.release()
cv2.destroyAllWindows()
