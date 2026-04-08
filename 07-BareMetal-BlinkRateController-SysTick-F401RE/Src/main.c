/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Your Name
 * @brief          : SysTick Blinker & EXTI Debouncer (Bare-Metal)
 * @description    : - SysTick automatically toggles the LED (PA5).
 * - PA10 (EXTI10) decreases the blink delay (faster).
 * - PB12 (EXTI12) increases the blink delay (slower).
 * - SysTick is also used to provide software debouncing
 * for the mechanical buttons.
 ******************************************************************************
 */

#include <stdint.h>

/* ========================================================================== */
/* 1. REGISTER MEMORY MAPPING                                                 */
/* ========================================================================== */

// Clock Control Registers
#define RCC_AHB1ENR     (*(volatile uint32_t *)0x40023830) // GPIO Clocks
#define RCC_APB2ENR     (*(volatile uint32_t *)0x40023844) // SYSCFG Clock

// GPIO Registers
#define GPIOA_MODER     (*(volatile uint32_t *)0x40020000)
#define GPIOA_ODR       (*(volatile uint32_t *)0x40020014)
#define GPIOB_MODER     (*(volatile uint32_t *)0x40020400)
#define GPIOA_PUPDR     (*(volatile uint32_t *)0x4002000C)
#define GPIOB_PUPDR     (*(volatile uint32_t *)0x4002040C)

// System Configuration Registers (EXTI Routing)
#define SYSCFG_EXTICR3  (*(volatile uint32_t *)0x40013810) // EXTI 8 to 11
#define SYSCFG_EXTICR4  (*(volatile uint32_t *)0x40013814) // EXTI 12 to 15

// EXTI Registers
#define EXTI_IMR        (*(volatile uint32_t *)0x40013C00) // Interrupt Mask
#define EXTI_FTSR       (*(volatile uint32_t *)0x40013C0C) // Falling Trigger
#define EXTI_PR         (*(volatile uint32_t *)0x40013C14) // Pending Register

// NVIC Register
#define NVIC_ISER1      (*(volatile uint32_t *)0xE000E104) // IRQs 32-63

// SysTick Registers (Core M4)
#define SYSTICK_CTRL    (*(volatile uint32_t *)0xE000E010) // Control and Status
#define SYSTICK_LOAD    (*(volatile uint32_t *)0xE000E014) // Reload Value
#define SYSTICK_VAL     (*(volatile uint32_t *)0xE000E018) // Current Value


/* ========================================================================== */
/* 2. GLOBAL VARIABLES                                                        */
/* ========================================================================== */

#define DEBOUNCE_TIME 50 // Debounce window in milliseconds

// Volatile keyword is MANDATORY for variables modified inside interrupts
volatile uint32_t blink_delay = 500;    // LED toggle interval in ms
volatile uint32_t ms_counter = 0;       // Increments every 1 ms by SysTick

volatile uint32_t last_pa10_press = 0;  // Timestamp of the last PA10 trigger
volatile uint32_t last_pb12_press = 0;  // Timestamp of the last PB12 trigger


/* ========================================================================== */
/* 3. INTERRUPT SERVICE ROUTINES (ISR)                                        */
/* ========================================================================== */

/**
 * @brief System Tick Timer Handler
 * Triggered automatically every 1 ms.
 */
void SysTick_Handler(void) {
    ms_counter++; // Keep track of system time (1 tick = 1 ms)

    // Toggle LED when the counter reaches the desired delay
    if (ms_counter >= blink_delay) {
        ms_counter = 0; // Reset counter

        // XOR bitwise operator (^=) toggles the bit state (1->0, 0->1)
        GPIOA_ODR ^= (1 << 5);
    }
}

/**
 * @brief EXTI Lines 10 to 15 Handler
 * Triggered by physical buttons connected to PA10 and PB12.
 */
