/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Your Name
 * @brief          : Analog Dimmer (ADC to PWM Mapping in Bare-Metal C)
 * @description    : - TIM2 generates a 1kHz PWM signal on PA5 (LED).
 * - SysTick triggers an ADC conversion every 50ms.
 * - ADC1 reads analog voltage on PA0 (Potentiometer).
 * - ADC Interrupt maps the 12-bit ADC value (0-4095)
 * to the PWM duty cycle (0-1000) asynchronously.
 * - Main loop remains 100% idle (Event-Driven Architecture).
 ******************************************************************************
 */

#include <stdint.h>

/* ========================================================================== */
/* 1. REGISTER MEMORY MAPPING                                                 */
/* ========================================================================== */

// RCC Clocks
#define RCC_AHB1ENR     (*(volatile uint32_t *)0x40023830) // GPIO Clocks
#define RCC_APB1ENR     (*(volatile uint32_t *)0x40023840) // TIM2 Clock
#define RCC_APB2ENR     (*(volatile uint32_t *)0x40023844) // ADC1 Clock

// GPIOA
#define GPIOA_MODER     (*(volatile uint32_t *)0x40020000)
#define GPIOA_AFRL      (*(volatile uint32_t *)0x40020020)

// NVIC (Interrupt Controller)
#define NVIC_ISER0      (*(volatile uint32_t *)0xE000E100) // IRQ 0-31 (ADC is 18)

// SysTick
#define SYSTICK_CTRL    (*(volatile uint32_t *)0xE000E010)
#define SYSTICK_LOAD    (*(volatile uint32_t *)0xE000E014)

// TIM2 Registers (Base: 0x40000000)
#define TIM2_BASE       0x40000000
#define TIM2_CR1        (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM2_CCMR1      (*(volatile uint32_t *)(TIM2_BASE + 0x18))
#define TIM2_CCER       (*(volatile uint32_t *)(TIM2_BASE + 0x20))
#define TIM2_PSC        (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM2_ARR        (*(volatile uint32_t *)(TIM2_BASE + 0x2C))
#define TIM2_CCR1       (*(volatile uint32_t *)(TIM2_BASE + 0x34))

// ADC1 Registers (Base: 0x40012000)
#define ADC_BASE        0x40012000
#define ADC_SR          (*(volatile uint32_t *)(ADC_BASE + 0x00))
#define ADC_CR1         (*(volatile uint32_t *)(ADC_BASE + 0x04))
#define ADC_CR2         (*(volatile uint32_t *)(ADC_BASE + 0x08))
#define ADC_SQR3        (*(volatile uint32_t *)(ADC_BASE + 0x34))
#define ADC_DR          (*(volatile uint32_t *)(ADC_BASE + 0x4C))


/* ========================================================================== */
/* 2. GLOBAL VARIABLES                                                        */
/* ========================================================================== */

volatile uint32_t ms_counter = 0;   // Tracks milliseconds for SysTick
volatile uint32_t raw_adc_val = 0;  // Stores the 12-bit ADC reading


/* ========================================================================== */
/* 3. INTERRUPT SERVICE ROUTINES (ISR)                                        */
/* ========================================================================== */

/**
 * @brief SysTick Handler - Triggers every 1 ms
 */
void SysTick_Handler(void) {
    ms_counter++;

    // Every 50 ms, trigger a new ADC conversion
    if (ms_counter >= 50) {
        ms_counter = 0; // Reset counter

        // SWSTART (Bit 30): Start Conversion of regular channels
        ADC_CR2 |= (1 << 30);
    }
}

/**
 * @brief ADC Interrupt Handler - Triggers when conversion is complete
 */
void ADC_IRQHandler(void) {

    // Check if End Of Conversion (EOC) flag is set
    if (ADC_SR & (1 << 1)) {

        // 1. Read the raw analog value (0 to 4095). This automatically clears EOC flag!
        raw_adc_val = ADC_DR;

        // 2. Map the 12-bit ADC value to the PWM duty cycle range (0 to 1000)
        // Formula: PWM_Target = (ADC_Value * Max_PWM) / Max_ADC
        uint32_t target_pwm = (raw_adc_val * 1000) / 4095;

        // 3. Update the Hardware Timer register directly
        TIM2_CCR1 = target_pwm;
    }
}


/* ========================================================================== */
/* 4. MAIN PROGRAM                                                            */
/* ========================================================================== */

int main(void) {

    /* --- A. CLOCKS ENABLE --- */
    RCC_AHB1ENR |= (1 << 0); // Enable GPIOA
    RCC_APB1ENR |= (1 << 0); // Enable TIM2
    RCC_APB2ENR |= (1 << 8); // Enable ADC1

    /* --- B. GPIO CONFIGURATION --- */
    // Clear mode bits for PA0 and PA5
    GPIOA_MODER &= ~((3 << 0) | (3 << 10));

    // PA0: Set to Analog Mode ('11') for ADC input
    GPIOA_MODER |= (3 << 0);

    // PA5: Set to Alternate Function Mode ('10') for TIM2_CH1 (LED)
    GPIOA_MODER |= (2 << 10);
    // Connect PA5 to AF1 (TIM2)
    GPIOA_AFRL &= ~(0xF << 20);
    GPIOA_AFRL |=  (1 << 20);

    /* --- C. TIM2 (PWM) CONFIGURATION --- */
    // 1 MHz Timer Clock (1 tick = 1 us)
    TIM2_PSC = 16 - 1;
    // 1 kHz PWM Frequency (Period = 1000 us)
    TIM2_ARR = 1000 - 1;
    // PWM Mode 1 on Channel 1
    TIM2_CCMR1 &= ~(7 << 4);
    TIM2_CCMR1 |=  (6 << 4);
    // Enable Channel 1 Output
    TIM2_CCER |= (1 << 0);
    // Start with 0% Duty Cycle
    TIM2_CCR1 = 0;
    // Enable Timer
    TIM2_CR1 |= (1 << 0);

    /* --- D. ADC1 CONFIGURATION --- */
    // Select Channel 0 (PA0) as the first and only conversion in sequence
    ADC_SQR3 &= ~(0x1F << 0);

    // Enable End Of Conversion (EOC) Interrupt
    ADC_CR1 |= (1 << 5);

    // Power up the ADC
    ADC_CR2 |= (1 << 0);

    /* --- E. NVIC CONFIGURATION --- */
    // Enable ADC IRQ (IRQ 18) in NVIC
    NVIC_ISER0 |= (1 << 18);

    /* --- F. SYSTICK CONFIGURATION --- */
    SYSTICK_CTRL = 0;
    SYSTICK_LOAD = 16000 - 1; // 1 ms tick @ 16MHz
    SYSTICK_CTRL |= ((1 << 2) | (1 << 1) | (1 << 0));

    /* --- G. MAIN LOOP --- */
    while(1) {
        // The CPU is completely free!
        // All tasks (Timing, Measurement, Math, Output) are handled by Hardware & Interrupts.
    }
}
