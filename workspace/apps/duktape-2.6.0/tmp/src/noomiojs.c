#include "duktape.h"
#include "duk_module_node.h"
#include "qapi_timer.h"
#include "qapi_device_info.h"
#include "qapi_fs.h"
#include "malloc.h"
#include "event_mngr.h"
#include "debug.h"

const char *LICENSE = "Copyright (c) 2020 Nikolas Karakotas\
\
Permission is hereby granted, free of charge, to any person obtaining a copy\
of this software and associated documentation files (the \"Software\"), to deal\
in the Software without restriction, including without limitation the rights\
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\
copies of the Software, and to permit persons to whom the Software is\
furnished to do so, subject to the following conditions:\
\
The above copyright notice and this permission notice shall be included in all\
copies or substantial portions of the Software.\
\
THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\
SOFTWARE.";

#define  LINEBUF_SIZE       2048

static duk_ret_t wrapped_compile_execute(duk_context *ctx, void *udata) {
	const char *src_data;
	duk_size_t src_len;
	duk_uint_t comp_flags;

	(void) udata;
	bool allow_bytecode = false;

	/* XXX: Here it'd be nice to get some stats for the compilation result
	 * when a suitable command line is given (e.g. code size, constant
	 * count, function count.  These are available internally but not through
	 * the public API.
	 */

	/* Use duk_compile_lstring_filename() variant which avoids interning
	 * the source code.  This only really matters for low memory environments.
	 */

	/* [ ... bytecode_filename src_data src_len filename ] */

	src_data = (const char *) duk_require_pointer(ctx, -3);
	src_len = (duk_size_t) duk_require_uint(ctx, -2);

	if (src_data != NULL && src_len >= 1 && src_data[0] == (char) 0xbf) {
		/* Bytecode. */
		if (allow_bytecode) {
			void *buf;
			buf = duk_push_fixed_buffer(ctx, src_len);
			memcpy(buf, (const void *) src_data, src_len);
			duk_load_function(ctx);
		} else {
			(void) duk_type_error(ctx, "bytecode input rejected (use -b to allow bytecode inputs)");
		}
	} else {
		/* Source code. */
		comp_flags = DUK_COMPILE_SHEBANG;
		duk_compile_lstring_filename(ctx, comp_flags, src_data, src_len);
	}

	/* [ ... bytecode_filename src_data src_len function ] */


#if 0
	/* Manual test for bytecode dump/load cycle: dump and load before
	 * execution.  Enable manually, then run "make ecmatest" for a
	 * reasonably good coverage of different functions and programs.
	 */
	duk_dump_function(ctx);
	duk_load_function(ctx);
#endif

#if defined(DUK_CMDLINE_LOWMEM)
	lowmem_start_exec_timeout();
#endif

	duk_push_global_object(ctx);  /* 'this' binding */
	duk_call_method(ctx, 0);



	/*
	 *  In interactive mode, write to stdout so output won't
	 *  interleave as easily.
	 *
	 *  NOTE: the ToString() coercion may fail in some cases;
	 *  for instance, if you evaluate:
	 *
	 *    ( {valueOf: function() {return {}},
	 *       toString: function() {return {}}});
	 *
	 *  The error is:
	 *
	 *    TypeError: coercion to primitive failed
	 *            duk_api.c:1420
	 *
	 *  These are handled now by the caller which also has stack
	 *  trace printing support.  User code can print out errors
	 *  safely using duk_safe_to_string().
	 */

	//duk_push_global_stash(ctx);
	//duk_get_prop_string(ctx, -1, "dukFormat");
	//duk_dup(ctx, -3);
	//duk_call(ctx, 1);  /* -> [ ... res stash formatted ] */

	//printf("= %s\n", duk_to_string(ctx, -1));

	return 0;  /* duk_safe_call() cleans up */
}


static duk_ret_t cb_resolve_module(duk_context *ctx) {
    /*
     *  Entry stack: [ requested_id parent_id ]
     */

	const char *module_id;
	const char *parent_id;

	module_id = duk_require_string(ctx, 0);
	parent_id = duk_require_string(ctx, 1);

	duk_push_sprintf(ctx, "datatx/%s.js", module_id);
	TX_DEBUGF(TX_DBG,("resolve_cb: id:'%s', parent-id:'%s', resolve-to:'%s'\n",
		module_id, parent_id, duk_get_string(ctx, -1)));

    return 1;  /*nrets*/
}

