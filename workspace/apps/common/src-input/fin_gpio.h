#if !defined(FIN_API_GPIO_H_INCLUDED)
#define FIN_API_GPIO_H_INCLUDED

struct gpio_list_entry {
    const char *key;
    uint32_t value;
 };

typedef void (*fin_gpio_interrupt_cb_t)(uint32_t arg);

int fin_gpio_pin_on(uint32_t pin, uint32_t trigger, fin_gpio_interrupt_cb_t interrupt_cb) ;
int fin_gpio_pin_trigger(uint32_t pin);
int fin_gpio_pin_disable_irq(uint32_t pin);
int fin_gpio_pin_enable_irq(uint32_t pin);
int fin_gpio_pin_config(uint32_t pin, uint32_t pull, uint32_t drive, uint32_t type);
int fin_gpio_pin_output(uint32_t pin, uint32_t pull, uint32_t drive);
int fin_gpio_pin_input(uint32_t pin, uint32_t pull, uint32_t drive);
int fin_gpio_pin_release(uint32_t pin);
int fin_gpio_pin_write(uint32_t pin, bool val);
int fin_gpio_pin_read(uint32_t pin);
void fin_gpio_config_dump(uint32_t pin);


#endif  /* FIN_API_GPIO_H_INCLUDED */