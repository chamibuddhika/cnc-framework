{% import "common_macros.inc.c" as util with context -%}
{{ util.auto_file_banner() }}

{% set defname = "_CNCHPX_H_" -%}
#ifndef {{defname}}
#define {{defname}}

#include "hpx.h"
#include <stdio.h>
#include <stdblib.h>

#define DEFAULT_NUM_BUCKETS 1024
#define DEFAULT_BUCKET_SIZE 128

#define DEFAULT_ARRAY_SIZE 128

#define CNC_SHUTDOWN_ON_FINISH(ctx) {{g.name}}_destroy(ctx) 

/********************************\
******** CNC TYPE ALIASES ********
\********************************/

typedef cncItemCollection_t hpx_addr_t; // Item collection address

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

inline hpx_type_t get_hpx_type(char* type_name) {
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
cncItemCollection_t _cncItemCollectionCreate(void);
void _cncItemCollectionDestroy(cncItemCollection_t coll);

cncItemCollection_t _cncItemCollectionSingletonCreate(void);
void _cncItemCollectionSingletonDestroy(cncItemCollection_t coll);


#endif /*{{defname}}*/
