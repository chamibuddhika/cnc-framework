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
void {{util.qualified_step_name(stepfun)}}_handler({{ util.print_tag(stepfun.tag, typed=True)
        }}{{ util.print_bindings(stepfun.inputItems, typed=True)
        }}{{util.g_ctx_param()}});

HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, 
           {{util.qualified_step_name(stepfun)}}_action, 
           {{util.qualified_step_name(stepfun)}}_handler,
           {% for x in stepfun.tag -%}get_hpx_type("cncTag_t"), {% endfor -%} 
           {% for input in stepfun.inputs recursive -%}
           get_hpx_type("{{hpxutil.print_collType(input.collName)}}"), {% endfor -%}
           HPX_POINTER, HPX_SIZE_T);

static void cncPrescribe_{{stepfun.collName}}_handler({{ util.print_tag(stepfun.tag, typed=True)
        }}{{ util.print_bindings(stepfun.inputItems, typed=True)
        }}void*{{util.g_ctx_var()}}, size_t size) {

{%- call util.render_indented(1) -%}
{{util.g_ctx_param()}}=({{util.g_ctx_t()}}*){{util.g_ctx_var()}};
{{util.qualified_step_name(stepfun)}}(
    {{util.print_tag(stepfun.tag, typed=False)}}
    {% for input in stepfun.inputs recursive %} {{input.binding}}, {% endfor -%}
    {{util.g_ctx_var()}});
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
{{hpxutil.print_collType(input.collName)}}{{input.binding}};
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
size_t sz = sizeof({{util.g_ctx_t()}});
hpx_process_call({{util.g_ctx_var()}}->process, HPX_HERE, {{util.qualified_step_name(stepfun)}}_action, done, 
    {{hpxutil.print_tag_pointers(stepfun.tag, typed=False)}}
    {% for input in stepfun.inputs recursive %} &{{input.binding}}, {% endfor -%}
    {{util.g_ctx_var()}}, &sz);
{% endcall %}

}
{% endfor %}
