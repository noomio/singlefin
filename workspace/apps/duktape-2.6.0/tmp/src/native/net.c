/*
 * sockets.c
 *
 *  Created on: 15 Dec.,2019
 *      Author: Nikolas Karakotas
 */

#include "txm_module.h"
#include "duktape.h"
#include "event_mngr.h"


static duk_ret_t connect(duk_context *ctx) {

}

static duk_ret_t write(duk_context *ctx) {

}

static duk_ret_t end(duk_context *ctx) {

}

static duk_ret_t destroy(duk_context *ctx) {

}

static duk_ret_t setTimeout(duk_context *ctx) {

}

duk_ret_t socket(duk_context *ctx){

	duk_idx_t obj_idx = duk_push_object(ctx);   /* -> [ ... global obj ] */

	//fd <number> If specified, wrap around an existing socket with the given file descriptor, otherwise a new socket will be created.
	duk_push_number(ctx, -1);
    duk_put_prop_string(ctx, obj_idx, "fd");

	//allowHalfOpen <boolean> Indicates whether half-opened TCP connections are allowed. See net.createServer() and the 'end' event for details. Default: false.
	duk_push_boolean(ctx, false);
    duk_put_prop_string(ctx, obj_idx, "allowHalfOpen");

	//readable  <boolean> Allow reads on the socket when an fd is passed, otherwise ignored. Default: false.
	duk_push_boolean(ctx, false);
    duk_put_prop_string(ctx, obj_idx, "readable");
	
	//writable  <boolean> Allow writes on the socket when an fd is passed, otherwise ignored. Default: false.
	duk_push_boolean(ctx, false);
    duk_put_prop_string(ctx, obj_idx, "writable");

	// net.connect(port[, host][, connectListener])
	duk_push_c_function(ctx, connect, 3  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "connect");

    //socket.write(data[, encoding][, callback])
    //data <string> | <Buffer> | <Uint8Array>
	//encoding <string> Only used when data is string. Default: utf8.
	//callback <Function>
	//Returns: <boolean>
	duk_push_c_function(ctx, write, 3  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "write");

	//socket.end([, callback])
	duk_push_c_function(ctx, end, 1  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "end");

	//socket.destroy()#
	//Added in: v0.1.90
	//exception <Object>
	//Returns: <net.Socket>
	duk_push_c_function(ctx, destroy, 1  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "destroy");

	//socket.setTimeout(timeout[, callback])[src]#
	//Added in: v0.1.90
	//timeout <number>
	//callback <Function>
	//Returns: <net.Socket> The socket itself.
	duk_push_c_function(ctx, setTimeout, 2  /*nargs*/);
    duk_put_prop_string(ctx, obj_idx, "setTimeout");

#if 1
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