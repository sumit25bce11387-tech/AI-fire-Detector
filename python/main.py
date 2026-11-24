import cv2
import serial
import time
from ultralytics import YOLO

# -----------------------------
# SETTINGS
# -----------------------------

from pathlib import Path
MODEL_PATH = Path(__file__).resolve().parent.parent / "Models" / "best.pt"

ARDUINO_PORT = "COM12"
BAUD_RATE = 9600

CONFIDENCE = 0.50


# -----------------------------
# LOAD YOLO MODEL
# -----------------------------

print("Loading YOLO model...")

model = YOLO(MODEL_PATH)

print("REAL FIRE MODEL LOADED")
print("Classes:", model.names)


# -----------------------------
# CONNECT ARDUINO
# -----------------------------

arduino = serial.Serial(
    ARDUINO_PORT,
    BAUD_RATE,
    timeout=1
)

time.sleep(2)

print("Connected to Arduino successfully!")


# -----------------------------
# CAMERA
# -----------------------------

cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("ERROR: Camera not found!")
    exit()


# -----------------------------
# MAIN LOOP
# -----------------------------

last_signal = "S"

while True:

    ret, frame = cap.read()

    if not ret:
        print("Camera frame error")
        break


    # YOLO detection
    results = model(frame, verbose=False)


    fire_found = False


    for result in results:

        boxes = result.boxes

        for box in boxes:

            confidence = float(box.conf[0])

            if confidence < CONFIDENCE:
                continue


            class_id = int(box.cls[0])

            class_name = model.names[class_id]


            # Draw detection
            x1, y1, x2, y2 = map(
                int,
                box.xyxy[0]
            )

            cv2.rectangle(
                frame,
                (x1, y1),
                (x2, y2),
                (0, 0, 255),
                2
            )

            cv2.putText(
                frame,
                f"{class_name} {confidence:.2f}",
                (x1, y1 - 10),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.6,
                (0, 0, 255),
                2
            )


            # FIRE DETECTED
            if class_name.lower() == "fire":
                fire_found = True


    # -----------------------------
    # SEND TO ARDUINO
    # -----------------------------

    if fire_found:

        signal = "F"

    else:

        signal = "S"


    # Only send when signal changes
    if signal != last_signal:

        arduino.write(signal.encode())

        print("Arduino signal:", signal)

        last_signal = signal


    # Display
    cv2.imshow(
        "YOLO11 Fire Detection",
        frame
    )


    # Press Q to quit
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break


# -----------------------------
# CLEANUP
# -----------------------------

arduino.write(b"S")

arduino.close()

cap.release()

cv2.destroyAllWindows()