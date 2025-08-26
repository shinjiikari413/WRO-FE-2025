# SSTEVE

This repository contains engineering materials of SSTEVE's self-driven vehicle model participating in the WRO Future Engineers competition in the season 2025.

## Content
* `t-photos` contains 2 photos of the team (an official one and a funny photo with all team members)
  * `Formal.jpeg` contains a formal picture of the team
  * `Informal.jpeg` contains a funny picture of the team
* `v-photos` contains 6 photos of the vehicle (from every side, from top and bottom)
  * `Back View.jpeg` contains 1 photo of the back view of our bot
  * `Bottom View.jpeg` contains 1 photo of the bottom view of our bot
  * `Front View.jpeg` contains 1 photo of the front view of our bot
  * `Left View.jpeg` contains 1 photo of the left view of our bot
  * `Right View.jpeg` contains 1 photo of the right view of our bot
  * `Top View.jpeg` contains 1 photo of the top view of our bot
* `video` contains the video.md file with the link to a video where a driving demonstration exists
* `schemes` contains one or several schematic diagrams in the form of JPEG, PNG or PDF of the electromechanical components illustrating all the elements (electronic components and motors) used in the vehicle and how they connect to each other.
  * `Overall System  Diagram.png` contains a photo of our block diagram
  * `Microcontroller System Diagram.png` contains a photo of our microcontroller system diagram
* `src` contains code of control software for all components which were programmed to participate in the competition
* `models` is for the files for models used by 3D printers, laser cutting machines and CNC machines to produce the vehicle elements. If there is nothing to add to this location, the directory can be removed.
  * `EvoTOF Lego Mount.stl` contains a 3D model of a mount for our time of flight sensor
  * `BNO055 Lego mount.stl` contains a 3D model of a mount for our gyro sensor
  * `EVO brick.stl` contains a 3D model of a housing for our microcontroller system
* `other` is for other files which can be used to understand how to prepare the vehicle for the competition. It may include documentation how to connect to a SBC/SBM and upload files there, datasets, hardware specifications, communication protocols descriptions etc. If there is nothing to add to this location, the directory can be removed.

## Introduction

### Usage
1.⁠ ⁠Install the libraries found in the src/lib/external_libraries by going to the library manager in the Arduino IDE and installing each of them through the Add .ZIP Library. ALL LIBRARIRES ARE REQUIRED FOR THE CODE TO RUN CORRECTLY. 
2.⁠ ⁠Install src/Evo.zip and follow the following on the Arduino IDE: Sketch --> Include Library --> Add .ZIP Library --> Select EvoEditted.zip. 
3.⁠ ⁠Open the desired file (i.e. mainopen.ino or mainobject.ino) with the Arduino IDE.

## Reminders Before Running Bot
- Software
  - Ensure that the board is set to ESP32S3
  - Ensure serial bot rate is 115200
  - Ensure all sensor values have been calibrated to the current environmental conditions
  - Run!
- Hardware
  - Wires have been connected properly
  - Components are working properly
  - Structural parts are connected properly
  - Spaces on the wheels are correct
- Placement of the bot
  - Make sure the bot is placed straight
  - Ensure gyro sensor is strongly secured on the bot

## Components used
- Microcontroller: EvolutionX1, ESP32S3 x1
- Motors: EV3 Medium motors x2
- Camera: HuskeyLens x1
- Distance Sensor: Time of Flight, VL53L0X x2
- Compass: IMU, BNO055 x1

