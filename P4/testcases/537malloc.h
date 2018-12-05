#include "stdinclude.h"
#include "utils.h"
#include "autoarr.h"
#include "memavl.h"

#define   malloc537(sz)     _malloc537((sz), FILE_LINE)
#define     free537(pt)     _free537((pt), FILE_LINE)
#define  realloc537(pt, sz) _realloc537((pt), (sz), FILE_LINE) 
#define memcheck537(pt, sz) _memcheck537((pt), (sz), FILE_LINE)

void *_malloc537(size_t size, char *fl);

void _free537(void *ptr, char *fl);

void *_realloc537(void *ptr, size_t size, char *fl);

void _memcheck537(void *ptr, size_t size, char *fl);

