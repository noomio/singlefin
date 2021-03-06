/*
*
* @author :  Nikolas Karakotas
* @date   :  4/1/2021
*
*/
#include "fin_internal.h"

typedef struct{
    uint32_t pin_num;   // module pin
    uint32_t pin_soc;   // internal soc pin number
    uint32_t gpio_func;
    const char *int_name;
}GPIO_MAP_TBL;


#define PIN_E_GPIO_MAX 14
const GPIO_MAP_TBL gpio_map_tbl[PIN_E_GPIO_MAX] = {
    /* PIN NUM,       PIN_SOC  GPIO FUNC  INTERRUPT NAME     */
    {  4,             23,      0,         "_int00000004"},
    {  5,             20,      0,         "_int00000005"},
    {  6,             21,      0,         "_int00000006"},
    {  7,             22,      0,         "_int00000007"},
    { 18,             11,      0,         "_int00000018"},  // IO1
    { 19,             10,      0,         "_int00000019"},  // IO2 -> ap ready
    { 22,              9,      0,         "_int00000022"},
    { 23,              8,      0,         "_int00000023"},
    { 26,             15,      0,         "_int00000026"},  // IO3
    { 27,             12,      0,         "_int00000027"},
    { 28,             13,      0,         "_int00000028"},
    { 40,             19,      0,         "_int00000040"},  
    { 41,             18,      0,         "_int00000041"},
    { 64,             07,      0,         "_int00000064"}
};

/* gpio id table - stores */
qapi_GPIO_ID_t gpio_id_tbl[PIN_E_GPIO_MAX];

/* gpio tlmm config table - stores */
qapi_TLMM_Config_t tlmm_config[PIN_E_GPIO_MAX];


qapi_Instance_Handle_t gpio_interrupt_id_tbl[PIN_E_GPIO_MAX];


bool gpio_is_interrupt[PIN_E_GPIO_MAX];


static const struct gpio_list_entry gpio_module_consts[] = {
    { "PIN4",       4 },
    { "PIN5",       5 },
    { "PIN6",       6 },
    { "PIN7",       7 },
    { "PIN18",      18 },
    { "PIN19",      19 },
    { "PIN22",      22 },
    { "PIN23",      23 },
    { "PIN26",      26 },
    { "PIN27",      27 },
    { "PIN28",      28 },
    { "PIN40",      40 },
    { "PIN41",      41 },
    { "PIN64",      64 }
};

static const struct gpio_list_entry gpio_pull_consts[] = {
    { "PullUp",     QAPI_GPIO_PULL_UP_E },
    { "PullDown",   QAPI_GPIO_PULL_DOWN_E },
    { "PullNone",   QAPI_GPIO_NO_PULL_E },
    { "Keeper",  QAPI_GPIO_KEEPER_E }
};

static const struct gpio_list_entry gpio_drive_consts[] = {
    { "Drive2mA",  QAPI_GPIO_2MA_E },
    { "Drive4mA",  QAPI_GPIO_4MA_E },
    { "Drive6mA",  QAPI_GPIO_6MA_E },
    { "Drive8mA",  QAPI_GPIO_8MA_E },
    { "Drive10mA", QAPI_GPIO_10MA_E },
    { "Drive12mA", QAPI_GPIO_12MA_E },
    { "Drive14mA", QAPI_GPIO_14MA_E },
    { "Drive16mA", QAPI_GPIO_16MA_E }
};

static uint32_t get_soc_pin(uint32_t pin){
    uint32_t pin_soc = (uint32_t)-1;

    for(int i=0; i < PIN_E_GPIO_MAX; i++){
        if(gpio_map_tbl[i].pin_num == pin){
            pin_soc = gpio_map_tbl[i].pin_soc;
            break;
        }
    }

   return pin_soc;
}

static int get_soc_pin_index(uint32_t pin){

    for(int i=0; i < PIN_E_GPIO_MAX; i++){
        if(gpio_map_tbl[i].pin_num == pin)
            return i;
    }

   return -1;
}

static qapi_GPIO_ID_t get_gpio_id(uint32_t pin){

    for(uint8_t i=0; i < PIN_E_GPIO_MAX; i++){
        if(gpio_map_tbl[i].pin_num == pin){
            return gpio_id_tbl[i];
        }
    }

    return (uint32_t)-1;
}


