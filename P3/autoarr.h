#include "stdinclude.h"
#include <string.h>

// the initial size of the auto-growing array
extern const size_t DEFAULT_ARR_SIZE;

// an auto-growing array
#define ASARR_INIT(name, type) \
	typedef type AUTOSIZE_ARR_##name##_type;\
	size_t AUTOSIZE_ARR_##name##_size = DEFAULT_ARR_SIZE;\
	type *AUTOSIZE_ARR_##name##_arr = PEONZ(malloc,\
			AUTOSIZE_ARR_##name##_size * sizeof(AUTOSIZE_ARR_##name##_type));\
	size_t AUTOSIZE_ARR_##name##_used = 0;

#define ASARR_INSERT(name, val) \
	AUTOSIZE_ARR_##name##_arr[AUTOSIZE_ARR_##name##_used++] = val;\
	if (AUTOSIZE_ARR_##name##_used == AUTOSIZE_ARR_##name##_size) {\
		AUTOSIZE_ARR_##name##_type *AUTOSIZE_ARR_##name##_arr_old = AUTOSIZE_ARR_##name##_arr;\
		AUTOSIZE_ARR_##name##_size *= 2;\
		AUTOSIZE_ARR_##name##_arr = PEONZ(malloc,\
				AUTOSIZE_ARR_##name##_size * sizeof(AUTOSIZE_ARR_##name##_type));\
		memcpy(AUTOSIZE_ARR_##name##_arr, AUTOSIZE_ARR_##name##_arr_old,\
				AUTOSIZE_ARR_##name##_size / 2 * sizeof(AUTOSIZE_ARR_##name##_type));\
		free(AUTOSIZE_ARR_##name##_arr_old);\
	}

#define ASARR_GET(name) \
	AUTOSIZE_ARR_##name##_arr;

#define ASARR_SIZE(name) \
	AUTOSIZE_ARR_##name##_used;

#define ASARR_RENEW(name) \
	if (AUTOSIZE_ARR_##name##_used != 0) {\
		AUTOSIZE_ARR_##name##_size = DEFAULT_ARR_SIZE;\
		AUTOSIZE_ARR_##name##_arr = PEONZ(malloc,\
				AUTOSIZE_ARR_##name##_size * sizeof(AUTOSIZE_ARR_##name##_type));\
		AUTOSIZE_ARR_##name##_used = 0;\
	}

#define ASARR_DESTROY(name) \
	free(AUTOSIZE_ARR_##name##_arr);\
	AUTOSIZE_ARR_##name##_arr = NULL;\
	AUTOSIZE_ARR_##name##_size = 0;\
	AUTOSIZE_ARR_##name##_used = 0;

