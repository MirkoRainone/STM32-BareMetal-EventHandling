# Introduction to Hardware Timers (TIMx) in Bare-Metal

This project serves as an introduction to STM32 **General Purpose Hardware Timers (TIMx)** using pure bare-metal C programming. It demonstrates how to replace the `SysTick` timer with `TIM2` to generate highly accurate periodic events (interrupts) without keeping the CPU busy.

## 🎯 Project Goal
To blink the on-board LED (PA5) exactly every 500 milliseconds using `TIM2` interrupts. The `while(1)` loop in the `main` function remains completely empty, showcasing true event-driven programming and CPU offloading.

## 🧠 Why TIMx instead of SysTick?
While `SysTick` is a great universal timer built into the ARM Cortex core (often used for OS ticks or simple delays), STM32 microcontrollers come with advanced peripheral timers (`TIM2`, `TIM3`, etc.) that offer superior capabilities:
- They run on separate peripheral buses (APB1/APB2).
- They have their own dedicated interrupt lines.
- They form the foundation for advanced features like **PWM (Pulse Width Modulation)**, Input Capture, and Output Compare.

## 🛠️ Key Concepts Explained

### 1. Peripheral Buses (AHB vs APB)
Unlike GPIO pins which sit on the high-speed AHB1 bus, standard timers reside on the **APB1 (Advanced Peripheral Bus)**. To turn on TIM2, we must route power to it by enabling the correct bit in the `RCC_APB1ENR` register.

### 2. The Time Machine: PSC and ARR
To tame the blazing fast 16 MHz system clock, timers use two fundamental registers:
* **`TIMx_PSC` (Prescaler):** Acts as a frequency divider. By setting it to `16000 - 1`, we slow the 16 MHz clock down to a manageable **1 kHz** (1 tick = 1 millisecond).
* **`TIMx_ARR` (Auto-Reload Register):** Defines the "finish line". By setting it to `500 - 1`, the timer counts from 0 to 499 (500 ms total) before resetting and triggering an event.

### 3. The Communication: DIER and SR
* **`TIMx_DIER` (Interrupt Enable):** Gives the timer permission to send an interrupt signal to the CPU when it reaches the ARR value.
* **`TIMx_SR` (Status Register):** Contains the "Update Interrupt Flag". **Crucial Note:** The hardware raises this flag automatically, but the software *must* clear it (`TIM2_SR &= ~(1 << 0);`) inside the ISR to prevent infinite interrupt loops.

## 🔌 Pinout & Hardware

| Component | Pin | Configuration |
| :--- | :--- | :--- |
| **User LED (LD2)** | `PA5` | Output Push-Pull |
| **TIM2 Core** | Internal | APB1 Bus, IRQ 28 |

## 🚀 How to Run
1. Create a new bare-metal project in STM32CubeIDE for the **Nucleo-F401RE**.
2. Ensure you are not using HAL or LL libraries.
3. Replace the contents of `main.c` with the provided code.
4. Ensure your startup file points the IRQ 28 exception to `TIM2_IRQHandler`.
5. Compile and flash!

---
*Developed as part of an STM32 bare-metal learning roadmap.*