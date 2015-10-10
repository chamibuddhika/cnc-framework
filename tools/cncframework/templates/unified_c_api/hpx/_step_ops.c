{% import "common_macros.inc.c" as util with context -%}
{% import "hpx_macros.inc.c" as hpxutil with context -%}
{{ util.auto_file_banner() }}

#include "{{g.name}}.h"
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
{{util.g_ctx_param()}}=({{util.g_ctx_t()}}*)context;
/* #if CNC_DEBUG_TRACE
    printf("<<CnC Trace>>: RUNNING {{stepfun.collName}} @ %ld, %ld\n", 
        ctx->addToLeftEdge.row, ctx->addToLeftEdge.col, ctx);
#endif */
{{util.qualified_step_name(stepfun)}}(
    {% for tag in stepfun.tag %}{{util.g_ctx_var()}}->{{stepfun.collName}}.{{tag}}, {% endfor -%}
    {% for input in stepfun.inputs recursive %}{{util.g_ctx_var()}}->{{stepfun.collName}}.{{input.binding}}, {% endfor -%}
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
{%- set comment = "Set up \"" ~ input.binding ~ "\" input dependencies" -%}
{{hpxutil.print_collType(input.collName)}} {{input.binding}};
{%- call(var) util.render_tag_nest(comment, input, useTag=inputIsEnabled[-1]) -%}
{#/* FIXME: shouldn't even do gets if the input is disabled,
  but that will require a more complicated calculation on the
  dependence count. I could do something where the complicated count
  is only used for steps with conditional ranged inputs... */-#}
{%- if inputIsEnabled[-1] -%}
{{input.binding}} = cncGet_{{input.collName}}(
        {%- for k in input.key %}_i{{loop.index0}}, {% endfor -%}
        {{util.g_ctx_var()}});
{%- endif -%}
{%- endcall -%}
{% endif %}
{% endfor %}
{% endcall %}

{%- call util.render_indented(1) -%}
{% for input in stepfun.inputs recursive -%}
{{hpxutil.print_set_ctx_binding(stepfun.collName, input.binding, input.binding, stepfun.inputs, ";")}}
{% endfor %}
{% for tag in stepfun.tag -%}
{{hpxutil.print_set_ctx_tag(stepfun.collName, tag, tag, stepfun.tag, ";")}} 
{% endfor %}

{{ util.log_msg("PRESCRIBED", stepfun.collName, stepfun.tag) }}
size_t sz = sizeof({{util.g_ctx_t()}});
hpx_process_call(
    {{util.g_ctx_var()}}->process, 
    HPX_HERE, 
    {{util.qualified_step_name(stepfun)}}_action, 
    HPX_NULL, 
    {{util.g_ctx_var()}}, sz);
{% endcall %}

}
{% endfor %}
