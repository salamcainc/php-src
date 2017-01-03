/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   |         Xinchen Hui <laruence@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_SMART_STRING_H
#define PHP_SMART_STRING_H

#include "zend_smart_string_public.h"

#include <stdlib.h>
#ifndef SMART_STR_USE_REALLOC
#include <zend.h>
#endif

#ifndef SMART_STRING_PREALLOC
#define SMART_STRING_PREALLOC 128
#endif

#ifndef SMART_STRING_START_SIZE
#define SMART_STRING_START_SIZE 78
#endif

#ifdef SMART_STRING_USE_REALLOC
#define SMART_STRING_REALLOC(a,b,c) realloc((a),(b))
#else
#define SMART_STRING_REALLOC(a,b,c) perealloc((a),(b),(c))
#endif

#define SMART_STRING_DO_REALLOC(d, what) \
	(d)->c = SMART_STRING_REALLOC((d)->c, (d)->a + 1, (what))

/* wrapper */

#define smart_string_appends_ex(str, src, what) \
	smart_string_appendl_ex((str), (src), strlen(src), (what))
#define smart_string_appends(str, src) \
	smart_string_appendl((str), (src), strlen(src))
#define smart_string_append_ex(str, src, what) \
	smart_string_appendl_ex((str), ((smart_string *)(src))->c, \
		((smart_string *)(src))->len, (what));
#define smart_string_sets(str, src) \
	smart_string_setl((str), (src), strlen(src));

#define smart_string_appendc(str, c) \
	smart_string_appendc_ex((str), (c), 0)
#define smart_string_free(s) \
	smart_string_free_ex((s), 0)
#define smart_string_appendl(str, src, len) \
	smart_string_appendl_ex((str), (src), (len), 0)
#define smart_string_append(str, src) \
	smart_string_append_ex((str), (src), 0)
#define smart_string_append_long(str, val) \
	smart_string_append_long_ex((str), (val), 0)
#define smart_string_append_unsigned(str, val) \
	smart_string_append_unsigned_ex((str), (val), 0)

static zend_always_inline size_t smart_string_alloc(smart_string *str, size_t len, zend_bool persistent) {
	if (!str->c) {
		str->len = 0;
		str->a = len < SMART_STRING_START_SIZE
				? SMART_STRING_START_SIZE
				: len + SMART_STRING_PREALLOC;
		SMART_STRING_DO_REALLOC(str, persistent);
		return len;
	} else {
		if (UNEXPECTED((size_t) len > SIZE_MAX - str->len)) {
			zend_error(E_ERROR, "String size overflow");
		}
		len += str->len;
		if (UNEXPECTED(len >= str->a)) {
			str->a = len + SMART_STRING_PREALLOC;
			SMART_STRING_DO_REALLOC(str, persistent);
		}
	}
	return len;
}

static zend_always_inline void smart_string_free_ex(smart_string *str, zend_bool persistent) {
	if (str->c) {
		pefree(str->c, persistent);
		str->c = NULL;
	}
	str->a = str->len = 0;
}

static zend_always_inline void smart_string_0(smart_string *str) {
	if (str->c) {
		str->c[str->len] = '\0';
	}
}

static zend_always_inline void smart_string_appendc_ex(smart_string *dest, char ch, zend_bool persistent) {
	dest->len = smart_string_alloc(dest, 1, persistent);
	dest->c[dest->len - 1] = ch;
}

static zend_always_inline void smart_string_appendl_ex(smart_string *dest, const char *str, size_t len, zend_bool persistent) {
	size_t new_len = smart_string_alloc(dest, len, persistent);
	memcpy(dest->c + dest->len, str, len);
	dest->len = new_len;

}

static zend_always_inline void smart_string_append_long_ex(smart_string *dest, zend_long num, zend_bool persistent) {
	char buf[32];
	char *result = zend_print_long_to_buf(buf + sizeof(buf) - 1, num);
	smart_string_appendl_ex(dest, result, buf + sizeof(buf) - 1 - result, persistent);
}

static zend_always_inline void smart_string_append_unsigned_ex(smart_string *dest, zend_ulong num, zend_bool persistent) {
	char buf[32];
	char *result = zend_print_ulong_to_buf(buf + sizeof(buf) - 1, num);
	smart_string_appendl_ex(dest, result, buf + sizeof(buf) - 1 - result, persistent);
}

static zend_always_inline void smart_string_setl(smart_string *dest, char *src, size_t len) {
	dest->len = len;
	dest->a = len + 1;
	dest->c = src;
}

#endif
