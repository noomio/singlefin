#include "malloc.h"
#include "duktape.h"
#include "qapi_timer.h"
#include "event_mngr.h"
#include "qapi_quectel.h"


#if 0 // not working as not enabled during threadx compilation
static void evnt_mngr_send_notify(TX_QUEUE *queue)
{
	tx_semaphore_put (evnt_gatekeeper);
}

static void at_send_notify(TX_QUEUE *queue)
{
	tx_semaphore_put (evnt_gatekeeper);
}
#endif

int event_mngr_task(ULONG arg){

	event_msg_t evt;

	duk_context *ctx = (duk_context *)arg;

	txm_module_object_allocate(&evnt_gatekeeper, sizeof(TX_SEMAPHORE));
	tx_semaphore_create (evnt_gatekeeper, "evnt_gatekeeper", 0);

	txm_module_object_allocate(&evnt_group, sizeof(TX_EVENT_FLAGS_GROUP));
	tx_event_flags_create(evnt_group,"evnt_group");

	/*
	*	EVENT MNGR MSG QUEUE
	*/ 
	txm_module_object_allocate(&event_mngr_msg_queue, sizeof(TX_QUEUE));
	tx_queue_create(event_mngr_msg_queue, "event_mngr_msg_mem",EVENT_MNGR_ELEMENT_SIZE, event_mngr_msg_mem, sizeof(event_mngr_msg_mem));
	//tx_queue_send_notify(event_mngr_msg_queue, evnt_mngr_send_notify);

	/*
	* 	TIMER OBJ ALLOCATION
	*/
	txm_module_object_allocate(&timer_pool, sizeof(TX_BLOCK_POOL));
	tx_block_pool_create(timer_pool, "timer_pool", sizeof(timer_store_msg_t), timer_mem, sizeof(timer_mem));

	/*
	*	AT MSG QUEUE
	*/ 
	txm_module_object_allocate(&at_msg_queue, sizeof(TX_QUEUE));
	tx_queue_create(at_msg_queue, "at_msg_mem",AT_MSG_ELEMENT_SIZE, at_msg_mem, sizeof(at_msg_mem));

	txm_module_object_allocate(&at_data_queue, sizeof(TX_QUEUE));
	tx_queue_create(at_data_queue, "at_data_mem",AT_MSG_ELEMENT_SIZE, at_msg_mem, sizeof(at_msg_mem));
	//tx_queue_send_notify(at_msg_queue, at_send_notify);


	for(;;){


		tx_semaphore_get (evnt_gatekeeper, TX_WAIT_FOREVER);

		if(tx_queue_receive(event_mngr_msg_queue, &evt, TX_NO_WAIT) == TX_SUCCESS){	

			if(evt.msg_id == MSG_TYPE_TIMER){

	 			timer_store_msg_t *t = (timer_store_msg_t *)evt.msg;
	 
	 			if(t != NULL){

				    /* Get current callback. */
				    duk_get_global_string(ctx, t->name);

				    /* Explicit check for callback existence; log and exit if no callback. */
				    if (!duk_is_function(ctx, -1)) {
				        puts("No callback registered\r\n");
				        duk_set_top(ctx, -1); 
					    continue;
				    }
				    

				    if(t->oneshot){	/* Remove registered callback to avoid calling again. */						
				    	duk_push_null(ctx);
				    	duk_put_global_string(ctx, t->name);
				    	tx_timer_delete(t->timer); /* free timer */
				   		txm_module_object_deallocate(&t->timer);
				    	tx_block_release(t);
				    }

				    duk_int_t rc = duk_pcall(ctx, 0);

				    if (rc != 0) {
				        printf("Callback failed: '%s'\r\n", duk_safe_to_string(ctx, -1));
				    }

			    	duk_pop(ctx);
	 			}


	    	}else if(evt.msg_id == MSG_TYPE_GPIO_INT){
				//printf("%s: msg gpio int\r\n", __func__);

	    		const char *name = (const char *)evt.msg;

	    		if(name != NULL){
				    /* Get current callback. */
				    duk_get_global_string(ctx, name);

				    if (!duk_is_function(ctx, -1)) {

				        puts("No callback registered\r\n");
				        duk_set_top(ctx, -1); 

				    }else{

					    duk_int_t rc = duk_pcall(ctx, 0 /*args*/);
					    if (rc != 0) {
					        printf("Callback failed: '%s'\r\n", duk_safe_to_string(ctx, -1));
					    }

				    	duk_pop(ctx);
			    	}
		    	}

	    	}else if(evt.msg_id == MSG_TYPE_AT_OPEN){

	    		const char *name = (const char *)evt.msg;

	    		if(name != NULL){
				    /* Get current callback. */
				    duk_get_global_string(ctx, name);

					if (!duk_is_function(ctx, -1)) {
					    puts("No callback registered\r\n");
				        duk_set_top(ctx, -1); 
				    }else{

				    	duk_int_t rc = duk_pcall(ctx, 0 /*args*/);

					    if (rc != 0) {
					        printf("Callback failed: '%s'\r\n", duk_safe_to_string(ctx, -1));
					    }

				    	duk_pop(ctx);
				    }

				    duk_push_null(ctx);
					duk_put_global_string(ctx, name);

			    }

	    	}else if(evt.msg_id == MSG_TYPE_AT_CLOSE){

				const char *name = (const char *)evt.msg;
				printf("%s\r", name);
	    		if(name != NULL){
				    /* Get current callback. */
				    duk_get_global_string(ctx, name);

					if (!duk_is_function(ctx, -1)) {
					    duk_int_t rc = duk_pcall(ctx, 0 /*args*/);

					    if (rc != 0) {
					        printf("Callback failed: '%s'\r\n", duk_safe_to_string(ctx, -1));
					    }

				    	duk_pop(ctx);
				    }

			    }

				duk_push_null(ctx);
				duk_put_global_string(ctx, name);


	    	}else if(evt.msg_id == MSG_TYPE_AT_WRITE_CB){

				at_pipe_map_tbl_t *at = (at_pipe_map_tbl_t *)evt.msg;

				duk_get_global_string(ctx, at->data);	
				duk_push_string(ctx,at->buffer);	

				if (!duk_is_function(ctx, -2)) {
					duk_set_top(ctx, -1); 	   
					puts("No callback registered\r\n");		    

			    }else{

				    duk_int_t rc = duk_pcall(ctx, 1 /*args*/);

				    if (rc != 0) {
				        printf("Callback failed: '%s'\r\n", duk_safe_to_string(ctx, -1));
				    }

		    		duk_pop(ctx);
		    	}

		    	tx_semaphore_ceiling_put(at->lock,1);
				free(at->buffer);
				at->buffer = NULL;


	    	}else if(evt.msg_id == MSG_TYPE_I2C_OPEN){
		   		
	    		const char *name = (const char *)evt.msg;

	    		if(name != NULL){
				    /* Get current callback. */
				    duk_get_global_string(ctx, name);

				    if (!duk_is_function(ctx, -1)) {

				        puts("No callback registered\r\n");
				        duk_set_top(ctx, -1); 

				    }else{

					    duk_int_t rc = duk_pcall(ctx, 0);
					    if (rc != 0) {
					        printf("Callback failed: '%s'\r\n", duk_safe_to_string(ctx, -1));
					    }

				    	duk_pop(ctx);

				    	// null it, no need to call it again
		    			duk_push_null(ctx);
						duk_put_global_string(ctx, name);
			    	}
		    	}


	    	}else if(evt.msg_id == MSG_TYPE_I2C_CLOSE){

				printf("%s\r\n", "i2c close");



	    	}else if(evt.msg_id == MSG_TYPE_I2C_TRANSFER_CB){
				
				i2c_map_tbl_t *i2c = (i2c_map_tbl_t *)evt.msg;
				const char *name = i2c->data;

	    		if(name != NULL){

	    			duk_set_top(ctx, 0); 
				    /* Get current callback. */
				    duk_get_global_string(ctx, name);

				    if (!duk_is_function(ctx, -1)) {

				        puts("No callback registered\r\n");
				        duk_set_top(ctx, -1); 

				    }else{

						duk_push_heapptr(ctx, i2c->rx_data_ref);

					    duk_int_t rc = duk_pcall(ctx, 1);
					    if (rc != 0) {
					        printf("Callback failed: '%s'\r\n", duk_safe_to_string(ctx, -1));
					    }

				    	duk_pop(ctx);

			    	}

			    	if( i2c->rx_data_ref != NULL){
			    		duk_push_null(ctx);
						duk_put_global_string(ctx,  i2c->rx_name_ref); // allow garbage collect
			    		duk_push_null(ctx);
						duk_put_global_string(ctx,  i2c->rx_data_ref); // allow garbage collect
					}

		    	}

	    	}else{
	    		printf("%s: msg undef\r\n", __func__);
	    	}


		}


	}

}


