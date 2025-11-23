# 📘 ChibiOS Real-Time Examples — STM32G474RE

This repository contains a collection of **practical ChibiOS examples** developed for the bachelor thesis *“Real-Time Development with ChibiOS: Architecture, Programming, and Comparison with Other RTOS”*.

All projects are designed for the **STM32 Nucleo-G474RE** board and demonstrate the most important features of the ChibiOS/RT kernel and HAL, from basic GPIO handling to advanced real-time latency measurements.

A portion of these examples is inspired by and derived from the hands-on sessions taught during the **Neapolis Innovation Summer Campus**, where students were trained on embedded systems, STM32 microcontrollers, and ChibiOS development.

---

## 📂 Repository Contents

### 🔹 1. LED Blinking with Threads (GPIO + RT Kernel)

Introductory example covering:

* GPIO configuration with `palSetLineMode()`
* thread creation (`chThdCreateStatic`)
* deterministic periodic blinking using `chThdSleepMilliseconds`

---

### 🔹 2. Serial Communication (USART + ChibiOS Serial Driver)

Demonstrates how to use the ChibiOS Serial Driver (`SDx`):

* USART initialization and configuration
* I/O queues and timeout-based operations
* formatted output with `chprintf`

Reference document: `/mnt/data/USART.pdf`

---

### 🔹 3. OLED SSD1306 Display via I²C

Complete I²C example featuring:

* I²C bus initialization on PB8/PB9
* SSD1306 driver integration
* screen rendering from a dedicated thread

---

### 🔹 4. Real-Time Latency & Jitter Measurement

This is the main experimental setup used in **Chapter 3** of the thesis.
It includes:

* a 500 ms periodic task
* wake-time vs actual-time measurement
* latency computed using `chVTGetSystemTimeX`
* a buffer of 1000 samples
* jitter and average latency calculation
* additional CPU-load threads (integer and floating-point)
* OLED visualization + serial logging

---

### 🔹 5. Additional HAL Demos

Standalone examples showcasing:

* GPIO input (buttons)
* PWM generation (RGB LED)
* ADC analog measurements
* SPI communication structure

---

## 🎯 Purpose of the Repository

The goal of this repository is to provide **clear, modular, and reusable** examples for students, researchers, and embedded developers learning ChibiOS and STM32 real-time programming.

Each example is:

* **Educational** — with detailed comments inside the code.
* **Immediate to use** — fully compatible with ChibiStudio.
* **Technically aligned** with the official documentation:
  [https://chibiforge.org/doc/21.11/full_rm/modules.html](https://chibiforge.org/doc/21.11/full_rm/modules.html)

---

## 🛠️ Requirements

* **ChibiStudio** (latest release)
* **STM32 Nucleo-G474RE** development board
* USB cable
* Optional hardware:

  * SSD1306 OLED display
  * push buttons
  * RGB LED
  * analog sensors

---

## 📎 License

Released under the **MIT License**.
You are free to use, modify, and redistribute the examples.


