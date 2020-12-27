/*
 *  Identify module
 */

/* Include duktape.h and whatever platform headers are needed. */
#include "duktape.h"
#include "qapi_txm_base.h"
#include "qapi_gpioint.h"
#include "txm_module.h"
#include "bg96_gpio.h"
#include "event_mngr.h"

typedef struct{
    uint32_t pin_num;
    uint32_t gpio_id;
    uint32_t gpio_func;
    const char *int_name;
}GPIO_MAP_TBL;


 const GPIO_MAP_TBL gpio_map_tbl[PIN_E_GPIO_MAX] = {
/* PIN NUM,      GPIO ID  GPIO FUNC         INTERRUPT NAME*/
    {  4,             23,      0,         "_int00000004"},
    {  5,             20,      0,         "_int00000005"},
    {  6,             21,      0,         "_int00000006"},
    {  7,             22,      0,         "_int00000007"},
    { 18,             11,      0,         "_int00000018"},
    { 19,             10,      0,         "_int00000019"},
    { 22,              9,      0,         "_int00000022"},
    { 23,              8,      0,         "_int00000023"},
    { 26,             15,      0,         "_int00000026"},
    { 27,             12,      0,         "_int00000027"},
    { 28,             13,      0,         "_int00000028"},
    { 40,             19,      0,         "_int00000040"},
    { 41,             18,      0,         "_int00000041"},
    { 64,             07,      0,         "_int00000064"}
};

/* gpio id table */
qapi_GPIO_ID_t gpio_id_tbl[PIN_E_GPIO_MAX];

/* gpio tlmm config table */
qapi_TLMM_Config_t tlmm_config[PIN_E_GPIO_MAX];


qapi_Instance_Handle_t gpio_interrupt_id_tbl[PIN_E_GPIO_MAX];

/*
 *  Duktape/C functions providing module functionality.
 */

static duk_ret_t read(duk_context *ctx) {
    /* ... */
    duk_push_this(ctx);

    qapi_GPIO_Value_t val;

    (void) duk_get_prop_string(ctx,0, "pin_soc");
    uint32_t pin_soc = duk_to_uint32(ctx, -1);
    //printf("obj.propertyVal= %u\r\n", pin_soc);

    (void) duk_get_prop_string(ctx,0, "gpio_id");
    qapi_GPIO_ID_t gpio_id = duk_to_uint32(ctx, -1);
    //printf("obj.propertyVal= %u\r\n", gpio_id);


    qapi_TLMM_Read_Gpio(gpio_id,pin_soc, &val);

    val == 0 ?  duk_push_false(ctx) : duk_push_true(ctx);


    return 1;
}

static duk_ret_t write(duk_context *ctx) {
    /* ... */
    duk_push_this(ctx);

    bool val = duk_require_boolean(ctx, 0);

    (void) duk_get_prop_string(ctx,1, "pin_soc");
    uint32_t pin_soc = duk_to_uint32(ctx, -1);
    //printf("obj.propertyVal= %u\r\n", pin_soc);

    (void) duk_get_prop_string(ctx,1, "gpio_id");
    qapi_GPIO_ID_t gpio_id = duk_to_uint32(ctx, -1);
    //printf("obj.propertyVal= %u\r\n", gpio_id);


    if(val)
        qapi_TLMM_Drive_Gpio(gpio_id, pin_soc, QAPI_GPIO_HIGH_VALUE_E);
    else
        qapi_TLMM_Drive_Gpio(gpio_id, pin_soc, QAPI_GPIO_LOW_VALUE_E);

    return 0;
}




static duk_ret_t config(duk_context *ctx, void *udata) {
    /* ... */

    uint32_t pin = duk_require_uint(ctx, 0);
    uint32_t pull = duk_require_uint(ctx, 1);
    uint32_t dir = duk_require_uint(ctx, 2);

    qapi_Status_t status;


    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){

            uint8_t index = pin; // pin is the index as we are using consts string names
            tlmm_config[index].pin = gpio_map_tbl[index].gpio_id; 
            tlmm_config[index].func = 0; 
            // pin mux value 1
            tlmm_config[index].dir = dir;
            tlmm_config[index].pull =  pull; 
            tlmm_config[index].drive = QAPI_GPIO_2MA_E; // drive is for output pins, specify
            // the default here
            status = qapi_TLMM_Get_Gpio_ID( &tlmm_config[index], &gpio_id_tbl[index]);
            if (status == QAPI_OK){
                status = qapi_TLMM_Config_Gpio(gpio_id_tbl[index], &tlmm_config[index]);
            }

            if(status != QAPI_OK){
                duk_error(ctx,DUK_ERR_TYPE_ERROR, "config-> status:%d, pin:%d",  status,pin);
                (void) duk_throw(ctx);
            }

            duk_push_number(ctx, (long)gpio_map_tbl[index].gpio_id);  // pin_soc
            duk_push_number(ctx, (long)gpio_id_tbl[index]);           // gpio_id

            return 2;  /* nrets */
    }

   
    return 0;   /* nrets */

}


