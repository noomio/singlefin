#include "duktape.h"


duk_ret_t native_print(duk_context *ctx) {
	duk_push_string(ctx, " ");
	duk_insert(ctx, 0);
	duk_join(ctx, duk_get_top(ctx) - 1);
	printf("%s", duk_safe_to_string(ctx, -1));
	return 0;
}