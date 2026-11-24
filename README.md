# AI Fire Detection System

An experimental fire detection project that combines a camera-based YOLO model with an Arduino and smoke sensor. The Python program checks camera frames for a `fire` class and sends a one-character status over serial. The Arduino also reads a smoke sensor and controls an LCD, buzzer, LED, and relay.

> **Safety:** This is an educational prototype, not a certified fire alarm or life-safety system. Do not rely on it to protect people or property. Test it only in a safe, supervised setting, and use certified detection equipment for real protection.

## Project structure

```text
ai-fire-detection-system/
├── Arduino/
│   └── Arduino.ino   # Arduino firmware
├── Models/
│   └── best.pt       # Trained YOLO weights
├── python/
│   └── main.py       # Camera detection and serial communication
└── README.md
```

The Python program builds the model path relative to its own file, so it expects the weights at `Models/best.pt` in the project root.

## Hardware used by the sketch

- Arduino board with USB serial connection
- Smoke sensor with analog output connected to `A0`
- Active buzzer connected to digital pin `8`
- LED connected to digital pin `9` (use an appropriate current-limiting resistor)
- Relay module input connected to digital pin `7`
- 16×2 I2C LCD at address `0x27`
- Camera accessible to the computer running Python

Check your specific modules' voltage, current, and relay wiring requirements before connecting them. The sketch sets the buzzer, LED, and relay LOW at startup and sets them HIGH on an alarm.

## Software requirements

- Arduino IDE
- Python 3
- Python packages: `ultralytics`, `opencv-python`, and `pyserial`
- Arduino library: `LiquidCrystal_I2C`

Install the Python packages from a terminal:

```bash
python -m pip install ultralytics opencv-python pyserial
```

## Setup and run

1. Open `Arduino/Arduino.ino` in Arduino IDE. Install the `LiquidCrystal_I2C` library if needed, select your board and port, then upload the sketch.
2. Connect the camera and Arduino to the computer.
3. In `python/main.py`, set `ARDUINO_PORT` to the port assigned to your Arduino. The current setting is `COM12` on Windows; it will differ on other computers.
4. Make sure the baud rate in the Python program and Arduino sketch is the same (`9600`).
5. From the project root, start the Python program:

   ```bash
   python python/main.py
   ```

6. Press `q` in the camera window to quit. On normal exit, the program sends the safe (`S`) signal and closes the camera and serial connection.

## Detection and Arduino behavior

- Python opens camera index `0` and runs the YOLO model on each frame.
- Detections below confidence `0.50` are ignored. An accepted detection whose class name is `fire` sends `F`; otherwise, a changed status sends `S`.
- The Arduino raises the alarm when it receives `F` **or** when the smoke reading on `A0` is above `300` for four consecutive loop readings.
- In alarm mode the sketch turns on the buzzer, LED, and relay, and displays an alert. In safe mode it turns them off and displays the smoke reading.

The threshold, confidence, camera index, serial port, and LCD I2C address may need adjustment for your hardware and environment. Confirm the trained model's class names include `fire` (case-insensitive), because the Python code checks for that exact class name.

## Model file and GitHub

`Models/best.pt` is a trained model binary. GitHub blocks regular Git files over 100 MiB; this project's model is currently about 57 MB, so push it using Git or GitHub Desktop rather than uploading it through the browser. For larger weights, use Git LFS or provide a separate download link. See [GitHub's large file guidance](https://docs.github.com/en/repositories/working-with-files/managing-large-files/about-large-files-on-github).

## License

No license has been selected yet. Add a `LICENSE` file before indicating how others may reuse or modify this project.
