# HRV-monitor-based-on-Pulsesensor
HRV monitor based on Pulsesensor
https://github.com/Bastian2003-4/HRV-monitor-based-on-Pulsesensor/tree/main

**Introduction**

The project is dedicated to implementing HRV measurements on hardware designed for heart rate measurement. The core challenge of the project was to reduce interference from small amplitude motion through software algorithms to improve accuracy and fault tolerance.

The system has two modes of operation: one that provides real-time results during the measurement phase via the USB port, and another that delivers the aggregated results via the BLE after a predetermined interval.

Once connected to a PC, the microcontroller transmits real-time data—including raw PPG signals, IBI, and instantaneous heart rate—via the USB port, which can be displayed using a PC visualiser software. Pressing the user button resets the heartbeat and data counters, logs IBIs and heartbeat counts, and initiates a timeout. After the timeout, it calculates the HRV and transmits this data through BLE.


Wiring Diagram
![3ca44f52a0e6abbf76bd18c15f411fa](https://github.com/Bastian2003-4/HRV-monitor-based-on-Pulsesensor/assets/115745326/ee46647a-835f-4e23-88bf-68ebe735b813)


Real-time data mode

PC Visualiser: available at https://github.com/WorldFamousElectronics/PulseSensor_Amped_Processing_Visualizer

![image](https://github.com/Bastian2003-4/HRV-monitor-based-on-Pulsesensor/assets/115745326/bb0772b8-d366-4b02-84f5-839d8a5a3712)


Aggregated result mode

![image](https://github.com/Bastian2003-4/HRV-monitor-based-on-Pulsesensor/assets/115745326/b94696b1-8d3b-4f4d-a6b2-bcf9eae07634)


**Installation and running**

The system’s firmware is developed in C++ (Keil Studio Cloud platform, Mbed OS 6.16; Arm Limited, Cambridge, UK). This is an online IDE and does not need to be downloaded for use.

Step1:

Open Keil Studio using a browser https://studio.keil.arm.com/

Step2:

Click File, create a new Mbed Project, select Mbed OS 6, EMPTY Mbed OS project, and name the project.

Step3:

Download ToVisualiser.cpp from the repo and copy its contents into main.cpp. Then select build target as DISCO-L562QE and click build project to compile the project.

Step4:

Install and open STM32CubeProgrammer software for all STM32 at https://www.st.com/en/development-tools/stm32cubeprog.html.

Step5:

Click on the "Erasing and Programming" tab and select the file just compiled. Tick Verify Programming, Run after programming, then click Start Programming.

Step6:

Open Visualiser according to the Visualiser tutorial, Place  finger on the sensor to get data.

**More technical details**
The project uses dynamic thresholds to identify heartbeats and uses the SDNN standard to calculate HRV

![image](https://github.com/Bastian2003-4/HRV-monitor-based-on-Pulsesensor/assets/115745326/a668fbb7-9d75-4377-aa67-afc66b8ddc40)

Dynamic Threshold Algorithm

![image](https://github.com/Bastian2003-4/HRV-monitor-based-on-Pulsesensor/assets/115745326/d3786b7c-6c40-493a-8fd0-204b299d2a9c)

HRV formula

**Known Issues**

Although current algorithms have been optimised for motion artefacts, they are mainly for small movements such as breathing. It does not help with large movements such as arm movements and will need to be optimised in future versions.
