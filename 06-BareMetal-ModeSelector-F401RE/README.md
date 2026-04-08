# STM32 Level 2: Shared Interrupts & Internal Pull-Ups

This project demonstrates how to handle multiple External Interrupts (EXTI) that share the same hardware Interrupt Request (IRQ) line on the STM32F401RE. It also highlights a critical hardware concept: fixing "floating" inputs using internal Pull-Up resistors.

## 📌 Overview
The system uses two external push-buttons to control a single LED:
- **Button 1 (`PA10`)**: Turns the LED **ON**.
- **Button 2 (`PB12`)**: Turns the LED **OFF**.

Both buttons trigger an interrupt on the **Falling Edge** (when pressed). Because lines 10 and 12 are grouped together by the ARM Cortex-M4 architecture, they both trigger the exact same Interrupt Service Routine (`EXTI15_10_IRQHandler`). The software must determine which button was pressed by checking the EXTI Pending Register (`PR`).

## ⚠️ The Hardware Problem: Floating Pins
Initially, configuring the pins as plain `INPUT` caused erratic LED behavior. An unconnected input pin acts as an antenna, picking up electromagnetic noise and rapidly fluctuating between 0V and 3.3V (Floating State). This triggered thousands of false interrupts per second.

**The Solution:**
I configured the **`GPIOx_PUPDR` (Pull-Up/Pull-Down Register)** to enable the internal Pull-Up resistors for `PA10` and `PB12`. 
- **Idle State:** The pin is held firmly at 3.3V (Logic 1).
- **Pressed State:** The button connects the pin to GND, pulling it to 0V (Logic 0).
- This creates a clean, predictable Falling Edge for the EXTI controller.

## 📂 Register Mapping

| Component | Register | Purpose |
| :--- | :--- | :--- |
| **GPIO** | `MODER` | Set PA5 to Output, PA10/PB12 to Input. |
| **GPIO** | `PUPDR` | Enable internal Pull-Up resistors for PA10/PB12. |
| **SYSCFG** | `EXTICR3` / `EXTICR4` | Route Port A to Line 10 and Port B to Line 12. |
| **EXTI** | `IMR` | Unmask (Enable) EXTI lines 10 and 12. |
| **EXTI** | `FTSR` | Set trigger to Falling Edge for both lines. |
| **NVIC** | `ISER1` | Enable IRQ 40 (EXTI15_10) in the ARM Core. |

## 💻 Code Highlight: The Shared ISR
Because lines 10 through 15 share the same entry point in the Vector Table, the software must poll the `EXTI_PR` register to identify the interrupt source before taking action.

```c
void EXTI15_10_IRQHandler(void){
    
    // Check if EXTI Line 10 (PA10) triggered the interrupt
    if(EXTI_PR & (1 << 10)){
        EXTI_PR |= (1 << 10);   // Clear pending flag (rc_w1)
        GPIOA_ODR |= (1 << 5);  // Turn LED ON
    }
    
    // Check if EXTI Line 12 (PB12) triggered the interrupt
    if(EXTI_PR & (1 << 12)){
        EXTI_PR |= (1 << 12);   // Clear pending flag (rc_w1)
        GPIOA_ODR &= ~(1 << 5); // Turn LED OFF
    }
}
