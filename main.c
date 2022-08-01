#include <stdbool.h>
#include <stdint.h>
#include "pico/stdlib.h"

#define dGPIO_IRQ ( 0 )
#define dGPIO_OUT1 ( 2 )
#define dGPIO_OUT2 ( 3 )

void GPIO_IRQHandlerFunc( uint gpio, uint32_t events );
bool bSetting = false;

int main()
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
    /* Use falling slope as trigger source */
    gpio_set_irq_enabled_with_callback( dGPIO_IRQ, 0x04, true, &GPIO_IRQHandlerFunc);

    while(1)
    {
    }

    return 0;
}

// hal_gpio_irq_handler 

//void __not_in_flash_func()
// __attribute__((__section__(".scratch_x.GPIO_IRQHandlerFunc")))
void GPIO_IRQHandlerFunc( uint gpio, uint32_t events )
{
    padsbank0_hw->io[dGPIO_OUT1] |= PADS_BANK0_GPIO0_OD_BITS;
    padsbank0_hw->io[dGPIO_OUT2] |= PADS_BANK0_GPIO0_OD_BITS;
}