#include "txm_module.h"
#include "qapi_quectel.h"
#include "duktape.h"
#include "event_mngr.h"
#include "qapi_i2c_master.h"


enum I2C_NUM {
	I2C1 = 0,
	I2C2 = 1,
	I2C_MAX
};

i2c_map_tbl_t i2c_map_tbl[I2C_MAX] = {
    {  	1,  QAPI_I2CM_INSTANCE_004_E,      	NULL,     	0,	0,	0,	"_i2c01_open",	"_i2c01_close", "_i2c01_data", "_i2c01_error", "_i2c01_rx_ref", NULL, NULL},
    {  	2,  QAPI_I2CM_INSTANCE_005_E,      	NULL,       1,	0,	0,	"_i2c02_open",	"_i2c02_close", "_i2c02_data", "_i2c02_error", "_i2c02_rx_ref", NULL, NULL}
};


static void invoke_i2c_callback_1(const uint32 status, void *param){

	event_msg_t evt;
	evt.msg_id = MSG_TYPE_I2C_TRANSFER_CB;
	evt.msg = (void*)&i2c_map_tbl[I2C1];
	tx_queue_send(event_mngr_msg_queue, &evt, TX_NO_WAIT);
	tx_semaphore_put (evnt_gatekeeper);
	tx_semaphore_ceiling_put(i2c_map_tbl[I2C1].lock,1);
}

static void invoke_i2c_callback_2(const uint32 status, void *param){

	event_msg_t evt;
	evt.msg_id = MSG_TYPE_I2C_TRANSFER_CB;
	evt.msg = (void*)&i2c_map_tbl[I2C2];
	tx_queue_send(event_mngr_msg_queue, &evt, TX_NO_WAIT);
	tx_semaphore_put (evnt_gatekeeper);
	tx_semaphore_ceiling_put(i2c_map_tbl[I2C2].lock,1);
}



static duk_ret_t transfer(duk_context *ctx){

	int stat;
	uint32_t addr = duk_require_uint(ctx, 0);

	if (duk_is_buffer(ctx, 1)) {

		duk_size_t sz_tx, sz_rx;

		void *tx_data = duk_get_buffer(ctx, 1, &sz_tx);
		//printf("txbuf=%p, size=%lu\r\n", tx_data, (unsigned long) sz_tx);

		void *rx_data = duk_get_buffer(ctx, 2, &sz_rx);
		//printf("rxbuf=%p, size=%lu\r\n", rx_data, (unsigned long) sz_rx);


		duk_push_this(ctx);

		(void) duk_get_prop_string(ctx, 3, "frequency");
	    int frequency = duk_to_int(ctx, -1);

		(void) duk_get_prop_string(ctx, 3, "id");
	    int id = duk_to_int(ctx, -1);

	    if(id >= I2C_MAX || id < 0)
        	(void) duk_throw(ctx);


		/* keep the ref and avoid garbage collection */
		i2c_map_tbl[id].rx_data_ref = duk_get_heapptr(ctx, 2);
		duk_put_global_string(ctx, i2c_map_tbl[id].rx_name_ref);
		//printf("ref=%p\r\n",i2c_map_tbl[id].rx_data_ref);

	    qapi_I2CM_Config_t config;
	   	qapi_I2CM_Descriptor_t desc[2];


	   	// Configure the bus speed and slave address
		config.bus_Frequency_KHz = frequency; 
		config.slave_Address     = addr;
		config.SMBUS_Mode        = 0;
		config.slave_Max_Clock_Stretch_Us = 100000;
		config.core_Configuration1 = 0;
		config.core_Configuration2 = 0;

		uint8_t to_send = 0;

		if(tx_data != NULL){
			desc[0].buffer      = tx_data;
			desc[0].length      = sz_tx;
			desc[0].transferred = (uint32_t)&i2c_map_tbl[id].transferred1;
			desc[0].flags       = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_WRITE | QAPI_I2C_FLAG_STOP;
			to_send++;
 		}

		if(rx_data != NULL){
			desc[1].buffer      = rx_data;
			desc[1].length      = sz_rx;
			desc[1].transferred = (uint32_t)&i2c_map_tbl[id].transferred2;  
			desc[1].flags       = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_READ  | QAPI_I2C_FLAG_STOP;
			to_send++;
		}

		if(tx_semaphore_get(i2c_map_tbl[id].lock,TX_NO_WAIT) == TX_SUCCESS){ 
			if((stat = qapi_I2CM_Transfer(i2c_map_tbl[id].handle, &config, &desc[0], to_send, invoke_i2c_callback_1, &i2c_map_tbl[id].param, 100)) == QAPI_OK){
				duk_push_int(ctx,0);
			}else{
				duk_push_int(ctx,stat);
			}
		}else{
			// call error
		}

	}

	return 1;
}



