#include "sapi/embed/php_embed.h"
#include <emscripten.h>
#include <stdlib.h>

#include "zend_globals_macros.h"
#include "zend_exceptions.h"
#include "zend_closures.h"

int main() { return 0; }

void phpw_flush()
{
	fflush(stdout);
	fprintf(stdout, "\n");

	fflush(stderr);
	fprintf(stderr, "\n");
}

char *EMSCRIPTEN_KEEPALIVE phpw_exec(char *code)
{
	putenv("USE_ZEND_ALLOC=0");
	char *retVal = NULL;

	zend_try
	{
		zval retZv;

		zend_eval_string(code, &retZv, "php-wasm evaluate expression");

		convert_to_string(&retZv);

		retVal = Z_STRVAL(retZv);
	}
	zend_catch
	{
	}

	zend_end_try();

	// phpw_flush();

	return retVal;
}

void phpw(char *file)
{
	php_embed_init(0, NULL);

	zend_first_try {
		zend_file_handle file_handle;
		zend_stream_init_filename(&file_handle, file);

		if (php_execute_script(&file_handle) == FAILURE) {
			php_printf("Failed to execute PHP script.\n");
		}
		zend_destroy_file_handle(&file_handle);
	} zend_catch {
    /* int exit_status = EG(exit_status); */
	} zend_end_try();

	phpw_flush();
	php_embed_shutdown();
}