/*
*   Called and sent to event manager when interrupt happens
*/
static void invoke_interrupt_cb(qapi_GPIOINT_Callback_Data_t arg) {
    event_msg_t evt;
    evt.msg_id = MSG_TYPE_GPIO_INT;
    evt.msg = (void*)gpio_map_tbl[arg].int_name;
    tx_queue_send(event_mngr_msg_queue, &evt, TX_NO_WAIT);
    tx_semaphore_put (evnt_gatekeeper);
}



static duk_ret_t on(duk_context *ctx) {

    qapi_GPIOINT_Trigger_e trigger = -1;

    const char *param = duk_require_string(ctx, 0);
    duk_require_function(ctx, 1);

    duk_push_this(ctx);

    (void) duk_get_prop_string(ctx, 2, "pin");
    uint32_t pin = duk_to_uint32(ctx, -1);


    if(!duk_get_global_string(ctx,gpio_map_tbl[pin].int_name)){
        
        if(strncmp(param,"fall",4) == 0){
            trigger = QAPI_GPIOINT_TRIGGER_EDGE_FALLING_E;
        }else if(strncmp(param,"rise",4) == 0){
            trigger = QAPI_GPIOINT_TRIGGER_EDGE_RISING_E;
        }else if(strncmp(param,"high",4) == 0){
            trigger = QAPI_GPIOINT_TRIGGER_LEVEL_HIGH_E;
        }else if(strncmp(param,"low",3) == 0){
            trigger = QAPI_GPIOINT_TRIGGER_LEVEL_LOW_E;
        }else if(strncmp(param,"dual",4) == 0){
            trigger = QAPI_GPIOINT_TRIGGER_EDGE_DUAL_E;
        }else{
            duk_error(ctx,DUK_ERR_TYPE_ERROR, "invalid argument: %s",  param);
            (void) duk_throw(ctx);
        }

        if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
            qapi_Status_t status = qapi_GPIOINT_Register_Interrupt(&gpio_interrupt_id_tbl[pin],
                                                     gpio_map_tbl[pin].gpio_id,
                                                     invoke_interrupt_cb,
                                                     pin,
                                                     trigger,
                                                     QAPI_GPIOINT_PRIO_LOWEST_E,
                                                     false);
            if(status == QAPI_OK){
                duk_dup(ctx, 1);
                duk_put_global_string(ctx, gpio_map_tbl[pin].int_name);
            }

        }

    }else{
        duk_error(ctx,DUK_ERR_TYPE_ERROR, "invalid argument: %d", (int) pin);
        (void) duk_throw(ctx);
    }   

#if 0
        duk_push_context_dump(ctx);
        printf("%s\r\n", duk_to_string(ctx, -1));
        duk_pop(ctx);  
#endif


    return 0; /* no return value (= undefined) */
}



  
static duk_ret_t trigger(duk_context *ctx) {
   
    duk_push_this(ctx);

    (void) duk_get_prop_string(ctx, 0, "pin");
    uint32_t pin = duk_to_uint32(ctx, -1);

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        qapi_GPIOINT_Trigger_Interrupt(&gpio_interrupt_id_tbl[pin], gpio_map_tbl[pin].gpio_id );
    }


    return 0; /* no return value (= undefined) */
}


static duk_ret_t disable_irq(duk_context *ctx) {
    
    duk_push_this(ctx);

    (void) duk_get_prop_string(ctx, 0, "pin");
    uint32_t pin = duk_to_uint32(ctx, -1);

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        qapi_GPIOINT_Deregister_Interrupt(&gpio_interrupt_id_tbl[pin], gpio_map_tbl[pin].gpio_id);
    }

    return 0; /* no return value (= undefined) */
}

