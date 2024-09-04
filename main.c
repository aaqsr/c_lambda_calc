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

void scanner_parser_print_test(void)
{
  Arena input_arena = arena_init();
  Arena token_arena = arena_init();
  Arena exp_arena = arena_init();

  const_string main_inp = const_str(str_readln(input_arena));
  token* main_tokens = scan(token_arena, main_inp);
  Exp* main_expr = parse(exp_arena, main_tokens);

  print_exp(main_expr);

  // `printf("%d\n", isFreeVar(expressions, (Variable){.str = inp}));`
  Variable target = (Variable){.str = const_str(str_readln(input_arena))};

  const_string sub_inp = const_str(str_readln(input_arena));
  token* sub_tokens = scan(token_arena, sub_inp);
  Exp* substitution = parse(exp_arena, sub_tokens);

  substitute(exp_arena, main_expr, target, substitution);
  print_exp(main_expr);

  arena_free(input_arena);
  arena_free(exp_arena);
  arena_free(token_arena);
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
  Arena context_arena = arena_init();
  Arena prev_exprs = arena_init();
  Arena scratch = arena_init();

  context_node* context = NULL;
  context_node* context_cur = NULL;

  if (argc == 2) {
    FILE* f = fopen(argv[1], "r");
    const_string defn_inp;
    while (!str_eq(defn_inp = const_str(str_readln_from_file(input_arena, f)),
                   const_str(EMPTY_STR)))
    {
      token* tokens = scan(token_arena, defn_inp);

      if (tokens && tokens->type == tok_TERM && tokens->nxt &&
          tokens->nxt->type == tok_EQ)
      {
        if (context) {
          context_cur->nxt = arena_zalloc(context_arena, context_node, 1);
          context_cur = context_cur->nxt;
        } else {
          context = arena_zalloc(context_arena, context_node, 1);
          context_cur = context;
        }
        context_cur->exp = parse(context_arena, tokens->nxt->nxt);
        context_cur->var = (Variable){
          .str = const_str(str_copy(context_arena, tokens->term_str))};
      }
    }
  }

  arena_reset(input_arena);
  arena_reset(token_arena);

  while (1) {
    printf(GRN "> " RST);
    const_string main_inp = const_str(str_readln(input_arena));
    // TODO: Make this into command interpreter with commands beginning with '?'
    if (str_eq(main_inp, str_lit("?def"))) {
      arena_reset(input_arena);
      print_context(context);
      continue;
    }

    token* main_tokens = scan(token_arena, main_inp);

    // It's a new assignment
    if (main_tokens && main_tokens->type == tok_TERM && main_tokens->nxt &&
        main_tokens->nxt->type == tok_EQ)
    {
      if (context) {
        context_cur->nxt = arena_zalloc(context_arena, context_node, 1);
        context_cur = context_cur->nxt;
      } else {
        context = arena_zalloc(context_arena, context_node, 1);
        context_cur = context;
      }
      context_cur->exp = parse(context_arena, main_tokens->nxt->nxt);
      context_cur->var = (Variable){
        .str = const_str(str_copy(context_arena, main_tokens->term_str))};
    } else {
      Exp* main_expr = parse(exp_arena, main_tokens);
      lazy_interp(exp_arena, main_expr, context);
      // If it can be reduced, reduce it
      // TODO: Add a history
      while (isReducible(main_expr, context)) {
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