static duk_ret_t frequency(duk_context *ctx){

	uint32_t frequency = duk_require_uint(ctx, 0);

	duk_push_this(ctx);

	duk_push_number(ctx, frequency);
    duk_put_prop_string(ctx, 1, "khz");


	return 0;
}

static duk_ret_t start(duk_context *ctx){

	duk_push_this(ctx);

	(void) duk_get_prop_string(ctx, 0, "frequency");
    int frequency = duk_to_int(ctx, -1);

	(void) duk_get_prop_string(ctx, 0, "id");
    int id = duk_to_int(ctx, -1);

    if(id >= I2C_MAX || id < 0)
    	(void) duk_throw(ctx);

    qapi_I2CM_Config_t config;
   	qapi_I2CM_Descriptor_t desc[1];


   	// Configure the bus speed and slave address
	config.bus_Frequency_KHz = frequency; 
	config.slave_Address     = 0;
	config.SMBUS_Mode        = 0;
	config.slave_Max_Clock_Stretch_Us = 100000;
	config.core_Configuration1 = 0;
	config.core_Configuration2 = 0;

	desc[0].buffer      = NULL;
	desc[0].length      = 0;
	desc[0].transferred = 0; // (uint32)&transferred1;
	desc[0].flags       = QAPI_I2C_FLAG_START;


	if(qapi_I2CM_Transfer(i2c_map_tbl[id].handle, &config, &desc[0], 1, NULL, &i2c_map_tbl[id].param, 0) == QAPI_OK){

	}else{
		// call error
	}

	return 0;
}

static duk_ret_t stop(duk_context *ctx){

	duk_push_this(ctx);

	(void) duk_get_prop_string(ctx, 3, "frequency");
    int frequency = duk_to_int(ctx, -1);

	(void) duk_get_prop_string(ctx, 3, "id");
    int id = duk_to_int(ctx, -1);

    if(id >= I2C_MAX || id < 0)
    	(void) duk_throw(ctx);

    qapi_I2CM_Config_t config;
   	qapi_I2CM_Descriptor_t desc[2];


   	// Configure the bus speed and slave address
	config.bus_Frequency_KHz = frequency; 
	config.slave_Address     = 0;
	config.SMBUS_Mode        = 0;
	config.slave_Max_Clock_Stretch_Us = 100000;
	config.core_Configuration1 = 0;
	config.core_Configuration2 = 0;

	desc[0].buffer      = NULL;
	desc[0].length      = 0;
	desc[0].transferred = 0; // (uint32)&transferred1;
	desc[0].flags       = QAPI_I2C_FLAG_STOP;


	if(qapi_I2CM_Transfer(i2c_map_tbl[id].handle, &config, &desc[0], 1, NULL, &i2c_map_tbl[id].param, 0) == QAPI_OK){

	}else{
		// call error
	}

	return 0;
}