static duk_ret_t enable_irq(duk_context *ctx) {

    duk_push_this(ctx);

    (void) duk_get_prop_string(ctx, 0, "pin");
    uint32_t pin = duk_to_uint32(ctx, -1);

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        qapi_GPIOINT_Enable_Interrupt (&gpio_interrupt_id_tbl[pin], gpio_map_tbl[pin].gpio_id);
    }

    return 0; /* no return value (= undefined) */

}

static duk_ret_t deactivate(duk_context *ctx) {

    duk_push_this(ctx);

    (void) duk_get_prop_string(ctx, 0, "pin");
    uint32_t pin = duk_to_uint32(ctx, -1);

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        qapi_GPIOINT_Deregister_Interrupt (&gpio_interrupt_id_tbl[pin], gpio_map_tbl[pin].gpio_id);
    }

    return 0; /* no return value (= undefined) */

}





static duk_ret_t output(duk_context *ctx) {

    duk_push_this(ctx);

    (void) duk_get_prop_string(ctx, 0, "pin");
    uint32_t pin = duk_to_uint32(ctx, -1);

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        if(qapi_TLMM_Release_Gpio_ID(&tlmm_config[pin], gpio_id_tbl[pin]) == QAPI_OK){

            (void) duk_get_prop_string(ctx, 0, "pull");
            uint32_t pull = duk_to_uint32(ctx, -1);

            duk_push_number(ctx, pin);
            duk_push_number(ctx, pull);
            duk_push_number(ctx, QAPI_GPIO_OUTPUT_E);

            duk_int_t rc =  duk_safe_call(ctx, config, NULL, 3  /*nargs*/, 2 /* nrets */);

            if(rc == DUK_EXEC_SUCCESS){

                uint32_t pin_soc = duk_get_uint(ctx, -2);            // get the hardware mapped pin
                uint32_t gpio_id = duk_get_uint(ctx, -1);
                duk_pop_2(ctx);

                duk_push_number(ctx, 0);
            }else{
                duk_push_number(ctx, 1);
            }

        }
    }


    return 1;
}

static duk_ret_t input(duk_context *ctx) {

    duk_push_this(ctx);

    (void) duk_get_prop_string(ctx, 0, "pin");
    uint32_t pin = duk_to_uint32(ctx, -1);

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        if(qapi_TLMM_Release_Gpio_ID(&tlmm_config[pin], gpio_id_tbl[pin]) == QAPI_OK){

            (void) duk_get_prop_string(ctx, 0, "pull");
            uint32_t pull = duk_to_uint32(ctx, -1);

            duk_push_number(ctx, pin);
            duk_push_number(ctx, pull);
            duk_push_number(ctx, QAPI_GPIO_INPUT_E);

            duk_int_t rc =  duk_safe_call(ctx, config, NULL, 3  /*nargs*/, 2 /* nrets */);

            if(rc == DUK_EXEC_SUCCESS){

                uint32_t pin_soc = duk_get_uint(ctx, -2);            // get the hardware mapped pin
                uint32_t gpio_id = duk_get_uint(ctx, -1);
                duk_pop_2(ctx);

                duk_push_number(ctx, 0);
            }else{
                duk_push_number(ctx, 1);
            }

        }
    }

    return 1;


}

static duk_ret_t release(duk_context *ctx) {

    duk_push_this(ctx);

    (void) duk_get_prop_string(ctx, 0, "pin");
    uint32_t pin = duk_to_uint32(ctx, -1);

    if( pin >= PIN_E_GPIO_01 && pin < PIN_E_GPIO_MAX){
        if(qapi_TLMM_Release_Gpio_ID(&tlmm_config[pin], gpio_id_tbl[pin]) == QAPI_OK){

            duk_push_number(ctx, -1);
            duk_put_prop_string(ctx, 0, "pin_soc");

            duk_push_number(ctx, -1);
            duk_put_prop_string(ctx, 0, "gpio_id");
        }
    }

    return 0; /* no return value (= undefined) */
}


static duk_ret_t finalizer(duk_context *ctx) {
    /* Object being finalized is at stack index 0. */
    return release(ctx);
}


