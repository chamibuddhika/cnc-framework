{% import "common_macros.inc.c" as util with context -%}
{% import "hpx_macros.inc.c" as hpxutil with context -%}
{{ util.auto_file_banner() }}

#include "{{g.name}}_internal.h"
#include "cnchpx.h"

{#/****** Item instance data cast ******/-#}
{%- macro unpack_item(i) -%}
{%- with itemType = g.lookupType(i) -%}
{%- if not itemType.isPtrType %}*{% endif -%}
{{ "(" ~ itemType.ptrType ~ ")" }}
{%- endwith -%}
{%- endmacro -%}

#ifdef CNC_DEBUG_LOG
#if !defined(CNCOCR_x86)
#error "Debug logging mode only supported on x86 targets"
#endif
#include <pthread.h>
extern pthread_mutex_t _cncDebugMutex;
#endif /* CNC_DEBUG_LOG */

{% for stepfun in g.finalAndSteps %}
{% set isFinalizer = loop.first -%}
{% set paramTag = (stepfun.tag|count) <= 8 -%}

/* {{stepfun.collName}} task creation */
int {{util.qualified_step_name(stepfun)}}_handler(void* context, size_t size);

HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, 
           {{util.qualified_step_name(stepfun)}}_action, 
           {{util.qualified_step_name(stepfun)}}_handler,
           HPX_POINTER, HPX_SIZE_T);

int {{util.qualified_step_name(stepfun)}}_handler(void* context, size_t size) {

{%- call util.render_indented(1) %}
{{util.g_ctx_param()}} = ({{util.g_ctx_t()}}*)context;
/* #if CNC_DEBUG_TRACE
    printf("<<CnC Trace>>: RUNNING {{stepfun.collName}} @ %ld, %ld\n", 
        ctx->addToLeftEdge.row, ctx->addToLeftEdge.col, ctx);
#endif */
{{util.qualified_step_name(stepfun)}}(
    {% for tag in stepfun.tag %}{{util.g_ctx_var()}}->{{stepfun.collName}}.{{tag}}, {% endfor -%}
    {% for input in stepfun.inputs recursive %}
    {%- if input.keyRanges|count == 0 -%}
    {{util.g_ctx_var()}}->{{stepfun.collName}}.{{input.binding}},
    {%- else -%} 
    {{util.g_ctx_var()}}->arr_data.{{stepfun.collName}}.{{input.binding}}, 
    {%- endif -%}
    {% endfor -%}
    {{util.g_ctx_var()}});

/* #if CNC_DEBUG_TRACE
    printf("<<CnC Trace>>: DONE {{stepfun.collName}} @ %ld, %ld\n", 
        ctx->addToLeftEdge.row, ctx->addToLeftEdge.col, ctx);
#endif */
return HPX_SUCCESS;
{% endcall %}

}

void cncPrescribe_{{stepfun.collName}}({{
        util.print_tag(stepfun.tag, typed=True)
        ~ util.g_ctx_param() }}) {

  // Do a synchronous cncGet_cells to get the input parameters to the step 
  // function.
{#-/****** Set up input items *****/#}
{% set inputIsEnabled = [ true ] -%}
{%- call util.render_indented(1) -%}
size_t __ctxSz = 0;
{{util.g_ctx_t()}}* newCtx = {{util.g_ctx_var()}}; 
{% for input in stepfun.inputs recursive -%}
{% if input.kind in ['IF', 'ELSE'] -%}
if ({{ input.cond }}) {
{%- call util.render_indented(1) -%}
{{ loop(input.refs) }}
{%- endcall %}
}
{% do inputIsEnabled.append(false) -%}
else {
{%- call util.render_indented(1) -%}
{{ loop(input.refs) }}
{%- endcall %}
}

{% do inputIsEnabled.pop() -%}
{% else -%}
{{hpxutil.print_collType(input.collName)}} {{input.binding}};
{%- set comment = "Set up \"" ~ input.binding ~ "\" input dependencies" -%}
{#/* FIXME: shouldn't even do gets if the input is disabled,
  but that will require a more complicated calculation on the
  dependence count. I could do something where the complicated count
  is only used for steps with conditional ranged inputs... */-#}
{%- if inputIsEnabled[-1] -%}
{%- if input.keyRanges|count == 0 -%}
{%- call(var) util.render_tag_nest(comment, input, useTag=inputIsEnabled[-1]) -%}
{{input.binding}} = cncGet_{{input.collName}}(
        {%- for k in input.key %}_i{{loop.index0}}, {% endfor -%}
        {{util.g_ctx_var()}});
{%- endcall -%}
{% else %}
__ctxSz = sizeof({{util.g_ctx_t()}}) + 
    {%- for k in input.key %} ({{k.end}} - {{k.start}}) * {% endfor -%}sizeof({{hpxutil.print_collType(input.collName)}});
newCtx = ({{util.g_ctx_t()}}*)cncItemAlloc(__ctxSz);
memcpy(newCtx, ctx, sizeof(EvenOddSumsCtx));
// newCtx->arr_data = {0};
newCtx->arr_data.{{stepfun.collName}}.arr_size = {%- for k in input.key %} ({{k.end}} - {{k.start}}) * {% endfor -%} 1;

{%- call(var) util.render_tag_nest(comment, input, useTag=inputIsEnabled[-1]) -%}
newCtx->arr_data.{{stepfun.collName}}.{{var}} =  cncGet_{{input.collName}}(
        {%- for k in input.key %}_i{{loop.index0}}, {% endfor -%}
        {{util.g_ctx_var()}});
{%- endcall -%}
{%- endif -%}
{% else %}
{{ input.binding }} = 0;
// TODO here
{%- endif -%}
{%- endif -%}
{% endfor %}
{% endcall %}

{%- call util.render_indented(1) -%}
{%- set inputIsEnabled = [ true ] -%}
{% for input in stepfun.inputs recursive -%}
{{hpxutil.print_set_new_ctx_binding(stepfun.collName, input.binding, input.binding, stepfun.inputs, ";")}}
{% endfor %}
{% for tag in stepfun.tag -%}
{{hpxutil.print_set_new_ctx_tag(stepfun.collName, tag, tag, stepfun.tag, ";")}} 
{% endfor %}

if (__ctxSz == 0) {
  __ctxSz = sizeof({{util.g_ctx_t()}});
}

{{ util.log_msg("PRESCRIBED", stepfun.collName, stepfun.tag) }}

{% if isFinalizer %}
hpx_call_sync(
    HPX_HERE,
    {{util.qualified_step_name(stepfun)}}_action, 
    NULL,
    0,
    newCtx, __ctxSz);
{% else %}
hpx_process_call(ctx->process,
    HPX_HERE, 
    {{util.qualified_step_name(stepfun)}}_action, 
    HPX_NULL, 
    newCtx, __ctxSz);
{% endif %}

// cncItemFree(newCtx);
{% endcall %}

}
{% endfor %}
