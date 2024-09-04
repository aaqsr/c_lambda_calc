#include "context.h"

Exp* search_context(context_node* context, Variable var)
{
  while (context) {
    if (str_eq(context->var.str, var.str)) {
      return context->exp;
    }
    context = context->nxt;
  }
  return NULL;
}

void print_context(context_node* context)
{
  printf("Defn'\n");
  while (context) {
    printf("  ");
    str_print(context->var.str);
    printf(" = ");
    print_exp(context->exp);
    context = context->nxt;
  }
}
