#include <stdbool.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/structs/xip_ctrl.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/structs/iobank0.h"
#include "hardware/structs/sio.h"

#define dXIPdisabled    ( 1 )

#define dIRQSourceExclusive ( 1 )
#define dIRQSourceCallback  ( 0 )
#define dIRQSourceRaw       ( 0 )

#define dGPIO_IRQ   ( 0 )
#define dGPIO_OUT1  ( 2 )
#define dGPIO_OUT2  ( 3 )
#define dGPIO_PWM   ( 4 )

void GPIO_IRQHandlerFunc( uint gpio, uint32_t events );
void GPIO_ExclusiveIRQHandlerFunc();
void GPIO_Initialize();
void PWM_Initialize();

bool bSetting = false;

int main()
{
    #if dXIPdisabled
    /* Disable the XIP for no-XIP variants */
    xip_ctrl_hw->ctrl &= ~(1 << 0);
    #endif

    GPIO_Initialize();
    PWM_Initialize();

    while(1)
    {
    }

    return 0;
}

void GPIO_Initialize()
{
    gpio_init(dGPIO_OUT1);
    gpio_set_dir(dGPIO_OUT1, true);
    gpio_put(dGPIO_OUT1, true);
    gpio_set_pulls(dGPIO_OUT1, false, true);

    gpio_init(dGPIO_OUT2);
    gpio_set_dir(dGPIO_OUT2, true);
    gpio_put(dGPIO_OUT2, true);
    gpio_set_pulls(dGPIO_OUT2, false, true);

    gpio_init(dGPIO_IRQ);
    gpio_set_dir(dGPIO_IRQ, false);
    gpio_set_pulls(dGPIO_IRQ, true, false);

    gpio_init(dGPIO_PWM);
    gpio_set_dir(dGPIO_PWM, true);
    gpio_set_function(dGPIO_PWM, GPIO_FUNC_PWM);

    // gpio_set_dormant_irq_enabled

    /* 
     * Interrupt source setting 
     * General assmuption is to use falling slope as a IRQ trigger
     */ 
     
    #if dIRQSourceExclusive
    /* Option 1: exclusive handler*/
    irq_set_exclusive_handler(IO_IRQ_BANK0, GPIO_ExclusiveIRQHandlerFunc);
    gpio_set_irq_enabled(dGPIO_IRQ, 0x4, true);
    irq_set_enabled(IO_IRQ_BANK0, true);
    #endif

    #if dIRQSourceCallback
    /* Option 2: GPIO callback */
    gpio_set_irq_enabled_with_callback( dGPIO_IRQ, 0x04, true, &GPIO_IRQHandlerFunc);
    #endif

    #if dIRQSourceRaw
    /* Option 3: Raw handler from SDK 1.4.0*/
    gpio_add_raw_irq_handler();
    #endif
    }

void PWM_Initialize()
{
    /* 
     * Code on the courtesy of @WestfW: https://github.com/WestfW - brilliant idea to use PWM as
     * source of continous interrupts, thank you! :)
     * 
     * Kept 1 kHz output rate - convienient for scope observations.
     */
// Find out which PWM slice is connected to GPIO (it's slice 0)
  uint slice_num = pwm_gpio_to_slice_num(dGPIO_PWM);
  uint chan = pwm_gpio_to_channel(dGPIO_PWM);
    
// set clock divisor assuming 120MHz clock.  We want 500kHz
  pwm_set_clkdiv_int_frac(slice_num, 240, 0);  // 120MHz/240 = 500kHz.
// Set period of 500 cycles to get ~1kHz
  pwm_set_wrap(slice_num, 499);
// Set channel A output high for one cycle before dropping
  pwm_set_chan_level(slice_num, chan, 100);
// Set the PWM running
  pwm_set_enabled(slice_num, true);
}

// hal_gpio_irq_handler 

//void __not_in_flash_func()
// __attribute__((__section__(".scratch_x.GPIO_IRQHandlerFunc")))
void GPIO_IRQHandlerFunc( uint gpio, uint32_t events )
{
    padsbank0_hw->io[dGPIO_OUT1] ^= PADS_BANK0_GPIO0_OD_BITS;
    padsbank0_hw->io[dGPIO_OUT2] ^= PADS_BANK0_GPIO0_OD_BITS;
}

void GPIO_ExclusiveIRQHandlerFunc()
{
    padsbank0_hw->io[dGPIO_OUT1] ^= PADS_BANK0_GPIO0_OD_BITS;
    padsbank0_hw->io[dGPIO_OUT2] ^= PADS_BANK0_GPIO0_OD_BITS;
  /* Clear IRQ - if not this will be caled infinitely */
  iobank0_hw->intr[dGPIO_IRQ / 8] = 0xF << 4 * (dGPIO_IRQ % 8);
}