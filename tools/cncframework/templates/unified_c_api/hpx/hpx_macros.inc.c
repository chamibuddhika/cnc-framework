{#/****** Print all the components of a key or tag with pointer ******/#}
{% macro print_tag_pointers(tag, typed=False, prefix="") -%}
{% for x in tag %}{% if typed %}cncTag_t {% endif %}&{{prefix ~ x}}, {% endfor -%}
{%- endmacro %}

{#/****** Print the type of a given item collection ******/#}
{% macro print_collType(collName) -%}
{% for i in g.itemDeclarations.values() %}
{% if collName == i.collName -%}
{{i.type.baseType}}
{% endif %}
{% endfor %}
{%- endmacro %}

