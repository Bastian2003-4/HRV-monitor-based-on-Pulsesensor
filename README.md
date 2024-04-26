# HRV-monitor-based-on-Pulsesensor
HRV monitor based on Pulsesensor
https://github.com/Bastian2003-4/HRV-monitor-based-on-Pulsesensor/tree/main

**Overview**

The project is dedicated to implementing HRV measurements on hardware designed for heart rate measurement. The core challenge of the project was to reduce interference from small amplitude motion through software algorithms to improve accuracy and fault tolerance.

The system has two modes of operation: one that provides real-time results during the measurement phase via the USB port, and another that delivers the aggregated results via the BLE after a predetermined interval.

Once connected to a PC, the microcontroller transmits real-time data—including raw PPG signals, IBI, and instantaneous heart rate—via the USB port, which can be displayed using a PC visualiser software. Pressing the user button resets the heartbeat and data counters, logs IBIs and heartbeat counts, and initiates a timeout. After the timeout, it calculates the HRV and transmits this data through BLE.

PC Visualiser: available at https://github.com/WorldFamousElectronics/PulseSensor_Amped_Processing_Visualizer

![image](https://github.com/Bastian2003-4/HRV-monitor-based-on-Pulsesensor/assets/115745326/bb0772b8-d366-4b02-84f5-839d8a5a3712)

Real-time data mode

![image](https://github.com/Bastian2003-4/HRV-monitor-based-on-Pulsesensor/assets/115745326/b94696b1-8d3b-4f4d-a6b2-bcf9eae07634)

Aggregated result mode

