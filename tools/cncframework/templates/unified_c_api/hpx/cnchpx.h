{% import "common_macros.inc.c" as util with context -%}
{{ util.auto_file_banner() }}

{% set defname = "_CNCHPX_H_" -%}
#ifndef {{defname}}
#define {{defname}}

#include "{{cncRuntimeName}}.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_NUM_BUCKETS 1024
#define DEFAULT_BUCKET_SIZE 128

#define DEFAULT_ARRAY_SIZE 512 

/********************************\
******** CNC TYPE ALIASES ********
\********************************/

typedef hpx_addr_t cncItemCollection_t; // Item collection address

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;

typedef s64 cncTag_t; // tag components

/********************************\
******** CNC HPX TYPES **********
\********************************/

typedef struct cncItemFuture {
  hpx_addr_t future;
} cncItemFuture;

/*
   typedef struct cncItemFuture {
   cncItemFuture* next; 
   hpx_addr_t future;
   }

   typedef struct cncHashBucket {
   hpx_addr_t sema;
   cncItemFuture* entries;
   } cncHashBucket;
   */

/****************************************\
******** Functions and defintions *******
\****************************************/

static inline hpx_type_t get_hpx_type(char* type_name) {
  if (!strcmp(type_name, "u64")) {
    return HPX_UINT64;
  } else if (!strcmp(type_name, "u32")) {
    return HPX_UINT32;
  } else if (!strcmp(type_name, "u16")) {
    return HPX_UINT16;
  } else if (!strcmp(type_name, "u8")) {
    return HPX_UINT8;
  } else if (!strcmp(type_name, "s64")) {
    return HPX_SINT64;
  } else if (!strcmp(type_name, "s32")) {
    return HPX_SINT32;
  } else if (!strcmp(type_name, "s16")) {
    return HPX_SINT16;
  } else if (!strcmp(type_name, "s8")) {
    return HPX_SINT8;
  } else if (!strcmp(type_name, "char")) {
    return HPX_CHAR;
  } else if (!strcmp(type_name, "unsigned char")) {
    return HPX_UCHAR;
  } else if (!strcmp(type_name, "short")) {
    return HPX_SHORT;
  } else if (!strcmp(type_name, "unsigned short")) {
    return HPX_USHORT;
  } else if (!strcmp(type_name, "int")) {
    return HPX_INT;
  } else if (!strcmp(type_name, "unsigned int")) {
    return HPX_UINT;
  } else if (!strcmp(type_name, "long")) {
    return HPX_LONG;
  } else if (!strcmp(type_name, "unsigned long")) {
    return HPX_ULONG;
  } else if (!strcmp(type_name, "float")) {
    return HPX_FLOAT;
  } else if (!strcmp(type_name, "double")) {
    return HPX_DOUBLE;
  } else if (!strcmp(type_name, "cncTag_t")) {
    return HPX_UINT64;
  } else { // TODO: Catch all is HPX_POINTER. Revisit this for typedefs
    return HPX_POINTER;
  }
}

// This is for allocating items at current locality
void *cncItemAlloc(size_t bytes);
void cncItemFree(void *item);

hpx_addr_t item_at(hpx_addr_t start, int index); 

// This is for allocating global item collections at AGAS
cncItemCollection_t _cncItemCollectionCreate(long arr_size, int item_size, 
    int future_size);
void _cncItemCollectionDestroy(cncItemCollection_t coll);

cncItemCollection_t _cncItemCollectionSingletonCreate(int item_size,
    int future_size);
void _cncItemCollectionSingletonDestroy(cncItemCollection_t coll);

/*********************************\
******** CNC HELPER MACROS ********
\*********************************/

/* warning for variadic macro support */
#if __GNUC__ < 3 && !defined(__clang__) && __STDC_VERSION__ < 199901L && !defined(NO_VARIADIC_MACROS)
#warning Your compiler might not support variadic macros, in which case the CNC_REQUIRE macro is not supported. You can disable this warning by setting NO_VARIADIC_MACROS to 0, or disable the macro definitions by setting it to 1.
#endif

#define CNC_ABORT(err) exit(err)

#define CNC_SHUTDOWN_ON_FINISH(ctx) 

#if !NO_VARIADIC_MACROS
#define CNC_REQUIRE(cond, ...) do { if (!(cond)) { printf(__VA_ARGS__); CNC_ABORT(1); } } while (0)
#endif

/* squelch "unused variable" warnings */
#define MAYBE_UNUSED(x) ((void)x)

#endif /*{{defname}}*/