static const char *get_pin_name(uint32_t pin){

    for(int i=0; i < sizeof(gpio_module_consts)/sizeof(struct gpio_list_entry); i++){
        if(gpio_module_consts[i].value == pin)
            return gpio_module_consts[i].key;
    }

   return "NULL";
}

static const char *get_pin_drive(uint32_t drive){

    for(int i=0; i < sizeof(gpio_drive_consts)/sizeof(struct gpio_list_entry); i++){
        if(gpio_drive_consts[i].value == drive)
            return gpio_drive_consts[i].key;
    }

   return "NULL";
}

static const char *get_pin_pull(uint32_t pull){

    for(int i=0; i < sizeof(gpio_pull_consts)/sizeof(struct gpio_list_entry); i++){
        if(gpio_pull_consts[i].value == pull)
            return gpio_pull_consts[i].key;
    }

   return "NULL";
}


static qapi_TLMM_Config_t* get_tlmm_config(uint32_t pin){

    for(uint8_t i=0; i < PIN_E_GPIO_MAX; i++){
        if(gpio_map_tbl[i].pin_num == pin){
            return &tlmm_config[i];
        }
    }

    return NULL;
}

static qapi_Instance_Handle_t* get_instance_handle(uint32_t pin){
    for(uint8_t i=0; i < PIN_E_GPIO_MAX; i++){
        if(gpio_map_tbl[i].pin_num == pin){
            return &gpio_interrupt_id_tbl[i];
        }
    }

    return NULL;
}

static qapi_GPIO_Value_t gpio_read( uint32_t pin_soc, qapi_GPIO_ID_t gpio_id) {

    qapi_GPIO_Value_t val;

    qapi_TLMM_Read_Gpio(gpio_id,pin_soc, &val);

    return val;
}

static int gpio_write(uint32_t pin_soc, qapi_GPIO_ID_t gpio_id, bool val) {

    if(val)
        qapi_TLMM_Drive_Gpio(gpio_id, pin_soc, QAPI_GPIO_HIGH_VALUE_E);
    else
        qapi_TLMM_Drive_Gpio(gpio_id, pin_soc, QAPI_GPIO_LOW_VALUE_E);

    return 0;
}

static int gpio_config(uint32_t pin, uint32_t pull, uint32_t dir, qapi_GPIO_Drive_t drive) {

    int index = get_soc_pin_index(pin);
    if(index != -1){

            tlmm_config[index].pin = gpio_map_tbl[index].pin_soc; 
            tlmm_config[index].func = 0; 
            // pin mux value 1
            tlmm_config[index].dir = dir;
            tlmm_config[index].pull =  pull; 
            tlmm_config[index].drive = drive; // drive is for output pins, specify
            gpio_is_interrupt[index] = false;
            // the default here
            qapi_Status_t status = qapi_TLMM_Get_Gpio_ID( &tlmm_config[index], &gpio_id_tbl[index]);
            if (status == QAPI_OK){
                status = qapi_TLMM_Config_Gpio(gpio_id_tbl[index], &tlmm_config[index]);
            }

            if(status != QAPI_OK){
                return 1;
            }else
                return 0;

    }
   
    return 2;  

}


/*      
    QAPI_GPIOINT_TRIGGER_EDGE_FALLING_E;
    QAPI_GPIOINT_TRIGGER_EDGE_RISING_E;
    QAPI_GPIOINT_TRIGGER_LEVEL_HIGH_E;
    QAPI_GPIOINT_TRIGGER_LEVEL_LOW_E;
    QAPI_GPIOINT_TRIGGER_EDGE_DUAL_E;
*/
int fin_gpio_pin_on(uint32_t pin, uint32_t trigger, fin_gpio_interrupt_cb_t interrupt_cb) {

    uint32_t pin_soc = get_soc_pin(pin);

    if(pin_soc != (uint32_t)-1UL){

        qapi_Status_t status = qapi_GPIOINT_Register_Interrupt(
            get_instance_handle(pin),
            get_gpio_id(pin),
            interrupt_cb,
            pin_soc,
            trigger,
            QAPI_GPIOINT_PRIO_LOWEST_E,
            false
            );

        if(status == QAPI_OK){
            return 0;
        }


    }

    return 1; 
}
  
