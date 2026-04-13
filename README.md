# STM32 Level 2: Event Handling & Hardware Timers

Welcome to the second stage of my **STM32 Bare-Metal Programming** journey. This repository marks the transition from basic GPIO manipulation to **Event-Driven Programming**. 

Instead of wasting CPU cycles with continuous polling, I am now leveraging the internal hardware controllers (**NVIC**, **EXTI**, **SYSCFG**) to handle real-time events efficiently.

## 🚀 Learning Path: Level 2
* [**05 - Interrupts (EXTI & NVIC):**](/05-BareMetal-Hardware-Interrupt-F401RE/) : Handling external signals without blocking the CPU.
* [**06 - Shared Interrupts & Internal Pull-Ups:**](/06-BareMetal-ModeSelector-F401RE/) : How to handle multiple External Interrupts (EXTI) that share the same hardware Interrupt Request (IRQ).
* [**07 - Blink Rate Controller: Pure Bare-Metal SysTick & EXTI:**](/07-BareMetal-BlinkRateController-SysTick-F401RE/) : The goal is to demonstrate the concurrent management of time and asynchronous events
* [**08 - General Purpose Hardware Timers (TIMx):**](/08-BareMetal-LED-Blinker-TIMx-F401RE/) : To blink the on-board LED (PA5) exactly every 500 milliseconds using `TIM2` interrupts.
* [**09 - Hardware PWM Breathing LED with EXTI Control: **](/09-BareMetal-Fade-In-Fade-Out-Hardware-F401RE/) :  Hardware-driven breathing LED effect (fade-in / fade-out) using Pulse Width Modulation (PWM), combined with real-time speed control via External Interrupts (EXTI). 
*[**10 - Event-Driven Analog Dimmer (ADC to PWM)**](/10-BareMetal-EventDriven-ADC-PWM-F401RE/) : Asynchronous, event-driven analog dimmer.