static duk_ret_t cb_load_module(duk_context *ctx) {
    /*
     *  Entry stack: [ resolved_id exports module ]
     */
	int fd;
	struct qapi_FS_Stat_Type_s finfo;

	const char *filename;
	const char *module_id;

	module_id = duk_require_string(ctx, 0);
	duk_get_prop_string(ctx, 2, "filename");
	filename = duk_require_string(ctx, -1);

	TX_DEBUGF(TX_DBG,("load_cb: id:'%s', filename:'%s'\n", module_id, filename));


	if(qapi_FS_Open ( filename, QAPI_FS_O_RDONLY_E, &fd) == QAPI_OK){

		if(qapi_FS_Stat_With_Handle(fd,&finfo) == QAPI_OK){
			
			void *p_buf = malloc(finfo.st_size);
			
			if(p_buf){	
			
				uint32_t rbytes;				
				if( qapi_FS_Read (fd, p_buf, finfo.st_size, &rbytes) == QAPI_OK){			
					
					duk_push_lstring(ctx, (const char *) p_buf, (duk_size_t) finfo.st_size);

				}
				
				free(p_buf);
			}
			

			
		}
		
		qapi_FS_Close(fd);			
	
	}else
		(void) duk_type_error(ctx, "cannot find module: %s", module_id);


	return 1;
}

