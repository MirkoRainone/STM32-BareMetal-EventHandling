# STM32 Level 2: Event Handling & Hardware Timers

Welcome to the second stage of my **STM32 Bare-Metal Programming** journey. This repository marks the transition from basic GPIO manipulation to **Event-Driven Programming**. 

Instead of wasting CPU cycles with continuous polling, I am now leveraging the internal hardware controllers (**NVIC**, **EXTI**, **SYSCFG**) to handle real-time events efficiently.

## 🚀 Learning Path: Level 2
* [**04 - Interrupts (EXTI & NVIC):**](/05-BareMetal-Hardware-Interrupt-F401RE/) : Handling external signals without blocking the CPU.


---

## 📂 Featured Exercise: EXTI4 Hardware Interrupt
In this exercise, I configured an external trigger on pin **PB4** to toggle the onboard Green LED (**PA5**) using a **Rising Edge** interrupt.
