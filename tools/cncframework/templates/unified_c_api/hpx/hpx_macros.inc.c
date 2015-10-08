{#/****** Print ranged type for item collection ******/#}
{% macro ranged_type(item) -%}
{{ g.lookupType(item)
 }}{{ ("*" * item.keyRanges|count) }}
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

{#/****** Print bindings for a list of items with given delimiter ******/#}
{% macro print_bindings(items, delimiter, typed=False, prefix="") -%}
{% for i in items %}
{%- if typed %}{{ ranged_type(i) }}{% endif -%}{{ prefix ~ i.binding }}{{delimiter}} {% endfor -%}
{%- endmacro %}

{#/****** Print all the components of a key or tag with given delimiter ******/#}
{% macro print_tag(tag, delimiter, typed=False, prefix="") -%}
{% for x in tag %}{% if typed %}cncTag_t {% endif %}{{prefix ~ x}}{{delimiter}}{% endfor -%}
{%- endmacro %}

{#/****** Print bindings for a list of items with given delimiter ******/#}
{% macro print_set_ctx_binding(fun, key, value, items, delimiter, prefix="") -%}
{%- for i in items -%}
{%- if prefix ~ i.binding == key -%}
ctx->{{fun}}.{{ prefix ~ i.binding }} = {{value}}{{delimiter}}
{%- endif -%}
{%- endfor -%}
{%- endmacro %}

{#/****** Print all the components of a key or tag with given delimiter ******/#}
{% macro print_set_ctx_tag(fun, key, value, tags, delimiter, prefix="") -%}
{%- for x in tags -%}
{%- if prefix ~ x == key -%}
ctx->{{fun}}.{{prefix ~ x}} = {{value}}{{delimiter}}
{%- endif -%}
{%- endfor -%}
{%- endmacro %}
