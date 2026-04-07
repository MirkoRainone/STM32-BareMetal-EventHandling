# STM32 F401RE Bare-Metal External Interrupt (EXTI)

A low-level C implementation to demonstrate how to handle **External Interrupts (EXTI)** on the **STM32F401RE (Nucleo-64)** without using HAL or LL libraries. This project maps hardware registers directly to control a LED via an external signal.

## 📌 Overview

The goal of this project is to toggle the onboard Green LED (**PA5**) whenever a Rising Edge signal is detected on pin **PB4**. This is achieved by manually configuring the clock, GPIO, System Configuration (SYSCFG), External Interrupt Controller (EXTI), and the Nested Vectored Interrupt Controller (NVIC).

## 🛠️ Hardware Requirements
- **Microcontroller**: STM32F401RE (ARM Cortex-M4)
- **Board**: NUCLEO-F401RE
- **Peripheral**: An external button or signal generator connected to pin `PB4`.

## 🏗️ The Interrupt Chain
The signal travels through the following hardware blocks:
1. **GPIO**: Physical signal enters through Port B, Pin 4.
2. **SYSCFG**: Acts as a multiplexer to route Port B to the EXTI4 line.
3. **EXTI**: Detects the "Rising Edge" (0V to 3.3V transition) and raises a pending flag.
4. **NVIC**: The core controller that stops the CPU and jumps to the Interrupt Service Routine (ISR).

## 📂 Key Register Mappings

| Block | Register | Address | Purpose |
| :--- | :--- | :--- | :--- |
| **RCC** | `AHB1ENR` | `0x40023830` | Enable GPIOA and GPIOB Clocks |
| **RCC** | `APB2ENR` | `0x40023844` | Enable SYSCFG Clock |
| **SYSCFG**| `EXTICR2` | `0x4001380C` | Connect EXTI4 line to Port B |
| **EXTI** | `IMR` | `0x40013C00` | Unmask (Enable) Interrupt Line 4 |
| **EXTI** | `RTSR` | `0x40013C08` | Set Trigger to Rising Edge |
| **NVIC** | `ISER0` | `0xE000E100` | Enable IRQ 10 (EXTI4) in the Core |

## 🚀 Code Structure

### 1. Initialization
- Enable peripheral clocks.
- Configure `PA5` as Output and `PB4` as Input.
- Route `PB4` to `EXTI4` using `SYSCFG_EXTICR2`.
- Enable the interrupt in both `EXTI` and `NVIC`.

### 2. The ISR (Interrupt Service Routine)
```c
void EXTI4_IRQHandler(void) {
    // 1. Clear Pending Bit (Write 1 to clear)
    EXTI_PR |= (1 << 4);

    // 2. Toggle LED (XOR operation)
    GPIOA_ODR ^= (1 << 5);
}