duk_ret_t native_digital_in(duk_context *ctx) {
   
    uint32_t pin = duk_require_uint(ctx, 0);
    uint32_t pull = QAPI_GPIO_KEEPER_E; // default

    if (!duk_is_undefined(ctx, 1) ) {
        pull = duk_require_uint(ctx, 1);
    }else{
        duk_pop(ctx);   /* [ ... undefined] */
        duk_push_number(ctx, pull);
    }


    duk_push_number(ctx, QAPI_GPIO_INPUT_E);

    duk_int_t rc =  duk_safe_call(ctx, config, NULL, 3  /*nargs*/, 2 /* nrets */);

    if(rc == DUK_EXEC_SUCCESS){

        uint32_t pin_soc = duk_get_uint(ctx, -2);            // get the hardware mapped pin
        uint32_t gpio_id = duk_get_uint(ctx, -1);
        
        duk_pop_2(ctx);

        duk_idx_t obj_idx = duk_push_object(ctx);   /* -> [ ... global obj ] */

        /* Create an object finalizer(). */
        duk_push_c_function(ctx, finalizer, 1 /*nargs*/);
        duk_set_finalizer(ctx, obj_idx);

        duk_push_number(ctx, pin);
        duk_put_prop_string(ctx, obj_idx, "pin");

        duk_push_number(ctx, pin_soc);
        duk_put_prop_string(ctx, obj_idx, "pin_soc");

        duk_push_number(ctx, gpio_id);
        duk_put_prop_string(ctx, obj_idx, "gpio_id");

        duk_push_c_function(ctx, read, 0  /*nargs*/);
        duk_put_prop_string(ctx, obj_idx, "read");

        duk_push_c_function(ctx, release, 0  /*nargs*/);
        duk_put_prop_string(ctx, obj_idx, "release");
#if 0
        duk_push_context_dump(ctx);
        printf("%s\r\n", duk_to_string(ctx, -1));
        duk_pop(ctx);  
#endif
        /* Return value is the module object.  It's up to the caller to decide
        * how to use the value, e.g. write to global object.  The value can also
        * be e.g. a function/constructor instead of an object.
        */

        return 1;
    }else{
        duk_pop_2(ctx);
        return 0;
    }

}

/* Init function name is dukopen_<modname>. */
duk_ret_t native_digital_out(duk_context *ctx) {
   
    uint32_t pin = duk_require_uint(ctx, 0);
    uint32_t pull = QAPI_GPIO_KEEPER_E; // default


    if (!duk_is_undefined(ctx, 1) ) {
        pull = duk_require_uint(ctx, 1);
    }else{
        duk_pop(ctx);   /* [ ... undefined] */
        duk_push_number(ctx, pull);
    }

    duk_push_number(ctx, QAPI_GPIO_OUTPUT_E);

    duk_int_t rc =  duk_safe_call(ctx, config, NULL, 3  /*nargs*/, 2 /* nrets */);

    if(rc == DUK_EXEC_SUCCESS){

        uint32_t pin_soc = duk_get_uint(ctx, -2);            // get the hardware mapped pin
        uint32_t gpio_id = duk_get_uint(ctx, -1);

        duk_pop_2(ctx);

        duk_idx_t obj_idx = duk_push_object(ctx);   /* -> [ ... global obj ] */

        /* Create an object finalizer(). */
        duk_push_c_function(ctx, finalizer, 1 /*nargs*/);
        duk_set_finalizer(ctx, obj_idx);

        duk_push_number(ctx, pin);
        duk_put_prop_string(ctx, obj_idx, "pin");

        duk_push_number(ctx, pull);
        duk_put_prop_string(ctx, obj_idx, "pull");

        duk_push_number(ctx, pin_soc);
        duk_put_prop_string(ctx, obj_idx, "pin_soc");

        duk_push_number(ctx, gpio_id);
        duk_put_prop_string(ctx, obj_idx, "gpio_id");

        duk_push_c_function(ctx, write, 1  /*nargs*/);
        duk_put_prop_string(ctx, obj_idx, "write");

        duk_push_c_function(ctx, release, 0  /*nargs*/);
        duk_put_prop_string(ctx, obj_idx, "release");

#if 0
        duk_push_context_dump(ctx);
        printf("%s\r\n", duk_to_string(ctx, -1));
        duk_pop(ctx);  
#endif
        /* Return value is the module object.  It's up to the caller to decide
        * how to use the value, e.g. write to global object.  The value can also
        * be e.g. a function/constructor instead of an object.
        */

        return 1;
    }else{
        duk_pop_2(ctx);
        return 0;
    }

}

