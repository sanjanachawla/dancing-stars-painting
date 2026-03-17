# 🎵 Audio-Reactive LED Matrix (Arduino)

A real-time audio visualization system that captures sound through a microphone, performs frequency analysis using FFT, and maps the dominant frequency and volume to dynamic patterns on an LED matrix.

---

## Overview

This project uses an **Arduino Uno**, a **microphone sensor**, and a **WS2812B LED matrix** to create an audio-reactive light display.

- Audio is sampled via an analog microphone  
- A **Fast Fourier Transform (FFT)** extracts the dominant frequency  
- Frequency + volume are mapped to LED position and brightness  
- LEDs respond in real time to music or ambient sound  

---

## Features

- Real-time FFT-based frequency detection  
- Dynamic LED mapping across a 256-pixel matrix  
- Volume-normalized brightness scaling  
- Adaptive normalization (tracks min/max over time)  
- Fail-safe LED shutdown after inactivity  
- Multiple visualization modes (glimmer, gaussian spread)

---

## Hardware Requirements

- Arduino Uno  
- Analog microphone module (A0 + D0 output)  
- WS2812B LED strip / matrix (256 LEDs)  
- External power supply (recommended for LEDs)  
- Jumper wires  

---

## Wiring

| Component        | Arduino Pin |
|----------------|------------|
| Microphone A0   | A0         |
| Microphone D0   | D3         |
| LED Data        | D7         |
| LED Power       | 5V (external recommended) |
| Ground          | GND        |

---

## Software Dependencies

Install the following libraries in the Arduino IDE:

- `arduinoFFT`
- `FastLED`

---

## How It Works

### 1. Audio Sampling
- The microphone sends an analog signal to **A0**
- The signal is sampled at **2048 Hz**
- 128 samples are collected per cycle

### 2. Frequency Analysis
- FFT converts time-domain signal → frequency domain
- `MajorPeak()` extracts the dominant frequency

### 3. Normalization
- Tracks:
  - `highestPeak` / `lowestPeak` (frequency)
  - `highest_vol` / `lowest_vol` (volume)
- Values are dynamically normalized to maintain responsiveness

### 4. LED Mapping

| Input        | Output                  |
|-------------|-------------------------|
| Frequency   | LED position (0–255)    |
| Volume      | Brightness (0–255)      |

---

## Visualization Modes

### Glimmer Mode (Active)
- Lights a single LED based on frequency  
- Brightness tied to volume  
- Warm white/yellow tone  

### Gaussian Spread (Optional)
- Spreads light around the peak frequency  
- Color transitions across regions:
  - Red → Yellow → Green → Cyan → Blue  

### All-Off Mode
- Triggered when no sound is detected  
- Includes a **7-second fail-safe reset**  

---

## Key Parameters

```cpp
#define SAMPLES 128
#define SAMPLING_FREQUENCY 2048
#define NUM_LEDS 256
