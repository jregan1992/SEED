
import cv2
import numpy as np
from smbus2 import SMBus
from time import sleep
import time
import math
##import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd
##import board



# I2C address of the Arduino, set in Arduino sketch
ARD_ADDR = 8
# Initialize SMBus library with I2C bus 1
i2c = SMBus(1)
right_wheel = int(0)
left_wheel = int(0)
##lcd_columns = 16
##lcd_rows = 2
##L = board.I2C()
##lcd = character_lcd.Character_LCD_RGB_I2C(L, lcd_columns, lcd_rows)
##lcd.clear()

d = 0
center_x = 0
center_y = 0
# Define the grid boundaries
x_min, x_max = 0, 500
y_min, y_max = 0, 500

# Define the grid divisions
x_div = (x_max - x_min) / 3
y_div = (y_max - y_min) / 3

# Initialize the camera
cap = cv2.VideoCapture(0)

# Load the ArUco
aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_100)

# Create parameters for the marker
parameters = cv2.aruco.DetectorParameters()

parameters.adaptiveThreshWinSizeMin = 50  # You can adjust this parameter

parameters.adaptiveThreshWinSizeMax = 200

while True:
    # Read a frame from the camera
    ret, frame = cap.read()
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
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
    
    # Display the frame
    cv2.imshow('ArUco Marker Detection', frame)
    #top left
    if ids is not None and len(ids) > 0:
        if center_x > x_min and center_x < x_min + x_div and center_y > y_min and center_y < y_min + y_div:
            right_wheel = 3
            left_wheel = 2
            
        #top mid    
        elif center_x > x_min + x_div and center_x < x_min + 2 * x_div and center_y > y_min and center_y < y_min + y_div:
            right_wheel = 3
            left_wheel = 3
        #top right
        elif center_x > x_min + 2 * x_div and center_x < x_max and center_y > y_min and center_y < y_min + y_div:
            right_wheel =  2
            left_wheel = 3
        #mid left
        elif center_x > x_min and center_x < x_min + x_div and center_y > y_min + y_div and center_y < y_min + 2 * y_div:
            right_wheel = 3
            left_wheel = 2
        #mid mid
        elif center_x > x_min + x_div and center_x < x_min + 2 * x_div and center_y > y_min + y_div and center_y < y_min + 2 * y_div:
            right_wheel = 2
            left_wheel = 2
        #mid right    
        elif center_x > x_min + 2 * x_div and center_x < x_max and center_y > y_min + y_div and center_y < y_min + 2 * y_div:
            right_wheel = 2
            left_wheel = 3
        #lower left
        elif center_x > x_min and center_x < x_min + x_div and center_y > y_min + 2 * y_div and center_y < y_max:
            right_wheel = 1
            left_wheel = 0
        #lower mid    
        elif center_x > x_min + x_div and center_x < x_min + 2 * x_div and center_y > y_min + 2 * y_div and center_y < y_max:
            right_wheel = 1
            left_wheel = 1
        #lower right
        elif center_x > x_min + 2 * x_div and center_x < x_max and center_y > y_min + 2 * y_div and center_y < y_max:
            right_wheel = 0
            left_wheel = 1
        else:
            print("no")
            right_wheel = 0
            left_wheel = 0
    else:
            print("no")
            right_wheel = 0
            left_wheel = 0

    
    # Exit the loop if the 'q' key is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    ##lcd.message = "R: "+str(right_wheel)+"  L: "+str(left_wheel)   
    i2c.write_block_data(ARD_ADDR, 0, [int(left_wheel), int(right_wheel)])
    #i2c.write_i2c_block_data(ARD_ADDR, 1, right_wheel)
    time.sleep(.1)










# Release the camera and close all windows
cap.release()
cv2.destroyAllWindows()




# Release the camera and close all windows
cap.release()
cv2.destroyAllWindows()
