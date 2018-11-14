/* author1: Dasong Gao
 * author2: Haozhe Luo
 */

#include "stdinclude.h"
#include <string.h>

#pragma once

typedef struct __AUTOSIZINGARR {
	void *arr;
	int size;
	int used;
} ASArr;

// the initial size of the auto-growing array
extern const int DEFAULT_ARR_SIZE;

// frees the struct and returns the internal
void *ASARR_strip(ASArr *asarr);

// an auto-growing array
#define ASARR_INIT(name, type) \
	ASARR_DEFTYPE(name, type);\
	ASArr *name = PEONZ(malloc, sizeof(ASArr));\
	name->size = DEFAULT_ARR_SIZE;\
	name->arr  = PEONZ(malloc, DEFAULT_ARR_SIZE * sizeof(ASARR_##name##_type));\
	name->used = 0;

#define ASARR_INSERT(name, val) {\
	ASARR_##name##_type check = val;\
	((ASARR_##name##_type *)name->arr)[name->used++] = check;\
	((ASARR_##name##_type *)name->arr)[name->used] = 0;\
	if (name->used + 1 == name->size) {\
		void *ASARR_##name##_arr_old = name->arr;\
		int new_size = 2 * name->size;\
		name->arr = PEONZ(malloc, new_size * sizeof(ASARR_##name##_type));\
		memcpy(name->arr, ASARR_##name##_arr_old,\
				name->size * sizeof(ASARR_##name##_type));\
		name->size = new_size;\
		free(ASARR_##name##_arr_old);\
	}\
}

#define ASARR_GET(name) \
	((ASARR_##name##_type *) name->arr)

#define ASARR_STRIP(name) \
	((ASARR_##name##_type *) ASARR_strip(name))

#define ASARR_SIZE(name) \
	(name->used)

#define ASARR_RENEW(name) \
	if (name->used != 0) {\
		name->size = DEFAULT_ARR_SIZE;\
		name->arr  = PEONZ(malloc, name->size * sizeof(ASARR_##name##_type));\
		name->used = 0;\
		((ASARR_##name##_type *)name->arr)[name->used] = (ASARR_##name##_type) 0;\
	}

#define ASARR_DEFTYPE(name, type) \
	typedef type ASARR_##name##_type;\

#define ASARR_DESTROY(name) {\
	free(name->arr);\
	free(name);\
}