int fin_gpio_pin_trigger(uint32_t pin) {
   
    uint32_t pin_soc = get_soc_pin(pin);

    if(pin_soc != (uint32_t)-1UL){
        return qapi_GPIOINT_Trigger_Interrupt(get_instance_handle(pin), get_gpio_id(pin) );
    }

    return 1;
}


int fin_gpio_pin_disable_irq(uint32_t pin) {
    
    uint32_t pin_soc = get_soc_pin(pin);
    int index = get_soc_pin_index(pin);

    if(pin_soc != (uint32_t)-1UL){
        if(qapi_GPIOINT_Deregister_Interrupt(get_instance_handle(pin), get_gpio_id(pin)) == QAPI_OK){
            gpio_is_interrupt[index] = false;
            return 0;
        }
    }

    return 1;
}

int fin_gpio_pin_enable_irq(uint32_t pin) {

    uint32_t pin_soc = get_soc_pin(pin);
    int index = get_soc_pin_index(pin);

    if(pin_soc != (uint32_t)-1UL){
        if(qapi_GPIOINT_Enable_Interrupt (get_instance_handle(pin), get_gpio_id(pin)) == QAPI_OK){
            gpio_is_interrupt[index] = true;
            return 0;
        }
    }

    return 1;
}


int fin_gpio_pin_config(uint32_t pin, uint32_t pull, uint32_t drive, uint32_t type) {
    uint32_t pin_soc = get_soc_pin(pin);

    if(pin_soc != (uint32_t)-1UL){
        return gpio_config(pin, pull, type, drive);
    }

    return 1;
}

int fin_gpio_pin_output(uint32_t pin, uint32_t pull, uint32_t drive) {

    uint32_t pin_soc = get_soc_pin(pin);

    if(pin_soc != (uint32_t)-1UL){
        if(qapi_TLMM_Release_Gpio_ID(get_tlmm_config(pin), get_gpio_id(pin)) == QAPI_OK){
            return gpio_config(pin_soc, pull, QAPI_GPIO_OUTPUT_E, drive);
        }
    }

    return 1;
}

int fin_gpio_pin_input(uint32_t pin, uint32_t pull, uint32_t drive) {

    uint32_t pin_soc = get_soc_pin(pin);

    if(pin_soc != (uint32_t)-1UL){
        if(qapi_TLMM_Release_Gpio_ID(get_tlmm_config(pin), get_gpio_id(pin)) == QAPI_OK){
           return gpio_config(pin, pull, QAPI_GPIO_INPUT_E, drive);
        }
    }

    return 1;
}

int fin_gpio_pin_release(uint32_t pin) {

    uint32_t pin_soc = get_soc_pin(pin);

    if(pin_soc != (uint32_t)-1UL){
        return qapi_TLMM_Release_Gpio_ID(get_tlmm_config(pin), get_gpio_id(pin));
    }

    return 1; 
}

int fin_gpio_pin_write(uint32_t pin, bool val) {

    uint32_t pin_soc = get_soc_pin(pin);

    if( pin_soc != (uint32_t)-1UL){
        return gpio_write(pin_soc,get_gpio_id(pin), val);
    }

    return 1;
}

int fin_gpio_pin_read(uint32_t pin) {
    
    uint32_t pin_soc = get_soc_pin(pin);

    if( pin_soc != (uint32_t)-1UL){
        return gpio_read(pin_soc,get_gpio_id(pin));
    }

    return 1;
}

void fin_gpio_config_dump(uint32_t pin){
    uint32_t pin_soc = get_soc_pin(pin);
    int index = get_soc_pin_index(pin);

    if( pin_soc != (uint32_t)-1UL && index != -1){

        printf("Pin: %u\r\n"
            "Pin Soc: %u\r\n"
            "Pin Name: %s\r\n"
            "Direction: %s\r\n"
            "Function: %u\r\n"
            "Pull: %s\r\n"
            "Drive: %s\r\n"
            "Irq Enabled: %s\r\n",
            pin,
            pin_soc,
            get_pin_name(pin),
            (tlmm_config[index].dir == QAPI_GPIO_INPUT_E ? "input" : "output"),
            tlmm_config[index].func, 
            get_pin_pull(tlmm_config[index].pull),
            get_pin_drive(tlmm_config[index].drive),
            gpio_is_interrupt[index] == 0 ? "no" : "yes"
            );
    }else{
        puts("NULL\r\n");
    }
}