int worker_task(ULONG arg){

	event_msg_t evt;

	for(;;){

		tx_queue_receive(at_msg_queue, &evt, TX_WAIT_FOREVER);

		if(evt.msg_id == MSG_TYPE_AT_WRITE){

			at_pipe_map_tbl_t *at = (at_pipe_map_tbl_t *)evt.msg;

		   	if(qapi_QT_Apps_Send_AT(at->stream,at->cmd) == QAPI_QT_ERR_OK){

				while(tx_queue_receive(at_data_queue, &evt, 25) == TX_SUCCESS){

					if(evt.msg_id == MSG_TYPE_AT_CB_PIPE){
						const char *str = (const char *)evt.msg;		
						if(str != NULL){								
							int len = strlen(str);		
							at->buffer = realloc(at->buffer, at->buffer_tot_len+len+1);
							void *dst = at->buffer+at->buffer_tot_len;	
							memcpy(dst,str,len);
							at->buffer_tot_len += len;
							at->buffer[at->buffer_tot_len] = '\0';
						}
					}

				}
				
				event_msg_t evt_cb;
    			evt_cb.msg_id = MSG_TYPE_AT_WRITE_CB;
    			evt_cb.msg = (void*)at; // forward to main event mngr
    			tx_queue_send(event_mngr_msg_queue, &evt_cb, TX_NO_WAIT);
    			tx_semaphore_put (evnt_gatekeeper);

			}

		}

	}

}