static duk_ret_t on(duk_context *ctx){

	const char *param = duk_require_string(ctx, 0);
	duk_require_function(ctx, 1);

	duk_push_this(ctx);

	(void) duk_get_prop_string(ctx, 2, "id");
    int id = duk_to_int(ctx, -1);

    if(id >= I2C_MAX || id < 0)
        (void) duk_throw(ctx);

	if(strncmp(param,"open",4) == 0){

		if(qapi_I2CM_Open(i2c_map_tbl[id].instance, &i2c_map_tbl[id].handle) == QAPI_OK && i2c_map_tbl[id].lock == NULL){
		   	
		   	qapi_I2CM_Power_On(i2c_map_tbl[id].handle);

			txm_module_object_allocate(&i2c_map_tbl[id].lock, sizeof(TX_SEMAPHORE));
			tx_semaphore_create(i2c_map_tbl[id].lock,"lock", 1);

	        duk_dup(ctx, 1);
	        duk_put_global_string(ctx, i2c_map_tbl[id].open);

	        event_msg_t evt;
		    evt.msg_id = MSG_TYPE_I2C_OPEN;
		    evt.msg = (void*)i2c_map_tbl[id].open;
		    tx_queue_send(event_mngr_msg_queue, &evt, TX_NO_WAIT);
			tx_semaphore_put (evnt_gatekeeper);

		}else{
			// call error
		}


	}else if(strncmp(param,"close",5) == 0){

		if(qapi_I2CM_Close (i2c_map_tbl[id].handle) == QAPI_OK){

	        duk_dup(ctx, 1);
	        duk_put_global_string(ctx, i2c_map_tbl[id].close);

			event_msg_t evt;
		    evt.msg_id = MSG_TYPE_I2C_CLOSE;
		    evt.msg = (void*)i2c_map_tbl[id].close;
		    tx_queue_send(event_mngr_msg_queue, &evt, TX_NO_WAIT);
			tx_semaphore_put (evnt_gatekeeper);

			txm_module_object_deallocate(&i2c_map_tbl[id].lock);
			tx_semaphore_delete(i2c_map_tbl[id].lock);
			i2c_map_tbl[id].lock = NULL;

		}else{
			// call error
		}




	}else if(strncmp(param,"data",4) == 0){

		duk_dup(ctx, 1);
	    duk_put_global_string(ctx, i2c_map_tbl[id].data);

	}else if(strncmp(param,"error",5) == 0){

		duk_dup(ctx, 1);
	    duk_put_global_string(ctx, i2c_map_tbl[id].error);

	}else{
		duk_error(ctx,DUK_ERR_ERROR, "native_i2c on %s", param);
	    (void) duk_throw(ctx);
	}



#if 0
        duk_push_context_dump(ctx);
        printf("%s\r\n", duk_to_string(ctx, -1));
        duk_pop(ctx);  
#endif

	return 1;

}

duk_ret_t native_i2c(duk_context *ctx) {

	int id = duk_require_uint(ctx, 0);

	duk_idx_t obj_idx = duk_push_object(ctx);   /* -> [ ... global obj ] */

    duk_push_number(ctx, id);
    duk_put_prop_string(ctx, obj_idx, "id");

    duk_push_number(ctx, 100);
    duk_put_prop_string(ctx, obj_idx, "khz"); /* default freq 100kHz */

    duk_push_c_function(ctx, start, 0  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "start");

    duk_push_c_function(ctx, transfer, 3  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "transfer");

    duk_push_c_function(ctx, stop, 0  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "stop");

    duk_push_c_function(ctx, frequency, 1  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "frequency");

    duk_push_c_function(ctx, on, 2 /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "on");



#if 0
        duk_push_context_dump(ctx);
        printf("%s\r\n", duk_to_string(ctx, -1));
        duk_pop(ctx);  
#endif

	return 1;

}


static const duk_number_list_entry i2c_module_consts[] = {
    { "I2C1",       I2C1 },
    { "I2C2",       I2C2 },
    { NULL, 0 }
};


duk_ret_t native_init_i2c_consts(duk_context *ctx){
    duk_push_global_object(ctx);
    duk_put_number_list(ctx, -1, i2c_module_consts);
    duk_pop(ctx);  /* [ ... ]   */
    return 1;
}
