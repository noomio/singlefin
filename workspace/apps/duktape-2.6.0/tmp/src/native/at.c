#include "txm_module.h"
#include "qapi_quectel.h"
#include "duktape.h"
#include "event_mngr.h"


static void invoke_callback_pipe0(pipe_data_t *resp);
static void invoke_callback_pipe1(pipe_data_t *resp);

enum PIPE_NUM {
	PIPE0 = 0,
	PIPE1 = 1,
	PIPE_MAX
};


at_pipe_map_tbl_t at_pipe_map_tbl[PIPE_MAX] = {
    {  	-1,  0,  NULL, "_at_pipe0_open",	"_at_pipe0_close", "_at_pipe0_data", "_at_pipe0_error", {0,0}, NULL, 0, invoke_callback_pipe0, NULL},
    {  	-1,  1,  NULL, "_at_pipe1_open",	"_at_pipe1_close", "_at_pipe1_data", "_at_pipe1_error", {0,0}, NULL, 0, invoke_callback_pipe1, NULL}
};


static void invoke_callback_pipe0(pipe_data_t *resp){
    event_msg_t evt;
    evt.msg_id = MSG_TYPE_AT_CB_PIPE;
    evt.msg = (void*)resp;
    tx_queue_front_send(at_data_queue, &evt, TX_NO_WAIT);
	//tx_semaphore_put (evnt_gatekeeper);
}

static void invoke_callback_pipe1(pipe_data_t *resp){
    event_msg_t evt;
    evt.msg_id = MSG_TYPE_AT_CB_PIPE;
    evt.msg = (void*)resp;
    tx_queue_front_send(at_data_queue, &evt, TX_NO_WAIT);
	//tx_semaphore_put (evnt_gatekeeper);
}


static duk_ret_t write(duk_context *ctx){

	duk_ret_t stat = 1;

	const char *cmd = duk_require_string(ctx, 0);
    
    duk_push_this(ctx);


	(void) duk_get_prop_string(ctx, 1, "port");
    int port = duk_to_int(ctx, -1);


	if(tx_semaphore_get(at_pipe_map_tbl[port].lock,TX_NO_WAIT) == TX_SUCCESS){ 

		if(at_pipe_map_tbl[port].buffer != NULL)
			free(at_pipe_map_tbl[port].buffer);
		
		at_pipe_map_tbl[port].buffer_tot_len = 0;		
		at_pipe_map_tbl[port].cmd = cmd;
		event_msg_t evt;
		evt.msg_id = MSG_TYPE_AT_WRITE;
		evt.msg = (void*)&at_pipe_map_tbl[port];		
		stat = tx_queue_send(at_msg_queue, &evt, TX_WAIT_FOREVER);
	}


	duk_push_int(ctx,stat);

	return 1;

}

static duk_ret_t close(duk_context *ctx){
  	duk_push_this(ctx);

    (void) duk_get_prop_string(ctx, 0, "port");
    int port = duk_to_int(ctx, -1);

	tx_queue_flush(at_msg_queue);
   	qapi_QT_Apps_AT_Port_Close(at_pipe_map_tbl[port].stream);

	txm_module_object_deallocate(&at_pipe_map_tbl[port].lock);
	tx_semaphore_delete(at_pipe_map_tbl[port].lock);
	at_pipe_map_tbl[port].lock = NULL;

	event_msg_t evt;
    evt.msg_id = MSG_TYPE_AT_CLOSE;
    evt.msg = (void*)at_pipe_map_tbl[port].close;
    tx_queue_send(event_mngr_msg_queue, &evt, TX_NO_WAIT);
	tx_semaphore_put (evnt_gatekeeper);

	return 0;
}


static duk_ret_t on(duk_context *ctx){

	const char *param = duk_require_string(ctx, 0);
	duk_require_function(ctx, 1);

	duk_push_this(ctx);

	(void) duk_get_prop_string(ctx, 2, "port");
    int port = duk_to_int(ctx, -1);

    if(port >= 0 && port < PIPE_MAX){

		if(strncmp(param,"open",4) == 0){
			at_pipe_map_tbl[port].port = port;
			int status;
			if((status = qapi_QT_Apps_AT_Port_Open(at_pipe_map_tbl[port].port, &at_pipe_map_tbl[port].stream, at_pipe_map_tbl[port].callback, &at_pipe_map_tbl[port].pipe)) == QAPI_QT_ERR_OK){
				duk_dup(ctx,1);
				duk_put_global_string(ctx, at_pipe_map_tbl[port].open);

				txm_module_object_allocate(&at_pipe_map_tbl[port].lock, sizeof(TX_SEMAPHORE));
				tx_semaphore_create(at_pipe_map_tbl[port].lock,"lock", 1);
				
				event_msg_t evt;
			    evt.msg_id = MSG_TYPE_AT_OPEN;
			    evt.msg = (void*)at_pipe_map_tbl[port].open;
			    tx_queue_send(event_mngr_msg_queue, &evt, TX_NO_WAIT);
				tx_semaphore_put (evnt_gatekeeper);

			}else{
				duk_error(ctx,DUK_ERR_TYPE_ERROR, "native_at port open error: %d", status);
		        (void) duk_throw(ctx);
			}

		}else if(strncmp(param,"data",4) == 0){
			duk_dup(ctx,1);
			duk_put_global_string(ctx, at_pipe_map_tbl[port].data);
		}else if(strncmp(param,"close",5) == 0){



		}else{
			duk_error(ctx,DUK_ERR_ERROR, "native_at on %s", param);
		    (void) duk_throw(ctx);
		}
	}else{
		duk_error(ctx,DUK_ERR_ERROR, "native_at on %s", param);
	    (void) duk_throw(ctx);
	}

#if 0
        duk_push_context_dump(ctx);
        printf("%s\r\n", duk_to_string(ctx, -1));
        duk_pop(ctx);  
#endif

	return 1;

}

duk_ret_t native_at(duk_context *ctx) {

	uint32_t at_port = duk_require_uint(ctx, 0);

	duk_idx_t obj_idx = duk_push_object(ctx);   /* -> [ ... global obj ] */

    duk_push_number(ctx, at_port);
    duk_put_prop_string(ctx, obj_idx, "port");


    duk_push_c_function(ctx, write, 1  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "write");

    duk_push_c_function(ctx, close, 0  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "close");

    duk_push_c_function(ctx, on, 2 /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "on");

#if 0
        duk_push_context_dump(ctx);
        printf("%s\r\n", duk_to_string(ctx, -1));
        duk_pop(ctx);  
#endif

	return 1;

}


static const duk_number_list_entry at_module_consts[] = {
    { "PIPE0",       0 },
    { "PIPE1",       1 },
    { NULL, 0 }
};


duk_ret_t native_init_at_consts(duk_context *ctx){
    duk_push_global_object(ctx);
    duk_put_number_list(ctx, -1, at_module_consts);
    duk_pop(ctx);  /* [ ... ]   */
    return 1;
}

