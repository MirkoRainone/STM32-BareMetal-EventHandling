# 🎛️ STM32 Bare-Metal: Event-Driven Analog Dimmer (ADC to PWM)

## 📌 Overview
This project implements an **asynchronous, event-driven analog dimmer** using an STM32 microcontroller. It reads an analog voltage from a potentiometer via the ADC (Analog-to-Digital Converter) and instantly maps it to a hardware PWM (Pulse Width Modulation) signal to control the brightness of an LED or the speed of a DC motor.

The core achievement of this project is its **100% non-blocking architecture**. By leveraging Hardware Timers, the SysTick core peripheral, and Interrupt Service Routines (ISRs), the `main` `while(1)` loop remains completely empty, freeing up the CPU for other potential high-priority tasks.

## ⚙️ Hardware Setup
* **Microcontroller:** STM32F401RE (Nucleo-64 Board)
* **Input (Analog):** Potentiometer connected to Pin **PA0** (ADC1, Channel 0)
* **Output (PWM):** LED or Motor Driver connected to Pin **PA5** (TIM2, Channel 1, Alternate Function 1)

## 🏗️ System Architecture & Data Flow

This system relies on three interconnected hardware blocks:

1. **The Pacing (SysTick):** Configured to tick every 1ms. Every 50ms, the SysTick handler manually triggers the ADC software start (`SWSTART`).
2. **The Acquisition (ADC1):** Configured in 12-bit resolution mode. Once triggered, it reads the voltage on PA0 in the background. Upon completion, it raises an End-Of-Conversion (EOC) Interrupt.
3. **The Translation & Output (ISR + TIM2):** Inside the `ADC_IRQHandler`, the CPU briefly wakes up, reads the raw 12-bit value (0-4095), scales it to the PWM resolution (0-1000) using purely integer arithmetic, and updates the `TIM2_CCR1` register. The Timer then automatically handles the high-frequency (1 kHz) electrical output.

### 🧮 The Mapping Formula
Floating-point math is intentionally avoided to maximize CPU performance. The scaling is done via proportional integer arithmetic:
```c
uint32_t target_pwm = (raw_adc_val * 1000) / 4095;