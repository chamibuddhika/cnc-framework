{% import "common_macros.inc.c" as util with context -%}
{{ util.auto_file_banner() }}

{% set defname = "_CNCOCR_" ~ g.name.upper() ~ "_INTERNAL_H_" -%}
#ifndef {{defname}}
#define {{defname}}

#include "{{g.name}}.h"

/********************************\
 ********* ITEM FUNCTIONS ********
\********************************/

{% for name, i in g.itemDeclarations.items() %}
// void cncGet_{{name}}({{ util.print_tag(i.key, typed=True) }}ocrGuid_t destination, u32 slot, ocrDbAccessMode_t mode, {{util.g_ctx_param()}});
{{i.type.ptrType}} cncGet_{{name}}({{ util.print_tag(i.key, typed=True) }}
    {{util.g_ctx_param()}});

{% endfor %}

/********************************\
 ******** STEP FUNCTIONS ********
\********************************/

void {{util.qualified_step_name(g.initFunction)}}({{g.name}}Args *args, {{util.g_ctx_param()}});
{% for stepfun in g.finalAndSteps %}
void {{util.qualified_step_name(stepfun)}}({{
        util.print_tag(stepfun.tag, typed=True)}}{{
        util.print_bindings(stepfun.inputItems, typed=True)
        }}{{util.g_ctx_param()}});
{% endfor %}
#endif /*{{defname}}*/
