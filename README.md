# Electronic Skee Ball Machine

## Overview 

This repository contains the source code for the Electronic Skee Ball machine project. This project was created as a senior capstone design project for Purdue University's ECE47700 
course in Spring 2025. 
This project was created by: 
* **Andrew Cali (*Hardware Lead*)**
* **David Fall (*Team Lead*)**
* **Jennifer Ferguson (*Software Lead*)**
* **Madeline Flanagan (*Systems Lead*)**

all of whom are (or were at the time) computer engineering students at Purdue University. 

## Repository Structure
This repository is divided into numerous discrete sections. 
* **src**: This folder contains the embedded C source code for the project. The software in this directory is the final software that was run on the hardware (specifically, on the STM32F091RCT6 MCU that was used in this project). 
* **micropython**: This directory contains some micropython code that was used during early prototyping done on an ESP32 Feather v2 development board. This development board (or any type of ESP32) was not used in the final design of the project. None of the software in this directory was used in the final design of the project.
* **docs**: This folder contains any and all technical documentation for the project. This includes a product overview, functional specification, etc.
* **hardware**: This folder contains the KiCAD project files for the hardware of the project. This includes the schematic and PCB layout. 
* **packaging**: This folder contains the CAD files for the packaging of the project. This includes the design files for the enclosure and any other packaging components.


## Additional Documentation 

Any and all technical documentation, including a product overview, functional specification, etc., can be found in the `docs` folder in this repository.

## Hardware Design
The hardware of the project, including the schematic and PCB layout, is designed using KiCAD. The KiCAD project files can be found in the `hardware` folder. 

## Packaging Design 
The packaging for the product hardware and user interface was designed using a variety of softwares. The main components of the packaging design were designed using CAD. Relevant CAD files are contained in the `packaging` folder in this repository. 

Various resources at Purdue University were used to create the packaging. These include but are not limited to university-provided 3D printers and university-owned woodworking tools.

## Reusability 
The software contained in this repository was designed specifically for the hardware of this project. It is not intended for reuse on alternative hardware configurations. However, should one order the exact PCB that was designed for this project and incorporate the components listed in the BOM, the software should work as intended.

## Contact Information
At the time of reading this, it is possible that the team members have graduated from Purdue University, meaning that they may not be reachable at their Purdue email addresses. For any questions or comments, please contact David Fall at his personal email address 
[david.c.fall33@gmail.com](mailto:david.c.fall33@gmail.com).

