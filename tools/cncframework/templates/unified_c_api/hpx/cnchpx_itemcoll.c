{% import "common_macros.inc.c" as util with context -%}
{{ util.auto_file_banner() }}

#include "cnchpx.h"

/***********************************
 *           HPX Tasks             *
 ***********************************/

// Utility functions
hpx_addr_t item_at(hpx_addr_t start, int index) {
  return hpx_addr_add(start, sizeof(cncItemFuture) * index, 
      sizeof(cncItemFuture)); 
}

// Handler prototypes 
int item_init_handler(cncItemFuture* item, int future_size);

// HPX actions
HPX_ACTION(HPX_DEFAULT, HPX_PINNED, item_init, item_init_handler, HPX_POINTER, HPX_INT);

// Handler implementations
int item_init_handler(cncItemFuture* item, int future_size) {
  item->future = hpx_lco_future_new(future_size);
  return HPX_SUCCESS;
}

/***********************************************
 *    CNC item collection implementations      *
 **********************************************/

void *cncItemAlloc(size_t bytes) {
  return malloc(bytes);
}

void cncItemFree(void *item) {
  free(item);
}

cncItemCollection_t _cncItemCollectionCreate(long arr_size, int item_size, 
    int future_size) {

    hpx_addr_t addr = hpx_gas_calloc_cyclic(arr_size, item_size, 0);
    assert(addr != HPX_NULL);

    // Initialize each item
    hpx_addr_t done = hpx_lco_and_new(arr_size); 
    int i;
    for (i = 0; i < arr_size; i++) {
       hpx_addr_t item = item_at(addr, i); 
       hpx_call(item, item_init, done, &future_size); 
    }
   
    hpx_lco_wait(done); 
    hpx_lco_delete(done, HPX_NULL);

    return addr;
}

void _cncItemCollectionDestroy(cncItemCollection_t coll) {
    hpx_gas_free(coll, HPX_NULL);
}

cncItemCollection_t _cncItemCollectionSingletonCreate(int item_size,
    int future_size) {
    hpx_addr_t addr = hpx_gas_calloc_cyclic(1, item_size, 0);
    assert(addr != HPX_NULL);

    hpx_call_sync(addr, item_init, NULL, 0, &future_size);

    return addr;
}

void _cncItemCollectionSingletonDestroy(cncItemCollection_t coll) {
    hpx_gas_free(coll, HPX_NULL);
}
