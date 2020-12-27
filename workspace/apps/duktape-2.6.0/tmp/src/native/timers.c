#include "qapi_timer.h"
#include "event_mngr.h"

static void invoke_timer_cb(ULONG arg);
//__func__

/*
	One shot timer
*/
duk_ret_t native_timer_set_timeout(duk_context *ctx) {
    long timeout;

    duk_require_function(ctx, 0);
    timeout = (long) duk_require_uint(ctx, 1);

	timer_store_msg_t *t = NULL;

	if(tx_block_allocate(timer_pool, (VOID **)&t,TX_NO_WAIT) == TX_SUCCESS){
		if(txm_module_object_allocate(&t->timer, sizeof(TX_TIMER)) == TX_SUCCESS){
			snprintf(t->name,TIMERS_NAME_LEN,"_tmr%08X",(uint32_t)t->timer);
			t->oneshot = true;
		    if(tx_timer_create(t->timer,t->name,invoke_timer_cb, (uint32_t)t, (timeout*TX_TIMER_TICKS_PER_SECOND)/1000, 0,TX_AUTO_ACTIVATE) == TX_SUCCESS){
		    	duk_dup(ctx, 0);
		    	duk_put_global_string(ctx, t->name);
			}
		}else{
			tx_block_release((VOID **)&t);
		}
	}

    return 0; /* no return value (= undefined) */
}

duk_ret_t native_timer_set_interval(duk_context *ctx) {
    long timeout;

    duk_require_function(ctx, 0);
    timeout = (long) duk_require_uint(ctx, 1);

	timer_store_msg_t *t;
	

	if(tx_block_allocate(timer_pool, (VOID **)&t,TX_NO_WAIT) == TX_SUCCESS){

		if(txm_module_object_allocate(&t->timer, sizeof(TX_TIMER))  == TX_SUCCESS){

			snprintf(t->name,TIMERS_NAME_LEN,"_tmr%08X",(uint32_t)t->timer);
			t->oneshot = false;

		    if(tx_timer_create(t->timer,t->name,invoke_timer_cb, (uint32_t)t, 100, (timeout*TX_TIMER_TICKS_PER_SECOND)/1000,TX_AUTO_ACTIVATE) == TX_SUCCESS){
		    	duk_dup(ctx, 0);
		    	duk_put_global_string(ctx, t->name);
		    	duk_push_number(ctx, (uint32_t) t);
		    	return 1; /* one return value */;
		    }		    	
		}else{
			tx_block_release((VOID **)&t);
		}
	}

	return 0; /* no return value (= undefined) */

}

duk_ret_t native_timer_clear_interval(duk_context *ctx) {
 
    uint32_t id = (uint32_t) duk_require_uint(ctx, 0);

	timer_store_msg_t *t = (timer_store_msg_t *)id;

	if(t != NULL){
		tx_timer_deactivate(t->timer);

		duk_push_null(ctx);
    	duk_put_global_string(ctx, t->name);
		duk_pop(ctx);

    	/* free timer */
    	tx_timer_delete(t->timer);
   		txm_module_object_deallocate(&t->timer);
    	tx_block_release(t);

	}

    return 0;
}


static void invoke_timer_cb(ULONG arg) {
	event_msg_t evt;
	evt.msg_id = MSG_TYPE_TIMER;
	evt.msg = (void*)arg;
	tx_queue_send(event_mngr_msg_queue, &evt, TX_NO_WAIT);
	tx_semaphore_put (evnt_gatekeeper);
}


