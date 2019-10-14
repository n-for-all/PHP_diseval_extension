/*
 * phpize
 * ./configure
 * make
 * php -dextension=modules/diseval.so -r "diseval_info();";
 **/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "diseval.h"
#include "zend_compile.h"
#include "ext/standard/info.h"

#ifdef COMPILE_DL_DISEVAL
ZEND_GET_MODULE(diseval)
#endif

void (*zend_execute_old)(zend_execute_data *execute_data TSRMLS_DC);

zend_function_entry diseval_functions[] = {
	PHP_FE(diseval_info,   0)
	PHP_FE_END
};

zend_module_entry diseval_module_entry = {
	STANDARD_MODULE_HEADER,
	"diseval",
	diseval_functions,
	PHP_MINIT(diseval),
	PHP_MSHUTDOWN(diseval),
	PHP_RINIT(diseval),
	PHP_RSHUTDOWN(diseval),
	PHP_MINFO(diseval),
	"0.1",
	STANDARD_MODULE_PROPERTIES
};

PHP_INI_BEGIN()
STD_ZEND_INI_BOOLEAN("diseval.enable", "On", PHP_INI_ALL, OnUpdateBool, enable, zend_diseval_globals, diseval_globals)
PHP_INI_END()

static void php_diseval_init_globals(zend_diseval_globals *diseval_globals)
{
    diseval_globals->enable = 1;
}

PHP_MINIT_FUNCTION(diseval)
{
	zend_execute_old = zend_execute_ex;
	zend_execute_ex = diseval_execute_ex;
	ZEND_INIT_MODULE_GLOBALS(swoole, php_diseval_init_globals, NULL);
	return SUCCESS;
}

PHP_MINFO_FUNCTION(diseval)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Diseval support", diseval_globals->enable);
	php_info_print_table_end();
}

PHP_MSHUTDOWN_FUNCTION(diseval)
{
	zend_execute_ex = zend_execute_old;
	return SUCCESS;
}

PHP_RINIT_FUNCTION(diseval)
{
	return SUCCESS;
}
PHP_RSHUTDOWN_FUNCTION(diseval)
{
	return SUCCESS;
}

PHP_FUNCTION(diseval_info)
{
	php_printf("DISEVAL Version 0.1\n");
	RETURN_FALSE;
}

void diseval_execute_ex(zend_execute_data *execute_data TSRMLS_DC)
{
#if PHP_VERSION_ID >= 70000
	const zend_op_array *op_array = &execute_data->func->op_array;
#else
	const zend_op_array *op_array = execute_data->op_array;
#endif
	if (op_array->type == ZEND_EVAL_CODE && diseval_globals->enable == 1) {
		zend_error(E_ERROR, "DISEVAL - Use of eval is forbidden");
		zend_bailout();
	}
	zend_execute_old(execute_data TSRMLS_CC);
}
