from ultralytics import YOLO
import cv2
import serial
import time

model = YOLO('yolov10n.pt')
arduino = serial.Serial("COM4", 9600, timeout=1)

left_flag = False
right_flag = False
center_flag = False

'''Defining The Cases :
000- No obstacle : code - 'a'
001- obstacle on right : code - 'b'
010- obstacle on center : code - 'c'
011- obstacle on right and center : code - 'd'
100- obstacle on left : code - 'e'
101- obstacle on left and right : code - 'f'
110-obstacle on right : code - 'g'
111-obstacle on whole frame : code - 'h'
'''

cap = cv2.VideoCapture(1)
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH)) / 3

left = frame_width
center = frame_width * 2
right = frame_width * 3

while True: 
    ret, frame = cap.read()
    
    # Reset the flags at the start of each frame processing
    left_flag = False
    center_flag = False
    right_flag = False
    
    # Flag to track if any object is detected
    object_detected = False

    results = model(frame)
    for result in results:
        boxes = result.boxes
        for box in boxes:
            if box.cls.item() == 39:  # Detect bottle or other object (class 39 in COCO dataset)
                object_detected = True
                x1, y1, x2, y2 = map(int, box.xyxy[0])
                label = result.names[box.cls[0].item()]
                pos = (x1 + x2) / 2
                
                if pos < left:
                    cv2.putText(frame, 'Obstacle on left', (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 0, 0), 2)
                    left_flag = True
                elif left < pos < center:
                    cv2.putText(frame, 'Obstacle on center', (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 0, 0), 2)
                    center_flag = True
                elif pos > center:
                    cv2.putText(frame, 'Obstacle on right', (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 0, 0), 2)
                    right_flag = True
                
                cv2.rectangle(frame, (x1, y1), (x2, y2), (255, 0, 0), 2)

    # Now check the flags and send the appropriate code
    if not object_detected:
        code = 'a'
        arduino.write(b'a')
        time.sleep(0.1)
    elif not left_flag and not center_flag and right_flag:
        code = 'b'
        arduino.write(b'b')
        time.sleep(0.1)
    elif not left_flag and center_flag and not right_flag:
        code = 'c'
        arduino.write(b'c')
        time.sleep(0.1)
    elif not left_flag and center_flag and right_flag:
        code = 'd'
        arduino.write(b'd')
        time.sleep(0.1)
    elif left_flag and not center_flag and not right_flag:
        code = 'e'
        arduino.write(b'e')
        time.sleep(0.1)
    elif left_flag and not center_flag and right_flag:
        code = 'f'
        arduino.write(b'f')
        time.sleep(0.1)
    elif left_flag and center_flag and not right_flag:
        code = 'g'
        arduino.write(b'g')
        time.sleep(0.1)
    elif left_flag and center_flag and right_flag:
        code = 'h'
        arduino.write(b'h')
        time.sleep(0.1)
    
    # Output the code for the current situation
    print(f"Code: {code}")

    cv2.imshow('Yolov8', frame)
    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()
