#include "arena.h"
#include "ast.h"
#include "colours.h"
#include "context.h"
#include "lambda_calc.h"
#include "parser.h"
#include "scanner.h"
#include "str.h"
#include <assert.h>
#include <stdio.h>

#define TODO() (assert(0 && "TODO"), 0)

context_list parse_args(Arena context_arena, int argc, char* argv[])
{
  if (argc == 2) {
    FILE* f = fopen(argv[1], "r");
    return context_from_file(context_arena, f);
  }

  return (context_list){0};
}

// clean up,
// tests
// better errors
// better seg fault tolerance
int main(int argc, char* argv[])
{
  Arena input_arena = arena_init();
  Arena token_arena = arena_init();
  Arena exp_arena = arena_init();
  Arena prev_exprs = arena_init();
  Arena scratch = arena_init();

  Arena context_arena = arena_init();
  context_list context = parse_args(context_arena, argc, argv);

  while (1) {
    printf(GRN "> " RST);
    const_string main_inp = const_str(str_readln(input_arena).res);
    // TODO: Make this into command interpreter with commands beginning with '?'
    if (str_eq(main_inp, str_lit("?def"))) {
      arena_reset(input_arena);
      context_print(context);
      continue;
    }

    token* main_tokens = scan(token_arena, main_inp);

    // It's a new assignment
    if (main_tokens && main_tokens->type == tok_TERM && main_tokens->nxt &&
        main_tokens->nxt->type == tok_EQ)
    {
      context = context_push_back(
        context_arena, context,
        (Variable){.str =
                     const_str(str_copy(context_arena, main_tokens->term_str))},
        parse(context_arena, main_tokens->nxt->nxt));
    } else {
      Exp* main_expr = parse(exp_arena, main_tokens);
      lazy_interp(exp_arena, main_expr, context);
      // If it can be reduced, reduce it
      // TODO: Add a history
      // while (isReducible(main_expr, context)) {
      for (int i = 0; i < 15; ++i) {
        *arena_zalloc(prev_exprs, Exp, 1) = deep_clone_exp(scratch, main_expr);
        lazy_interp(exp_arena, main_expr, context);
      }
      arena_reset(scratch);
      arena_reset(prev_exprs);
      printf(YEL "  -> " RST);
      print_exp(main_expr);
    }

    arena_reset(input_arena);
    arena_reset(exp_arena);
    arena_reset(token_arena);
  }

  arena_free(scratch);
  arena_free(prev_exprs);
  arena_free(context_arena);
  arena_free(input_arena);
  arena_free(exp_arena);
  arena_free(token_arena);
}
