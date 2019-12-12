/* shamem extension for PHP */

#ifndef PHP_SHAMEM_H
# define PHP_SHAMEM_H

/* shamem extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/mman.h>
#include "php.h"
#include "ext/standard/info.h"


extern zend_module_entry shamem_module_entry;
# define phpext_shamem_ptr &shamem_module_entry

# define PHP_SHAMEM_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_SHAMEM)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

/* shamem extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif


#ifndef HAVE_MAP_ANON
#define HAVE_MAP_ANON  1
#endif



#endif	/* PHP_SHAMEM_H */