int main(void)
{


	int fd;
	struct qapi_FS_Stat_Type_s finfo;


	duk_context *ctx = duk_create_heap_default();
	
	if(ctx){


		/* After initializing the Duktape heap or when creating a new
		 * thread with a new global environment:
		 */
		duk_push_object(ctx);
		duk_push_c_function(ctx, cb_resolve_module, DUK_VARARGS);
		duk_put_prop_string(ctx, -2, "resolve");
		duk_push_c_function(ctx, cb_load_module, DUK_VARARGS);
		duk_put_prop_string(ctx, -2, "load");
		duk_module_node_init(ctx);

		duk_push_c_function(ctx, native_print, DUK_VARARGS /*nargs*/);
		duk_put_global_string(ctx, PRINT_NAME);

		duk_push_c_function(ctx, native_timer_set_timeout, 2 /*nargs*/);
		duk_put_global_string(ctx, TIMER_SET_TIMEOUT_NAME);

		duk_push_c_function(ctx, native_timer_set_interval, 2 /*nargs*/);
		duk_put_global_string(ctx, TIMER_SET_INTERVAL_NAME);

		duk_push_c_function(ctx, native_timer_clear_interval, 1 /*nargs*/);
		duk_put_global_string(ctx, TIMER_CLEAR_INTERVAL_NAME);

		duk_push_c_function(ctx, native_at, 1 /*nargs*/);
    	duk_put_global_string(ctx, AT_NAME);

    	duk_push_c_function(ctx, native_init_at_consts, 0 /*nargs*/);
		duk_pcall(ctx, 0);


		duk_push_c_function(ctx, native_i2c, 1 /*nargs*/);
    	duk_put_global_string(ctx, I2C_NAME);

    	duk_push_c_function(ctx, native_init_i2c_consts, 0 /*nargs*/);
		duk_pcall(ctx, 0);


		duk_push_c_function(ctx, native_digital_in, 2 /*nargs*/);
    	duk_put_global_string(ctx, GPIO_DIGITAL_IN_NAME);

		duk_push_c_function(ctx, native_digital_out, 2 /*nargs*/);
    	duk_put_global_string(ctx, GPIO_DIGITAL_OUT_NAME);

    	duk_push_c_function(ctx, native_digital_interrupt_in, 2 /*nargs*/);
    	duk_put_global_string(ctx, GPIO_DIGITAL_INTERRUPT_IN_NAME);

    	duk_push_c_function(ctx, native_init_gpio_consts, 0 /*nargs*/);
		duk_pcall(ctx, 0);


		/*
		* 	Task
		*/
		txm_module_object_allocate(&event_mngr_task_byte_pool, sizeof(TX_BYTE_POOL));
	 	tx_byte_pool_create(event_mngr_task_byte_pool, "event_mgr_byte_pool", event_mngr_task_mem, EVENT_MNGR_TASK_BYTE_POOL_SIZE);
	 	
	 	tx_byte_allocate(event_mngr_task_byte_pool, (VOID **) &event_mngr_thread_stack, EVENT_MNGR_TASK_STACK_SIZE, TX_NO_WAIT);		
		txm_module_object_allocate(&event_mngr_thread_handle, sizeof(TX_THREAD));
		tx_thread_create(event_mngr_thread_handle,
							"event_mngr",
						   	event_mngr_task,
						   	(ULONG)ctx,
						   	event_mngr_thread_stack,
						   	EVENT_MNGR_TASK_STACK_SIZE,
						   	128,
						   	128,
						   	TX_NO_TIME_SLICE,
						   	TX_AUTO_START
		);


	

		tx_byte_allocate(event_mngr_task_byte_pool, (VOID **) &worker_thread_stack, WORKER_TASK_STACK_SIZE, TX_NO_WAIT);		
		txm_module_object_allocate(&worker_thread_handle, sizeof(TX_THREAD));
		tx_thread_create(worker_thread_handle,
							"worker_task",
						   	worker_task,
						   	(ULONG)ctx,
						   	worker_thread_stack,
						   	WORKER_TASK_STACK_SIZE,
						   	180,
						   	180,
						   	TX_NO_TIME_SLICE,
						   	TX_AUTO_START
		);

		
		#if 0
		//duk_eval_string_noresult(ctx, "test = { name: 'test', read: function(){ return null;} }; print(test.read());");
		//duk_eval_string_noresult(ctx, "function DigitalIn(pin,mode) { return { pin: pin, mode: mode, read: function(){ return null;} } }; var test = DigitalIn(1,0); print(test.read());");
		//duk_eval_string_noresult(ctx, "var din = DigitalIn(PIN4); print(din.read());"); //print(Duktape.enc('jx', din));
    	//duk_eval_string_noresult(ctx, "var sw1;  sw1 = InterruptIn(PIN4); sw1.rise(function(){ print('rise!');  });  sw1.trigger(); ");  
    	//duk_eval_string_noresult(ctx, "var dout = DigitalOut(4,0); dout.write(true); "); //print(Duktape.enc('jx', din));
    	//var dout = DigitalOut(4,0); dout.write(true);
		#endif


		#if 0
		duk_eval_string_noresult(ctx, "\
			var intervalId; \
			const port = at(PIPE0); \
			port.on('open',function(){ \
				print('open'); \
				intervalId = setInterval(function () { \
					port.write('ATE0\\r\\n'); \
				}, 1000); \
			}); \
			port.on('data',function(data){ \
				print(data); \
			}); \
		");  
		 #endif

		#if 0
		duk_eval_string_noresult(ctx, "\
		const port = at(PIPE0); \
		port.on('open',function(){ print('open'); }); \
		port.on('data',function(data){ print(data); }); \
		port.write('ATE0\\r\\n'); \
		port.write('AT+CPIN?\\r\\n'); \
		port.write('AT+CPIN?\\r\\n'); \
		port.write('AT+CPIN?\\r\\n'); \
		port.write('AT+CPIN?\\r\\n'); \
		port.write('AT+CPIN?\\r\\n'); \
		port.write('AT+CPIN?\\r\\n'); \
		port.write('AT+CPIN?\\r\\n'); \
		port.write('AT+CPIN?\\r\\n'); \
		  ");  
		 #endif

		#if 0
		duk_eval_string_noresult(ctx, "\
			var intervalId; \
			var reg = Uint8Array.allocPlain(1); \
			var recvd = Uint8Array.allocPlain(2); \
			reg[0] = 0x06; \
			const sensor = i2c(I2C1); \
			sensor.frequency(150); \
			sensor.on('open',function(){ \
				intervalId = setInterval(function () { \
					sensor.transfer(0x18,reg,recvd); \
				}, 1000); \
				sensor.on('data',function(data){ \
					print(data[0]); \
					print(data[1]); \
				}); \
			}); \
		");
		#endif

		/*
		duk_eval_string_noresult(ctx, "\
			var pig = require('pig'); \
			print(pig.who); \
		");  
		*/


		if(qapi_FS_Open ( "datatx/app.js", QAPI_FS_O_RDONLY_E, &fd) == QAPI_OK){

			if(qapi_FS_Stat_With_Handle(fd,&finfo) == QAPI_OK){
				
				void *p_buf = malloc(finfo.st_size);
				
				if(p_buf){	
				
					uint32_t rbytes;				
					if( qapi_FS_Read (fd, p_buf, finfo.st_size, &rbytes) == QAPI_OK){			
						
						duk_push_lstring(ctx, (const char *) p_buf, (duk_size_t) finfo.st_size);
						/* Run and catch any errors */
						if (duk_peval(ctx) != 0) {
							printf("Error: %s\n", duk_safe_to_string(ctx, -1));
						}

						duk_pop(ctx);  
					}
					
					free(p_buf);
				}
				

				
			}
			
			qapi_FS_Close(fd);			
		
		}
	
	}


	char *buffer = NULL;

	buffer = (char *) malloc(LINEBUF_SIZE);

	for(;;){	

		int retval = 0;
		int rc;
		int got_eof = 0;

		while (!got_eof) {
			size_t idx = 0;

			puts("\n$> ");

			for (;;) {
				int c = getchar();

				if (c == EOF) {
					got_eof = 1;
					break;
				} else if (c == '\n' || c == '\r') {
					break;
				} else if (idx >= LINEBUF_SIZE) {
					//fprintf(stderr, "line too long\n");
					//fflush(stderr);
					//retval = -1;
					//goto done;
				} else {
					buffer[idx++] = (char) c;
				}
			}

			duk_push_pointer(ctx, (void *) buffer);
			duk_push_uint(ctx, (duk_uint_t) idx);
			duk_push_string(ctx, "input");

			rc = duk_safe_call(ctx, wrapped_compile_execute, NULL /*udata*/, 3 /*nargs*/, 1 /*nret*/);


			if (rc != DUK_EXEC_SUCCESS) {
				/* in interactive mode, write to stdout */
				printf("%s\n", duk_safe_to_stacktrace(ctx, -1));
				duk_pop(ctx);
				retval = -1;  /* an error 'taints' the execution */
			} else {
				duk_pop(ctx);
			}
		}

	}



	return 1;

}


