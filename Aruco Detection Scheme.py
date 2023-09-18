# Capture a reference image for white balance calibration
_, reference_frame = cap.read()

# Compute auto-white-balance gains
gains = cv2.awbGains(cap)
print("Auto White Balance Gains:", gains)

import cv2
import numpy as np

# Initialize the camera (0 for default camera)
cap = cv2.VideoCapture(0)
# Set the white balance gains manually
cap.set(cv2.CAP_PROP_AUTO_WB, 0)  # Turn off auto white balance
cap.set(cv2.CAP_PROP_XI_AUTO_WB, 0)  # For some cameras, you may need this as well
cap.set(cv2.CAP_PROP_TEMPERATURE, 5000)  # Adjust temperature as needed
cap.set(cv2.CAP_PROP_TINT, 0)  # Adjust tint as needed
cap.set(cv2.CAP_PROP_GAIN, gains[0])  # Set the red gain
cap.set(cv2.CAP_PROP_BLUE_GAIN, gains[1])  # Set the blue gain
cap.set(cv2.CAP_PROP_GREEN_GAIN, gains[2])  # Set the green gain

aruco_dict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_6X6_250)
aruco_params = cv2.aruco.DetectorParameters_create()

while True:
    # Read a frame from the camera
    ret, frame = cap.read()
    if not ret:
        break

    # Convert the frame to grayscale for marker detection
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Detect Aruco markers
    corners, ids, rejected = cv2.aruco.detectMarkers(
        gray, aruco_dict, parameters=aruco_params
    )

    # Draw detected markers on the frame
    if ids is not None:
        cv2.aruco.drawDetectedMarkers(frame, corners, ids)

    # Display the frame
    cv2.imshow("Aruco Detection", frame)

    # Exit when the 'q' key is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the camera and close all OpenCV windows
cap.release()
cv2.destroyAllWindows()
