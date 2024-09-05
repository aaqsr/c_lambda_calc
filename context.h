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

typedef struct
{
  context_node* head;
  context_node* tail;
} context_list;

Exp* context_search(context_list context_list, Variable var);

void context_print(context_list context_list);

context_list context_from_file(Arena context_arena, FILE* f);

context_list context_push_back(Arena context_arena, context_list lst, Variable new, Exp* exp);

#endif
