{% import "common_macros.inc.c" as util with context -%}
{% import "hpx_macros.inc.c" as hpxutil with context -%}
{{ util.auto_file_banner() }}

{% set defname = "_CNC_" ~ g.name.upper() ~ "_CONTEXT_H_" -%}
#ifndef {{defname}}
#define {{defname}}

#include "{{g.name}}_defs.h"
#include "cnchpx.h"

typedef struct {{g.name}}Context {
{%- for line in g.ctxParams %}
    {{ line }}
{%- endfor %}
{%- for i in g.concreteItems %}
    cncItemCollection_t {{i.collName}};
    long {{i.collName}}_size; 
{%- endfor %}
{% for stepfun in g.finalAndSteps %}
    struct {
       {{ hpxutil.print_tag(stepfun.tag, ";", typed=True)}}
       {{ hpxutil.print_bindings(stepfun.inputItems, ";", typed=True)}}
     } {{stepfun.collName}};
{%- endfor %}
    {{util.g_args_t()}} {{util.g_args_var()}};
    hpx_addr_t process;
    hpx_addr_t process_termination_lco;
   union {
{%- for stepfun in g.finalAndSteps %}
      struct {
        {{ hpxutil.print_range_inputs(stepfun.inputItems, ";", typed=True)}}
      } {{stepfun.collName}};
{%- endfor %}
    } arr_data;
} {{util.g_ctx_t()}};

#endif /*{{defname}}*/
