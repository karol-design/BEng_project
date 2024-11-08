![HertzNet Logo](https://i.ibb.co/nDjV8Mm/Hertz-Net-Logo-blue-white.png)
## HertzNet | High-accuracy Wireless Mains Frequency Measurement System
BEng 3rd Year Individual Project | University of Manchester

### CI Workflow results

![ESP-IDF CI Build](https://github.com/karol-design/HertzNet/actions/workflows/esp-idf.yml/badge.svg) [![MATLAB](https://github.com/karol-design/HertzNet/actions/workflows/matlab.yml/badge.svg)](https://github.com/karol-design/HertzNet/actions/workflows/matlab.yml)


## About 
HertzNet is a system for measuring mains (utility/line) frequency in different locations and aggregating collected data.

The aim of this project was to design and build a system for real-time mains frequency monitoring,
consisting of a low-price plug-in devices with wireless communication, capable of measuring and
uploading accurate and high-resolution frequency data and a web interface which aggregates that
data and makes it searchable and well-presented. 

## Directories
- /board-design - Altium Design files, i.e. Schematics, PCB layouts and 3D models of current and previous versions of the HertzNet Measurement Unit
- /board-fw - Firmware for ESP32-WROOM-32E MCU which controls the HertzNet Measurement Unit
- /cloud-scripts - MATLAB script(s) for the HeartzNet's ThingsSpeak channel

## Contributing (Firmware)
Commit to the main only the code that compile without any warnings or errors.
To test, compile or flash the code use ESP-IDF 4.4

### HertzNet Proto 3.0
![3D Model for Proto 3.0 ](https://media.licdn.com/dms/image/v2/D4E22AQEgBaO1t34uLQ/feedshare-shrink_2048_1536/feedshare-shrink_2048_1536/0/1686078955866?e=1733961600&v=beta&t=h2UDcXQ_McMbzV3IVpuXrCgDkTPCk10d-FBV32_YIds)

<center>Figure 1. - 3D Model for Proto 3.0</center>
