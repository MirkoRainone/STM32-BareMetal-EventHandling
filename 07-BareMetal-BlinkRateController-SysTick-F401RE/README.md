# Blink Rate Controller: Pure Bare-Metal SysTick & EXTI

This project is a pure "bare-metal" embedded programming exercise (without the use of HAL or CMSIS libraries) developed for the **STM32 Nucleo-F401RE** board. 

The goal is to demonstrate the concurrent management of time (via **SysTick**) and asynchronous events (via **EXTI**), while also implementing a **software debouncing** system for mechanical buttons.

## 🚀 Features

* **Automatic Blinking:** An LED blinks in the background in an autonomous, non-blocking manner, driven by the system timer (SysTick).
* **Speed Control:** * Pressing Button 1 decreases the delay time (faster blinking).
  * Pressing Button 2 increases the delay time (slower blinking).
* **Software Debouncing:** Both buttons are protected from mechanical contact bounce using a 50ms time filter, based on the SysTick millisecond count.

## 🛠️ Key Concepts Demonstrated

* **Manual Memory Mapping:** Direct access to registers via pointers to physical memory addresses (`volatile uint32_t *`).
* **Bitwise Operations:** Safe register configuration using bitmasks (Read-Modify-Write, `|=`, `&= ~`, `^=`).
* **Interrupt Service Routines (ISR):** Handling system exceptions (`SysTick_Handler`) and peripheral interrupts (`EXTI15_10_IRQHandler`).
* **Inter-Interrupt Communication:** Use of the `volatile` keyword to share state variables between different ISRs without blocking the CPU in `main()`.

## 🧰 Hardware & Software Requirements

* **Board:** STM32 Nucleo-F401RE (or equivalent STM32F4 family).
* **IDE:** STM32CubeIDE (or any GCC toolchain for ARM).
* **External Components:** 2 standard mechanical push buttons.

## 🔌 Pinout and Connections

| Pin  | Function | Internal Configuration | External Connection |
| :--- | :--- | :--- | :--- |
| **PA5** | LED Output | Output (Push-Pull) | On-board Nucleo LED (LD2). |
| **PA10** | Button 1 (Faster) | EXTI10 (Input + Pull-Up) | Connect to GND via push button. |
| **PB12** | Button 2 (Slower) | EXTI12 (Input + Pull-Up) | Connect to GND via push button. |

*Note: Since internal Pull-Up resistors are enabled, the buttons must close the circuit to ground (GND). The interrupt is configured to trigger on the Falling Edge.*

## 🧠 How the Logic Works

1. **The Heart (SysTick):** Configured to generate an exception exactly every 1 ms (based on the 16 MHz HSI clock). Its ISR increments a global `ms_counter` and, if the `blink_delay` threshold is reached, toggles the logical state of pin PA5 (LED).
2. **The Ears (EXTI + NVIC):** EXTI lines 10 and 12 are listening on the physical pins. When they detect a falling edge (button pressed), they send a signal to the Nested Vectored Interrupt Controller (NVIC), which makes the processor jump to the `EXTI15_10_IRQHandler`.
3. **The Brain (The Filter):** When EXTI triggers, the ISR compares the current time (provided by SysTick) with the timestamp of the last recorded press. If the difference is greater than `DEBOUNCE_TIME` (50 ms), the press is considered valid, the shared `blink_delay` variable is updated, and the new timestamp is saved.
4. **The Main Loop:** The `while(1)` loop inside `main()` is completely empty. The entire application is hardware Event-Driven.

## ⚠️ Setup Warnings for STM32CubeIDE

Since the code is written entirely from scratch:
1. Ensure that in your startup file (e.g., `startup_stm32f401retx.s`), the routine names match `SysTick_Handler` and `EXTI15_10_IRQHandler` exactly.
2. If you are using the default clock (HSI), the system will run at 16 MHz. If you have configured the PLL for higher speeds (e.g., 84 MHz), remember to update the calculation in the `SYSTICK_LOAD` register.

---
*Project created as a learning exercise for the ARM Cortex-M4 architecture and Bare-Metal programming.*