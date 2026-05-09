# STM32-Spectrum-Analyzer
Firmware and Schematic design for a real-time, 64-band STM32 Audio Spectrum Analyzer.
# STM32 Audio Spectrum Analyzer

*Firmware and Hardware design files for a real-time, 64-band audio spectrum analyzer built from the ground up.*

## Overview
This project processes audio signals in real-time using an STM32 microcontroller and the ARM CMSIS-DSP library. It captures audio via a custom analog front-end, computes a 512-point FFT, and drives an ILI9341 display over a high-speed 8-bit parallel bus.

**Note:** Full hardware documentation—including Altium schematics, PCB layout renders, and wiring pinout diagrams—will be uploaded to this README shortly. 

## Tech Stack
* **Microcontroller:** STM32F1 series
* **DSP:** ARM CMSIS-DSP (Fast Fourier Transform)
* **Display:** 2.4" tft lcd shield (ILI9341) 
