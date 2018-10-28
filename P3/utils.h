#include "stdinclude.h"

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

