/*
*
* @author :  Nikolas Karakotas
* @date   :  4/1/2021
*
*/
#include "qapi_txm_base.h"
#include "qapi_gpioint.h"
#include "txm_module.h"
#include "bg96/gpio.h"

typedef struct{
    uint32_t pin_num;
    uint32_t gpio_id;
    uint32_t gpio_func;
    const char *int_name;
    gpio_interrupt_cb_t invoke_interrupt_cb;
}GPIO_MAP_TBL;


 const GPIO_MAP_TBL gpio_map_tbl[PIN_E_GPIO_MAX] = {
/* PIN NUM,      GPIO ID  GPIO FUNC         INTERRUPT NAME*/
    {  4,             23,      0,         "_int00000004",       NULL},
    {  5,             20,      0,         "_int00000005",       NULL},
    {  6,             21,      0,         "_int00000006",       NULL},
    {  7,             22,      0,         "_int00000007",       NULL},
    { 18,             11,      0,         "_int00000018",       NULL},
    { 19,             10,      0,         "_int00000019",       NULL},
    { 22,              9,      0,         "_int00000022",       NULL},
    { 23,              8,      0,         "_int00000023",       NULL},
    { 26,             15,      0,         "_int00000026",       NULL},
    { 27,             12,      0,         "_int00000027",       NULL},
    { 28,             13,      0,         "_int00000028",       NULL},
    { 40,             19,      0,         "_int00000040",       NULL},
    { 41,             18,      0,         "_int00000041",       NULL},
    { 64,             07,      0,         "_int00000064",       NULL}
};

/* gpio id table */
qapi_GPIO_ID_t gpio_id_tbl[PIN_E_GPIO_MAX];

/* gpio tlmm config table */
qapi_TLMM_Config_t tlmm_config[PIN_E_GPIO_MAX];


qapi_Instance_Handle_t gpio_interrupt_id_tbl[PIN_E_GPIO_MAX];


static const duk_number_list_entry gpio_module_consts[] = {
    { "PIN4",       PIN_E_GPIO_01 },
    { "PIN5",       PIN_E_GPIO_02 },
    { "PIN6",       PIN_E_GPIO_03 },
    { "PIN7",       PIN_E_GPIO_04 },
    { "PIN18",      PIN_E_GPIO_05 },
    { "PIN19",      PIN_E_GPIO_06 },
    { "PIN22",      PIN_E_GPIO_07 },
    { "PIN23",      PIN_E_GPIO_08 },
    { "PIN26",      PIN_E_GPIO_09 },
    { "PIN27",      PIN_E_GPIO_10 },
    { "PIN28",      PIN_E_GPIO_11 },
    { "PIN40",      PIN_E_GPIO_19 },
    { "PIN41",      PIN_E_GPIO_20 },
    { "PIN64",      PIN_E_GPIO_21 },
    { "PullUp",     QAPI_GPIO_PULL_UP_E },
    { "PullDown",   QAPI_GPIO_PULL_DOWN_E },
    { "PullNone",   QAPI_GPIO_NO_PULL_E },
    { "OpenDrain",  QAPI_GPIO_KEEPER_E },
    { NULL, 0 }
};


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

    qapi_Status_t status;

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){

            uint8_t index = pin; // pin is the index as we are using consts string names
            tlmm_config[index].pin = gpio_map_tbl[index].gpio_id; 
            tlmm_config[index].func = 0; 
            // pin mux value 1
            tlmm_config[index].dir = dir;
            tlmm_config[index].pull =  pull; 
            tlmm_config[index].drive = drive; // drive is for output pins, specify
            // the default here
            status = qapi_TLMM_Get_Gpio_ID( &tlmm_config[index], &gpio_id_tbl[index]);
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
int gpio_on(uint32_t pin, qapi_GPIOINT_Trigger_e trigger, gpio_interrupt_cb_t interrupt_cb) {


    if(pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){

        if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
            qapi_Status_t status = qapi_GPIOINT_Register_Interrupt(&gpio_interrupt_id_tbl[pin],
                                                     gpio_map_tbl[pin].gpio_id,
                                                     interrupt_cb,
                                                     pin,
                                                     trigger,
                                                     QAPI_GPIOINT_PRIO_LOWEST_E,
                                                     false);
            gpio_map_tbl[pin].invoke_interrupt_cb = interrupt_cb;
            if(status == QAPI_OK){
                return 0;
            }

        }

    }

    return 1; 
}
  
int gpio_trigger(uint32_t pin) {
   
    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        qapi_GPIOINT_Trigger_Interrupt(&gpio_interrupt_id_tbl[pin], gpio_map_tbl[pin].gpio_id );
    }

    return 0; /* no return value (= undefined) */
}


int gpio_disable_irq(uint32_t pin) {
    
    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        qapi_GPIOINT_Deregister_Interrupt(&gpio_interrupt_id_tbl[pin], gpio_map_tbl[pin].gpio_id);
    }

    return 0; /* no return value (= undefined) */
}

int gpio_enable_irq(uint32_t pin) {

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        qapi_GPIOINT_Enable_Interrupt (&gpio_interrupt_id_tbl[pin], gpio_map_tbl[pin].gpio_id);
    }

    return 0; /* no return value (= undefined) */
}

int gpio_deactivate(uint32_t pin) {

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        qapi_GPIOINT_Deregister_Interrupt (&gpio_interrupt_id_tbl[pin], gpio_map_tbl[pin].gpio_id);
    }

    return 0; /* no return value (= undefined) */

}

int gpio_output(uint32_t pin, uint32_t pull, uint32_t drive) {

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        if(qapi_TLMM_Release_Gpio_ID(&tlmm_config[pin], gpio_id_tbl[pin]) == QAPI_OK){
            return gpio_config(pin, pull, QAPI_GPIO_OUTPUT_E, drive)
        }
    }

    return 1;
}

int gpio_input(uint32_t pin, uint32_t pull) {

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        if(qapi_TLMM_Release_Gpio_ID(&tlmm_config[pin], gpio_id_tbl[pin]) == QAPI_OK){
           return gpio_config(pin, pull, QAPI_GPIO_INPUT_E, QAPI_GPIO_KEEPER_E);
        }
    }

    return 1;
}

int gpio_release(uint32_t pin) {

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        if(qapi_TLMM_Release_Gpio_ID(&tlmm_config[pin], gpio_id_tbl[pin]) == QAPI_OK){
            return 0;
        }
    }

    return 1; 
}