void EXTI15_10_IRQHandler(void) {

    // Snapshot of the current time for debouncing logic
    uint32_t current_time = ms_counter;

    /* --- CHECK PA10 (FASTER BLINK) --- */
    if (EXTI_PR & (1 << 10)) {
        // 1. Clear the pending flag immediately (Write 1 to clear)
        EXTI_PR |= (1 << 10);

        // 2. Debouncing check: Ensure enough time has passed
        if ((current_time - last_pa10_press) > DEBOUNCE_TIME) {

            // 3. Execute action: Decrease delay (min 100ms)
            if (blink_delay > 100) {
                blink_delay -= 100;
            }

            // 4. Update the timestamp of the last valid press
            last_pa10_press = current_time;
        }
    }

    /* --- CHECK PB12 (SLOWER BLINK) --- */
    if (EXTI_PR & (1 << 12)) {
        // 1. Clear the pending flag immediately
        EXTI_PR |= (1 << 12);

        // 2. Debouncing check
        if ((current_time - last_pb12_press) > DEBOUNCE_TIME) {

            // 3. Execute action: Increase delay (max 2000ms)
            if (blink_delay < 2000) {
                blink_delay += 100;
            }

            // 4. Update the timestamp
            last_pb12_press = current_time;
        }
    }
}


/* ========================================================================== */
/* 4. MAIN PROGRAM                                                            */
/* ========================================================================== */

int main(void) {
    /* --- A. CLOCKS --- */
    RCC_AHB1ENR |= (3 << 0);  // Enable GPIOA (Bit 0) and GPIOB (Bit 1)
    RCC_APB2ENR |= (1 << 14); // Enable SYSCFG (Bit 14)

    /* --- B. GPIO CONFIGURATION --- */
    // Clear mode bits for PA5, PA10, and PB12
    GPIOA_MODER &= ~((3 << 20) | (3 << 10));
    GPIOB_MODER &= ~(3 << 24);

    // Set PA5 as Output (01)
    GPIOA_MODER |= (1 << 10);
    // PA10 and PB12 remain as Input (00)

    // Enable Pull-Up resistors for PA10 and PB12
    GPIOA_PUPDR &= ~(3 << 20);
    GPIOB_PUPDR &= ~(3 << 24);
    GPIOA_PUPDR |= (1 << 20); // Write '01' to PA10
    GPIOB_PUPDR |= (1 << 24); // Write '01' to PB12

    /* --- C. SYSCFG ROUTING --- */
    // Route EXTI10 to Port A (0000 at bits 8-11)
    SYSCFG_EXTICR3 &= ~(0xF << 8);

    // Route EXTI12 to Port B (0001 at bits 0-3)
    SYSCFG_EXTICR4 &= ~(0xF << 0);
    SYSCFG_EXTICR4 |= (1 << 0);

    /* --- D. EXTI CONFIGURATION --- */
    // Unmask EXTI lines 10 and 12
    EXTI_IMR |= ((1 << 10) | (1 << 12));
    // Set trigger to Falling Edge for both lines
    EXTI_FTSR |= ((1 << 10) | (1 << 12));

    /* --- E. NVIC CONFIGURATION --- */
    // Enable IRQ 40 (EXTI15_10) in ISER1 at bit 8
    NVIC_ISER1 |= (1 << 8);

    /* --- F. SYSTICK CONFIGURATION --- */
    // Disable SysTick before configuration
    SYSTICK_CTRL = 0;

    // Set Reload Value for 1 ms tick (assuming 16 MHz HSI clock)
    // Formula: (Clock_Frequency / 1000) - 1
    SYSTICK_LOAD = 16000 - 1;

    // Clear the current value register
    SYSTICK_VAL = 0;

    // Enable Timer (Bit 0), Interrupt (Bit 1), and use Processor Clock (Bit 2)
    SYSTICK_CTRL |= ((1 << 2) | (1 << 1) | (1 << 0));

    /* --- G. INFINITE LOOP --- */
    while(1) {
        // The main loop is completely empty!
        // The entire application runs asynchronously via Interrupts.
    }
}
