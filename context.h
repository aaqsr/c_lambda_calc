#ifndef AAQ_CONTEXT_H_
#define AAQ_CONTEXT_H_

#include "ast.h"

typedef struct context_node context_node;

struct context_node
{
  Variable var;
  Exp* exp;
  context_node* nxt;
};

Exp* search_context(context_node* context, Variable var);

void print_context(context_node* context);

#endif
