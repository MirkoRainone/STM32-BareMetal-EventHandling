/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Your Name
 * @brief          : Introduction to Hardware Timers (TIMx) - Bare-Metal
 * @description    : Blinks the on-board LED (PA5) every 500ms using the
 * General Purpose Timer 2 (TIM2) interrupt.
 * The CPU is completely completely idle in the main loop.
 ******************************************************************************
 */

#include <stdint.h>

/* ========================================================================== */
/* 1. REGISTER MEMORY MAPPING                                                 */
/* ========================================================================== */

// Clock Control Registers
#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830) // AHB1 Bus (GPIO)
#define RCC_APB1ENR (*(volatile uint32_t *)0x40023840) // APB1 Bus (TIM2, TIM3...)

// GPIOA Registers
#define GPIOA_MODER (*(volatile uint32_t *)0x40020000) // Mode Register
#define GPIOA_ODR   (*(volatile uint32_t *)0x40020014) // Output Data Register

// TIM2 Registers (Base Address: 0x40000000)
#define TIM2_BASE   0x40000000
#define TIM2_CR1    (*(volatile uint32_t *)(TIM2_BASE + 0x00)) // Control Register 1
#define TIM2_DIER   (*(volatile uint32_t *)(TIM2_BASE + 0x0C)) // DMA/Interrupt Enable
#define TIM2_SR     (*(volatile uint32_t *)(TIM2_BASE + 0x10)) // Status Register
#define TIM2_PSC    (*(volatile uint32_t *)(TIM2_BASE + 0x28)) // Prescaler
#define TIM2_ARR    (*(volatile uint32_t *)(TIM2_BASE + 0x2C)) // Auto-Reload

// NVIC (Nested Vectored Interrupt Controller)
#define NVIC_ISER0  (*(volatile uint32_t *)0xE000E100) // IRQ 0 to 31


/* ========================================================================== */
/* 2. INTERRUPT SERVICE ROUTINE (ISR)                                         */
/* ========================================================================== */

/**
 * @brief TIM2 Global Interrupt Handler
 * Triggered automatically when TIM2 counter reaches the ARR value.
 */
void TIM2_IRQHandler(void) {

    // Check if the Update Interrupt Flag (UIF) is set (Bit 0)
    if (TIM2_SR & (1 << 0)) {

        // CRITICAL: Clear the Update Interrupt Flag!
        // If not cleared, the CPU will be stuck in this interrupt forever.
        TIM2_SR &= ~(1 << 0);

        // Toggle the LED state (PA5) using bitwise XOR
        GPIOA_ODR ^= (1 << 5);
    }
}


/* ========================================================================== */
/* 3. MAIN PROGRAM                                                            */
/* ========================================================================== */

int main(void) {

    /* --- A. ENABLE PERIPHERAL CLOCKS --- */
    // Enable clock for GPIOA (Bit 0)
    RCC_AHB1ENR |= (1 << 0);
    // Enable clock for TIM2 on the APB1 Bus (Bit 0)
    RCC_APB1ENR |= (1 << 0);

    /* --- B. CONFIGURE GPIO (LED) --- */
    // Set PA5 as General Purpose Output (01)
    GPIOA_MODER &= ~(3 << 10); // Clear bits 10 and 11
    GPIOA_MODER |=  (1 << 10); // Set bit 10 to 1

    /* --- C. CONFIGURE TIM2 --- */
    // Assuming 16 MHz HSI System Clock.
    // 1. Set Prescaler (PSC) to slow down the clock.
    // 16.000.000 Hz / 16.000 = 1.000 Hz (1 Tick = 1 ms)
    TIM2_PSC = 16000 - 1;

    // 2. Set Auto-Reload Register (ARR) for the desired period.
    // 500 ticks = 500 ms (Half a second)
    TIM2_ARR = 500 - 1;

    // 3. Enable the Update Interrupt Enable (UIE) bit
    TIM2_DIER |= (1 << 0);

    /* --- D. CONFIGURE NVIC --- */
    // TIM2 is mapped to IRQ 28 in the STM32F4 architecture.
    // Enable IRQ 28 in ISER0 (Interrupt Set-Enable Register 0)
    NVIC_ISER0 |= (1 << 28);

    /* --- E. START THE TIMER --- */
    // Enable the Counter (CEN bit) in Control Register 1
    TIM2_CR1 |= (1 << 0);

    /* --- F. INFINITE LOOP --- */
    while(1) {
        // The CPU is completely free to perform other tasks or enter sleep mode.
        // LED toggling is handled entirely by hardware and interrupts.
    }
}
