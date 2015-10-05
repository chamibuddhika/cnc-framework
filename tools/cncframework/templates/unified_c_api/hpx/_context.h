{% import "common_macros.inc.c" as util with context -%}
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
hpx_addr_t process;
hpx_addr_t termination_lco;
} {{util.g_ctx_t()}};

#endif /*{{defname}}*/
