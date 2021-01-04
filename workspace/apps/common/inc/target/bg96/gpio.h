#ifndef __BG96_GPIO_H__
#define __BG96_GPIO_H__

#include <stdint.h>
#include <stdbool.h>
#include <qapi_tlmm.h>
#include <qapi_txm_base.h>

struct gpio_list_entry {
    const char *key;
    uint32_t value;
 };

typedef void (*gpio_interrupt_cb_t)(uint32_t arg);

int gpio_pin_on(uint32_t pin, uint32_t trigger, gpio_interrupt_cb_t interrupt_cb) ;
int gpio_pin_trigger(uint32_t pin);
int gpio_pin_disable_irq(uint32_t pin);
int gpio_pin_enable_irq(uint32_t pin);
int gpio_pin_deactivate(uint32_t pin);
int gpio_pin_config(uint32_t pin, uint32_t pull, uint32_t drive, uint32_t type);
int gpio_pin_output(uint32_t pin, uint32_t pull, uint32_t drive);
int gpio_pin_input(uint32_t pin, uint32_t pull, uint32_t drive);
int gpio_pin_release(uint32_t pin);
int gpio_pin_write(uint32_t pin, bool val);
int gpio_pin_read(uint32_t pin, bool val);

#endif