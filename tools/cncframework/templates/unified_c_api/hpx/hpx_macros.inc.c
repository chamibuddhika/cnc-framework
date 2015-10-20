{#/****** Filter out ranged type for item collection since we handle it seperately ******/#}
{% macro filter_ranged_type(item) -%}
{%- if item.keyRanges|count == 0 -%}
{{ g.lookupType(item)}}{# /*{{ ("[]" * item.keyRanges|count) }}*/ #}
{%- endif -%}
{%- endmacro %}

{#/****** Print all the components of a key or tag with pointer ******/#}
{% macro print_tag_pointers(tag, typed=False, prefix="") -%}
{% for x in tag %}{% if typed %}cncTag_t {% endif %}&{{prefix ~ x}}, {% endfor -%}
{%- endmacro %}

{#/****** Print the type of a given item collection ******/#}
{% macro print_collType(collName) -%}
{% for i in g.itemDeclarations.values() %}
{%- if collName == i.collName -%}{{i.type.baseType}}{%- endif -%}
{% endfor %}
{%- endmacro %}

{#/****** Print bindings for a ranged inputs with given delimiter ******/#}
{% macro print_range_inputs(items, delimiter, typed=False, prefix="") -%}
{% for i in items %}
{%- if i.keyRanges|count > 0 -%}
int arr_size{{delimiter}}
{%- if typed %}{{ g.lookupType(i) }}{% endif -%} {{ prefix ~ i.binding }}{{ ("[]" * i.keyRanges|count)}}{{delimiter}}
{%- endif -%}
{% endfor -%}
{%- endmacro %}

{#/****** Print bindings for a list of items with given delimiter ******/#}
{% macro print_bindings(items, delimiter, typed=False, prefix="") -%}
{% for i in items %}
{%- if i.keyRanges|count == 0 -%}
{%- if typed %}{{ filter_ranged_type(i) }}{% endif -%} {{ prefix ~ i.binding }}{{delimiter}}
{%- endif -%}
{% endfor -%}
{%- endmacro %}

{#/****** Print all the components of a key or tag with given delimiter ******/#}
{% macro print_tag(tag, delimiter, typed=False, prefix="") -%}
{% for x in tag %}{% if typed %}cncTag_t {% endif %}{{prefix ~ x}}{{delimiter}}{% endfor -%}
{%- endmacro %}

{#/****** Print bindings for a list of items with given delimiter ******/#}
{% macro print_set_new_ctx_binding(fun, key, value, items, delimiter, prefix="") -%}
{%- for i in items -%}
{%- if i.keyRanges|count == 0 -%}
{%- if prefix ~ i.binding == key -%}
newCtx->{{fun}}.{{ prefix ~ i.binding }} = {{value}}{{delimiter}}
{%- endif -%}
{%- endif -%}
{%- endfor -%}
{%- endmacro %}

{#/****** Print all the components of a key or tag with given delimiter ******/#}
{% macro print_set_new_ctx_tag(fun, key, value, tags, delimiter, prefix="") -%}
{%- for x in tags -%}
{%- if prefix ~ x == key -%}
newCtx->{{fun}}.{{prefix ~ x}} = {{value}}{{delimiter}}
{%- endif -%}
{%- endfor -%}
{%- endmacro %}
