# 🫁 STM32 Bare-Metal: Hardware PWM Breathing LED with EXTI Control

![Microcontroller](https://img.shields.io/badge/MCU-STM32F401RE-blue.svg)
![Architecture](https://img.shields.io/badge/Architecture-ARM_Cortex--M4-orange.svg)
![Framework](https://img.shields.io/badge/Framework-Bare--Metal_(No_HAL)-red.svg)
![Language](https://img.shields.io/badge/Language-C-green.svg)

## 📌 Overview
This project implements a hardware-driven **breathing LED effect (fade-in / fade-out)** using Pulse Width Modulation (PWM), combined with real-time speed control via External Interrupts (EXTI). 

Built entirely in **Bare-Metal C** without external libraries (No HAL, No LL), this project demonstrates how to offload high-frequency tasks to hardware peripherals while maintaining responsive, asynchronous user inputs using interrupts and software debouncing.

## ⚙️ Hardware Setup
* **Microcontroller:** STM32F401RE (Nucleo-64 Board)
* **Output (PWM):** Onboard LED connected to Pin **PA5** (TIM2, Channel 1)
* **Inputs (EXTI):** * Button 1 connected to **PA10** (Increases breathing speed)
  * Button 2 connected to **PB12** (Decreases breathing speed)

## 🏗️ System Architecture & Mechanics

This system integrates three core peripherals working concurrently without blocking the main CPU execution:

1. **The PWM Engine (Hardware Timer - TIM2):** * Configured in *PWM Mode 1* with a 1 kHz frequency (1000 µs period).
   * Generates the electrical signal directly on the `PA5` pin via Alternate Function (`AF1`), completely bypassing the CPU.
2. **The Event Listeners (EXTI & NVIC):**
   * Physical buttons trigger falling-edge interrupts on lines `EXTI10` and `EXTI12`.
   * The `EXTI15_10_IRQHandler` dynamically modifies a shared global variable (`fade_delay`) to alter the animation speed in real-time.
3. **The Pacing & Debouncing (SysTick):**
   * Ticking at 1 ms intervals, the SysTick core timer provides a reliable timebase.
   * Used within the EXTI handler to implement a **50ms non-blocking software debounce window**, filtering out mechanical switch noise.
   * Used within the `while(1)` loop to act as a non-blocking delay for the fade animation frames.

## 🚀 Key Features & Learning Outcomes
* **Hardware Autonomy:** Utilizing Timer Capture/Compare (`CCMR1`, `CCER`, `CCR1`) registers to drive physical outputs without CPU overhead.
* **Alternate Functions:** Multiplexing GPIO pins to internal peripherals (`AFRL`).
* **Interrupt Prioritization:** Managing shared IRQ handlers (EXTI15_10) and identifying specific trigger sources via the Pending Register (`PR`).
* **Non-Blocking Logic:** Implementing software timers based on `SysTick` to avoid CPU-halting `for` loops.

## 🛠️ How to Run
1. Clone this repository and open the project in **STM32CubeIDE**.
2. Compile the code and flash it to your STM32F401RE board.
3. Observe the onboard LED (`PA5`) fading in and out continuously.
4. Press the button connected to `PA10` to make the breathing effect faster.
5. Press the button connected to `PB12` to make the breathing effect slower.