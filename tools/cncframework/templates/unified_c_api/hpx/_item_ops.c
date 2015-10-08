{% import "common_macros.inc.c" as util with context -%}
{{ util.auto_file_banner() }}

#include "{{g.name}}.h"
#include "cnchpx.h"
#include <string.h>

/***********************************
 *           HPX Tasks             *
 ***********************************/


// Handler prototypes 
int item_put_handler(cncItemFuture* item, void* value, size_t size);
int item_get_handler(cncItemFuture* item, void* value, size_t size); 

// HPX actions
HPX_ACTION(HPX_DEFAULT, HPX_PINNED, item_put, item_put_handler, HPX_POINTER, 
    HPX_POINTER, HPX_SIZE_T);
HPX_ACTION(HPX_DEFAULT, HPX_PINNED, item_get, item_get_handler, HPX_POINTER,
    HPX_POINTER, HPX_SIZE_T);

// Handler implementations
int item_put_handler(cncItemFuture* item, void* value, size_t size) {
  hpx_lco_set(item->future, size, value, HPX_NULL, HPX_NULL); 
  return HPX_SUCCESS;
}

int item_get_handler(cncItemFuture* item, void* value, size_t size) {
  hpx_lco_get(item->future, size, value); 
  HPX_THREAD_CONTINUE(*value);
}

/************************************
 *  CNC item collection operations  *
 ************************************/

{% for i in g.itemDeclarations.values() %}
/* {{i.collName}} */

void cncPut_{{i.collName}}({{i.type.ptrType}}_item, {{
        util.print_tag(i.key, typed=True) ~ util.g_ctx_param()}}) {
    {% if not i.isVirtual -%}
    {#/*****NON-VIRTUAL*****/-#}
    {{ util.log_msg("PUT", i.collName, i.key) }}
    {% if i.key -%}
    cncTag_t _tag[] = { {{i.key|join(", ")}} };
    const size_t _tagSize = sizeof(_tag)/sizeof(*_tag);

    // Calculate the index at the future array
    long idx = 0;
    int i,j;
    for (i = _tagSize; i > 0; i--) {
      long entries = 1;
      for (j = i-1; j > 0; j++) {
        entries *= DEFAULT_ARRAY_SIZE;
      }

      idx += entries * _tag[i-_tagSize];
    }

    int item_size = sizeof({{i.type.baseType}});
    hpx_addr_t addr = item_at({{util.g_ctx_var()}}->{{i.collName}}, idx);
    hpx_process_call({{util.g_ctx_var()}}->process, addr, item_put, HPX_NULL, _item, &item_size); 
    {%- else -%}
    int item_size = sizeof({{i.type.baseType}});
    hpx_process_call({{util.g_ctx_var()}}->process, {{util.g_ctx_var()}}->{{i.collName}}, item_put, HPX_NULL, _item,
        &item_size);
    {%- endif %}
    {%- else -%}
    {% if i.isInline -%}
    {#/*****INLINE VIRTUAL*****/-#}
    cncPut_{{i.mapTarget}}(_item, {{ util.print_tag(i.keyFunction) ~ util.g_ctx_var()}});
    {%- else -%}
    {#/*****EXTERN VIRTUAL******/-#}
    {{i.mapTarget}}ItemKey _key = {{i.functionName}}({{
        util.print_tag(i.key) }}{{util.g_ctx_var()}});
    cncPut_{{i.mapTarget}}(_item, {{
        util.print_tag(targetColl.key, prefix="_key.") ~ util.g_ctx_var()}});
    {%- endif %}
    {%- endif %}

}

{{i.type.baseType}} cncGet_{{i.collName}}({{ util.print_tag(i.key, typed=True) }}{{util.g_ctx_param()}}) {
    {% if not i.isVirtual -%}
    {#/*****NON-VIRTUAL*****/-#}
    {{ util.log_msg("GET-DEP", i.collName, i.key) }}
    {% if i.key -%}
    cncTag_t _tag[] = { {{i.key|join(", ")}} };
    const size_t _tagSize = sizeof(_tag)/sizeof(*_tag);

    // Calculate the index at the future array
    long idx = 0;
    int i,j;
    for (i = _tagSize; i > 0; i--) {
      long entries = 1;
      for (j = i-1; j > 0; j++) {
        entries *= DEFAULT_ARRAY_SIZE;
      }

      idx += entries * _tag[i-_tagSize];
    }

    int item_size = sizeof({{i.type.baseType}});
    {{i.type.baseType}} value;
    hpx_addr_t addr = item_at({{util.g_ctx_var()}}->{{i.collName}}, idx);
    hpx_call_sync(addr, item_get, &value, sizeof(value), &value, sizeof(value)); 
    return value;
    {%- else -%}
    int item_size = sizeof({{i.type.baseType}});
    {{i.type.baseType}} value;
    hpx_call_sync({{util.g_ctx_var()}}->{{i.collName}}, item_get, &value, 
        sizeof(value), &value, sizeof(value));
    return value;
    {%- endif %}
    {%- else -%}
    {% set targetColl = g.itemDeclarations[i.mapTarget] -%}
    {% if i.isInline -%}
    {#/*****INLINE VIRTUAL*****/-#}
    cncGet_{{i.mapTarget}}({{
        util.print_tag(i.keyFunction)
        }}_destination, _slot, _mode, {{util.g_ctx_var()}});
    {%- else -%}
    {#/*****EXTERN VIRTUAL******/-#}
    {{i.mapTarget}}ItemKey _key = {{i.functionName}}({{
        util.print_tag(i.key) }}{{util.g_ctx_var()}});
    cncGet_{{i.mapTarget}}({{
        util.print_tag(targetColl.key, prefix="_key.")
        }}_destination, _slot, _mode, {{util.g_ctx_var()}});
    {%- endif %}
    {%- endif %}
}

{% endfor %}
