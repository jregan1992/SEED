import cv2
import numpy as np
from smbus2 import SMBus
from time import sleep
import time
import math
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd
import board
from threading import Thread
from threading import Lock


ARD_ADDR = 8

lcd_columns = 16
lcd_rows = 2
lcdlock = Lock()


L = board.I2C()
lcd = character_lcd.Character_LCD_RGB_I2C(L, lcd_columns, lcd_rows)


def lcdomine():
    red = 150
    green = 0
    blue = 150
    lcdlock.acquire()
    lcd.message = str("angle: " + str(angle_deg))
    red = red - int(angle_deg)
    blue = blue - int(angle_deg)
    lcd.color = [red, green, blue]
    lcd.message = "\n" + str(red) + " " + str(green) + " " + str(blue)
    lcdlock.release()



angle_deg = 0.0
d = 0
x_max = 426
y_max = 240
actualCenterX = x_max/2
actualCenterY = y_max/2
center_x = 0
center_y = 0
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
            
            # Draw a circle at the center of the marker
            cv2.circle(frame, (center_x, center_y), 5, (0, 255, 0), -1)

            # Display the X and Y coordinates
            cv2.putText(frame, f"X: {center_x}, Y: {center_y}", (center_x + 10, center_y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
            
        frame_center_x = frame.shape[1] / 2
        frame_center_y = frame.shape[0] / 2
        cv2.imshow('Aruco Marker Detection', frame)
        if ids is not None and len(ids) > 0:
            vector_x = center_x - frame_center_x
            angle_rad = 120/180*np.arctan2(vector_x,  frame_center_x)
            angle_deg = np.degrees(angle_rad)
            print("")
            print("Angle between marker and center of the screen (degrees):", angle_deg)

    if lcdlock.locked() == False:
        myThread = Thread(target=lcdomine, args={})
        myThread.start()

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
