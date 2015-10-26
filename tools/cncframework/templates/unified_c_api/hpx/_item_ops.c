{% import "common_macros.inc.c" as util with context -%}
{{ util.auto_file_banner() }}

#include "{{g.name}}_internal.h"
#include "cnchpx.h"
#include <string.h>

/************************************
 *  CNC item collection operations  *
 ************************************/

{% for i in g.itemDeclarations.values() %}
/* {{i.collName}} related operations */

/***********************************
 *           HPX Tasks             *
 ***********************************/


// Handler prototypes 
int {{i.collName}}_item_put_handler(cncItemFuture* item, void* value, size_t size);
int {{i.collName}}_item_get_handler(cncItemFuture* item); 

// HPX actions
HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED|HPX_PINNED, {{i.collName}}_item_put, 
    {{i.collName}}_item_put_handler, HPX_POINTER, 
    HPX_POINTER, HPX_SIZE_T);
HPX_ACTION(HPX_DEFAULT, HPX_PINNED, {{i.collName}}_item_get, 
    {{i.collName}}_item_get_handler, HPX_POINTER);

// Handler implementations
int {{i.collName}}_item_put_handler(cncItemFuture* item, void* val, size_t size) {
  {{i.type.baseType}}* value = ({{i.type.baseType}}*) val;
  size_t item_size = sizeof({{i.type.baseType}});
  hpx_lco_set(item->future, item_size, value, HPX_NULL, HPX_NULL); 
  return HPX_SUCCESS;
}

int {{i.collName}}_item_get_handler(cncItemFuture* item) {
  {{i.type.baseType}} value;
  size_t item_size = sizeof({{i.type.baseType}});
  hpx_lco_get(item->future, item_size, &value); 
  HPX_THREAD_CONTINUE(value);
}

/************************************
 *    Item collection operations    *
 ***********************************/

void cncPut_{{i.collName}}({{i.type.ptrType}}_item, {{
        util.print_tag(i.key, typed=True) ~ util.g_ctx_param()}}) {
    {% if not i.isVirtual -%}
    {#/*****NON-VIRTUAL*****/-#}
    {{ util.log_msg("PUT", i.collName, i.key) }}
    {% if i.key -%}
    cncTag_t _tag[] = { {{i.key|join(", ")}} };
    const size_t _tagSize = sizeof(_tag)/sizeof(*_tag);

    // Calculate the index at the future array
    long __idx = 0;
    int __i,__j;
    for (__i = _tagSize; __i > 0; __i--) {
      long entries = 1;
      for (__j = __i-1; __j > 0; __j--) {
        entries *= DEFAULT_ARRAY_SIZE;
      }

      __idx += entries * _tag[_tagSize-__i];
    }

    assert (__idx < {{util.g_ctx_var()}}->{{i.collName}}_size);

    int __item_size = sizeof({{i.type.baseType}});
    hpx_addr_t addr = item_at({{util.g_ctx_var()}}->{{i.collName}}, __idx);
    {% if i.type.isPtrType %}
    hpx_process_call({{util.g_ctx_var()}}->process, addr, {{i.collName}}_item_put, HPX_NULL, _item, __item_size); 
    {% else %}
    hpx_process_call({{util.g_ctx_var()}}->process, addr, {{i.collName}}_item_put, HPX_NULL, &_item, __item_size); 
    {% endif %}
    {%- else -%}
    int __item_size = sizeof({{i.type.baseType}});
    hpx_process_call({{util.g_ctx_var()}}->process, {{util.g_ctx_var()}}->{{i.collName}}, {{i.collName}}_item_put, HPX_NULL, _item,
        __item_size);
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

{{i.type.ptrType}} cncGet_{{i.collName}}({{ util.print_tag(i.key, typed=True) }}{{util.g_ctx_param()}}) {
    {% if not i.isVirtual -%}
    {#/*****NON-VIRTUAL*****/-#}
    {{ util.log_msg("GET-DEP", i.collName, i.key) }}
    {% if i.key -%}
    cncTag_t _tag[] = { {{i.key|join(", ")}} };
    const size_t _tagSize = sizeof(_tag)/sizeof(*_tag);

    // Calculate the index at the future array
    long __idx = 0;
    int __i,__j;
    for (__i = _tagSize; __i > 0; __i--) {
      long entries = 1;
      for (__j = __i-1; __j > 0; __j--) {
        entries *= DEFAULT_ARRAY_SIZE;
      }

      __idx += entries * _tag[_tagSize-__i];
    }

    assert (__idx < {{util.g_ctx_var()}}->{{i.collName}}_size);

    int __item_size = sizeof({{i.type.baseType}});
    {{i.type.baseType}}* __value = cncItemAlloc(sizeof({{i.type.baseType}}));
    hpx_addr_t __addr = item_at({{util.g_ctx_var()}}->{{i.collName}}, __idx);
    hpx_call_sync(__addr, {{i.collName}}_item_get, __value, __item_size); 
    {% if i.type.isPtrType %}
    return __value;
    {% else %}
    return *__value;
    {%- endif %}
    {%- else -%}
    int __item_size = sizeof({{i.type.baseType}});
    {{i.type.ptrType}} __value;
    hpx_call_sync({{util.g_ctx_var()}}->{{i.collName}}, {{i.collName}}_item_get, &__value, 
        sizeof(__value));
    {% if i.type.isPtrType %}
    return __value;
    {% else %}
    return *__value;
    {%- endif %}
    {%- endif %}
    {%- else -%}
    {% set targetColl = g.itemDeclarations[i.mapTarget] -%}
    {% if i.isInline -%}
    {#/*****INLINE VIRTUAL*****/-#}
    return cncGet_{{i.mapTarget}}({{
        util.print_tag(i.keyFunction)
        }}{{util.g_ctx_var()}});
    {%- else -%}
    {#/*****EXTERN VIRTUAL******/-#}
    {{i.mapTarget}}ItemKey _key = {{i.functionName}}({{
        util.print_tag(i.key) }}{{util.g_ctx_var()}});
    return cncGet_{{i.mapTarget}}({{
        util.print_tag(targetColl.key, prefix="_key.")
        }}{{util.g_ctx_var()}});
    {%- endif %}
    {%- endif %}
}

{% endfor %}
