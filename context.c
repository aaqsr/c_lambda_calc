#include "context.h"
#include "arena.h"
#include "ast.h"
#include "parser.h"
#include "scanner.h"
#include "str.h"

Exp* context_search(context_list context_list, Variable var)
{
  context_node* context = context_list.head;

  while (context) {
    if (str_eq(context->var.str, var.str)) {
      return context->exp;
    }
    context = context->nxt;
  }

  return NULL;
}

void context_print(context_list context_list)
{
  context_node* context = context_list.head;

  printf("Defn'\n");
  while (context) {
    printf("  ");
    str_print(context->var.str);
    printf(" = ");
    print_exp(context->exp);
    context = context->nxt;
  }
}

context_list context_from_file(Arena context_arena, FILE* f)
{
  Arena scratch = arena_init();

  context_list context = {0};

  str_readln_return defn_inp = {0};
  while (defn_inp = str_readln_from_file(scratch, f),
         defn_inp.status != srr_EOF)
  {
    const_string inp = const_str(defn_inp.res);

    if (str_eq(inp, const_str(EMPTY_STR))) {
      continue;
    }

    token* tokens = scan(scratch, inp);

    if (tokens && tokens->type == tok_TERM && tokens->nxt &&
        tokens->nxt->type == tok_EQ)
    {
      context = context_push_back(
        context_arena, context,
        (Variable){.str = const_str(str_copy(context_arena, tokens->term_str))},
        parse(context_arena, tokens->nxt->nxt));
    }
  }

  arena_free(scratch);

  return context;
}

context_list context_push_back(Arena context_arena, context_list lst,
                               Variable new, Exp* exp)
{
  context_node* res = arena_zalloc(context_arena, context_node, 1);
  res->exp = exp;
  res->var = deep_clone_var(context_arena, new);

  return (context_list){(lst.head) ? lst.head : res,
                        (lst.tail) ? (lst.tail->nxt = res) : res};
}
