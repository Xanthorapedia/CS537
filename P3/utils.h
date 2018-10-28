#include "stdinclude.h"
#include <string.h>

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)

// error message on failed function call
#define CALL_FAIL_MSG(fn) "A call to "#fn"() failed at "__FILE__":"STRINGIZE(__LINE__)"\n"

// Print Error ON Non-zero return values
#define PEONN(fn,...) perr_neq(0, CALL_FAIL_MSG(fn), (void *) (long) fn(__VA_ARGS__))

// Print Error ON Zero return values
#define PEONZ(fn,...) perr_neq(1, CALL_FAIL_MSG(fn), (void *) (long) fn(__VA_ARGS__))

// prints error message if cond is zero but ret is non zero and vice versa
void *perr_neq(int cond, char *msg, void *ret);

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