/* Init function name is dukopen_<modname>. */
duk_ret_t native_digital_in_out(duk_context *ctx) {
   
    uint32_t pin = duk_require_uint(ctx, 0);
    uint32_t pull = QAPI_GPIO_KEEPER_E; // default


    if (!duk_is_undefined(ctx, 1) ) {
        pull = duk_require_uint(ctx, 1);
    }else{
        duk_pop(ctx);   /* [ ... undefined] */
        duk_push_number(ctx, pull);
    }

    duk_idx_t obj_idx = duk_push_object(ctx);   /* -> [ ... global obj ] */

    /* Create an object finalizer(). */
    duk_push_c_function(ctx, finalizer, 1 /*nargs*/);
    duk_set_finalizer(ctx, obj_idx);

    duk_push_number(ctx, pin);
    duk_put_prop_string(ctx, obj_idx, "pin");

    duk_push_number(ctx, pull);
    duk_put_prop_string(ctx, obj_idx, "pull");

    duk_push_number(ctx, -1);
    duk_put_prop_string(ctx, obj_idx, "pin_soc");

    duk_push_number(ctx, -1);
    duk_put_prop_string(ctx, obj_idx, "gpio_id");

    duk_push_c_function(ctx, write, 1  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "write");

    duk_push_c_function(ctx, read, 0  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "read");

    duk_push_c_function(ctx, release, 0  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "release");

#if 0
        duk_push_context_dump(ctx);
        printf("%s\r\n", duk_to_string(ctx, -1));
        duk_pop(ctx);  
#endif
     

    /* Return value is the module object.  It's up to the caller to decide
     * how to use the value, e.g. write to global object.  The value can also
     * be e.g. a function/constructor instead of an object.
     */
    return 1;
}

duk_ret_t native_digital_interrupt_in(duk_context *ctx) {
   
    uint32_t pin = duk_require_uint(ctx, 0);
    uint32_t pull = QAPI_GPIO_PULL_UP_E; // default


    if (!duk_is_undefined(ctx, 1) ) {
        pull = duk_require_uint(ctx, 1);
    }else{
        duk_pop(ctx);   /* [ ... undefined] */
        duk_push_number(ctx, pull);
    }


    duk_push_number(ctx, QAPI_GPIO_INPUT_E);



    duk_int_t rc =  duk_safe_call(ctx, config, NULL, 3  /*nargs*/, 2 /* nrets */);


    if(rc == DUK_EXEC_SUCCESS){

        uint32_t pin_soc = duk_get_uint(ctx, -2);            // get the hardware mapped pin
        uint32_t gpio_id = duk_get_uint(ctx, -1);
        
        duk_pop_2(ctx);


        duk_idx_t obj_idx = duk_push_object(ctx);   /* -> [ ... global obj ] */

        /* Create an object finalizer(). */
        duk_push_c_function(ctx, finalizer, 1 /*nargs*/);
        duk_set_finalizer(ctx, obj_idx);
        
        duk_push_number(ctx, pin);
        duk_put_prop_string(ctx, obj_idx, "pin");

        duk_push_number(ctx, pin_soc);
        duk_put_prop_string(ctx, obj_idx, "pin_soc");

        duk_push_number(ctx, pull);
        duk_put_prop_string(ctx, obj_idx, "pull");

        duk_push_number(ctx, gpio_id);
        duk_put_prop_string(ctx, obj_idx, "gpio_id");

        duk_push_c_function(ctx, on, 2  /*nargs*/);
        duk_put_prop_string(ctx, obj_idx, "on");

        duk_push_c_function(ctx, trigger, 0  /*nargs*/);
        duk_put_prop_string(ctx, obj_idx, "trigger");

        duk_push_c_function(ctx, enable_irq, 0  /*nargs*/);
        duk_put_prop_string(ctx, obj_idx, "enable");

        duk_push_c_function(ctx, disable_irq, 0  /*nargs*/);
        duk_put_prop_string(ctx, obj_idx, "disable");

        duk_push_c_function(ctx, deactivate, 0  /*nargs*/);
        duk_put_prop_string(ctx, obj_idx, "deactivate");

#if 0
        duk_push_context_dump(ctx);
        printf("%s\r\n", duk_to_string(ctx, -1));
        duk_pop(ctx);  
#endif
        /* Return value is the module object.  It's up to the caller to decide
        * how to use the value, e.g. write to global object.  The value can also
        * be e.g. a function/constructor instead of an object.
        */

        return 1;

    }else{
        duk_pop_2(ctx);
        return 0;
    }

}





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


duk_ret_t native_init_gpio_consts(duk_context *ctx){
    duk_push_global_object(ctx);
    duk_put_number_list(ctx, -1, gpio_module_consts);
    duk_pop(ctx);  /* [ ... ]   */
    return 1;
}


