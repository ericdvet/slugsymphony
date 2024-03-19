# Air Rock Band

## Overview
Air Rock Band is a project that enables users to play virtual musical instruments through hand gestures using a pair of gloves equipped with sensors. Developed as a part of the ECE 167: Sensors and Sensing Technology course, this project offers an immersive and intuitive musical experience.

## Requirements
To set up the project, ensure you have Python, MPLABX, and the XC32 compiler installed on your system. Use pip to install the required dependencies listed in `requirements.txt` by running:

```bash
pip install -r requirements.txt
```

## Hardware
- Pair of gloves: Interface for hand gestures and finger movements
- 10 flex sensors: Attached to each finger to detect bending
- 1 capacitive touch sensor
- 1 Uno32: Processing sensor data and generating musical output
- Speaker: Outputs the generated musical tones
- 2 9DOF sensors: Attached to the back of the gloves to track hand orientation in 3D space
- PIC32MX340FL520H Microcontroller w/ CHIPKIT Basic I/O shield

## Implementation
The project consists of five virtual instruments:
1. Guitar: Mimics guitar playing with hand gestures.
2. Drums: Simulates drumming movements for rhythm creation.
3. Saxophone: Replicates saxophone playing gestures for note modulation.
4. Piano: Allows users to play virtual piano keys through hand movements.
5. Trumpet: Emulates trumpet playing techniques with hand gestures.

## Usage
After setting up the hardware and installing dependencies, run the slugsumphony.X project while running the ToneGenerator.py script to start playing virtual instruments using hand gestures.

## Acknowledgements
This project acknowledges the use of MPLABX, an integrated development environment (IDE) for microcontroller programming, which facilitated the development of the Uno32-based components of the project.

## Contributors
- Clarissa Farfan, clmfarfa@ucsc.edu
- Jose Mario Gutierrez, jgutie80@ucsc.edu
- Karla Vazquez, kplopezv@ucsc.edu
- Jerret Schlosser, jrschlos@ucsc.edu
- Eric David Vetha, evetha@ucsc.edu

For any inquiries or collaborations, please contact the group members via their provided email addresses.
