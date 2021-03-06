{% import "common_macros.inc.c" as util with context -%}
{{ util.auto_file_banner() }}

#include "{{g.name}}_internal.h"
#include "cnchpx.h"

#include <string.h>
#include <stdlib.h>

#ifdef CNC_DEBUG_LOG
#ifndef CNCOCR_x86
#error "Debug logging mode only supported on x86 targets"
#endif
#include <pthread.h>
pthread_mutex_t _cncDebugMutex = PTHREAD_MUTEX_INITIALIZER;
#endif /* CNC_DEBUG_LOG */

/***********************************************************
 * HPX related functions                                  *
 ***********************************************************/

// Handler prototypes
int hpx_main_handler(void* context, size_t ctxSz);
int hpx_launch_handler(void* context, size_t ctxSz);

// Handlers and actions
HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, hpx_launch, hpx_launch_handler, HPX_POINTER, 
    HPX_SIZE_T);
HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, hpx_main, hpx_main_handler, HPX_POINTER, 
    HPX_SIZE_T);

int hpx_launch_handler(void* context, size_t ctxSz) {
  {{util.g_ctx_param()}} = ({{util.g_ctx_t()}}*) context;
  // initialize item collections
  {% for i in g.concreteItems -%}
  {
  {% if i.key -%}

  // Calculate the item collection size to be allocated
  long arr_size = 1;
  int i;
  for (i=0; i < {{length(i.key)}}; i++) {
    arr_size *= DEFAULT_ARRAY_SIZE;
  }

  {{util.g_ctx_var()}}->{{i.collName}} = _cncItemCollectionCreate(arr_size,
      sizeof(cncItemFuture), sizeof({{i.type.baseType}}));
  // printf("[HPX_LAUNCH_HANDLER] {{util.g_ctx_var()}}->{{i.collName}} : %lu\n", {{util.g_ctx_var()}}->{{i.collName}});
  {{util.g_ctx_var()}}->{{i.collName}}_size = arr_size; 
  {% else -%}
  {{util.g_ctx_var()}}->{{i.collName}} = _cncItemCollectionSingletonCreate(
    sizeof(cncItemFuture), sizeof({{i.type.baseType}}));
  {% endif -%}
  } 
  {% endfor -%}

  hpx_addr_t termination_lco = hpx_lco_and_new(1); 
  hpx_addr_t process = hpx_process_new(termination_lco);
  {{util.g_ctx_var()}}->process = process;
  {{util.g_ctx_var()}}->process_termination_lco = termination_lco;

  hpx_process_call({{util.g_ctx_var()}}->process, HPX_HERE, hpx_main, HPX_NULL,
      {{util.g_ctx_var()}}, ctxSz);

  cncPrescribe_{{g.finalizeFunction.collName}}({{util.g_ctx_var()}});

  hpx_process_delete(ctx->process, HPX_NULL);
  hpx_lco_delete(ctx->process_termination_lco, HPX_NULL);

  {{g.name}}_destroy(ctx);

  hpx_exit(0);
  return HPX_SUCCESS;

}

int hpx_main_handler(void* context, size_t ctxSz) {
  {{util.g_ctx_param()}} = ({{util.g_ctx_t()}}*) context;

  {{util.qualified_step_name(g.initFunction)}}(&{{util.g_ctx_var()}}->{{util.g_args_var()}}, {{util.g_ctx_var()}});

  hpx_lco_wait({{util.g_ctx_var()}}->process_termination_lco);

  // {{g.name}}_destroy(ctx);

  return HPX_SUCCESS;

}

{{util.g_ctx_t()}} *{{g.name}}_create() {
     
    {{util.g_ctx_t()}}* {{util.g_ctx_var()}} = 
      ({{util.g_ctx_t()}}*) malloc(sizeof({{util.g_ctx_t()}}));
    return {{util.g_ctx_var()}};
}

void {{g.name}}_destroy({{util.g_ctx_param()}}) {
    // destroy item collections
    {% for i in g.concreteItems -%}
    {% if i.key -%}
    _cncItemCollectionDestroy({{util.g_ctx_var()}}->{{i.collName}});
    {% else -%}
    _cncItemCollectionSingletonDestroy({{util.g_ctx_var()}}->{{i.collName}});
    {% endif -%}
    {% endfor -%}

    // Free the context
    // free({{util.g_ctx_var()}});
}

void {{g.name}}_launch({{util.g_args_param()}}, {{util.g_ctx_param()}}) {

  {{util.g_ctx_var()}}->{{util.g_args_var()}} = *{{util.g_args_var()}}; 
  size_t ctxSz = sizeof({{util.g_ctx_t()}});
  hpx_run(&hpx_launch, {{util.g_ctx_var()}}, ctxSz); 

}

void {{g.name}}_await({{
        util.print_tag(g.finalizeFunction.tag, typed=True)
        }}{{util.g_ctx_param()}}) {
  /*
  cncPrescribe_{{g.finalizeFunction.collName}}(
        {%- for x in g.finalizeFunction.tag %}tag[{{loop.index0}}], {% endfor -%}
        {{util.g_ctx_var()}});
  hpx_lco_wait({{util.g_ctx_var()}}->termination_lco);
  hpx_lco_delete({{util.g_ctx_var()}}->termination_lco, HPX_NULL);
  hpx_process_delete({{util.g_ctx_var()}}->process, HPX_NULL);
  */

}

/* define NO_CNC_MAIN if you want to use mainEdt as the entry point instead */
#ifndef NO_CNC_MAIN

extern int cncMain(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    if (hpx_init(&argc, &argv)) {
      hpx_print_help();
      return -1;
    }

    cncMain(argc, argv);

    hpx_finalize();

    return 0;
}

#endif /* NO_CNC_MAIN */

