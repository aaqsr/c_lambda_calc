#include "arena.h"
#include "ast.h"
#include "parser.h"
#include "scanner.h"
#include "str.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define TODO() (assert(0 && "TODO"), 0)

bool isFreeVar(exp* e, Variable v)
{
  assert(e);

  switch (e->type) {
    case exp_VAR: return str_eq(e->toVar.str, v.str);
    case exp_ABS:
      return (!str_eq(e->toAbs.var.str, v.str) && isFreeVar(e->toAbs.exp, v));
    case exp_APP:
      return (isFreeVar(e->toApp.exp1, v) || isFreeVar(e->toApp.exp2, v));
  }

  return false;
}

int counter(void)
{
  static int count = 1;
  return count++;
}

void substitute(Arena a, exp* e, Variable target, exp* sub)
{
  assert(e);
  switch (e->type) {
    case exp_VAR:
      if (str_eq(e->toVar.str, target.str)) {
        *e = *sub;
      }
      return;
    case exp_ABS:
      // $(λy.e1)[x<-e2] = λy.e1[x<-e2]$
      // where $x != y$, and $y$ not in $Fv[e2]$

      // Variable is free in the lambda
      if (isFreeVar(e, target)) {
        // We never want to do a substitution that makes a free variable bound
        // to prevent capture
        if (isFreeVar(sub, e->toAbs.var)) {
          // We replace the bound variable and continue with the substitution
          // Add a number in front of the bound variable
          Variable old_bound = e->toAbs.var;
          Variable new_bound = (Variable){
            .str = const_str(str_cat(
              a, e->toAbs.var.str, const_str(str_from_integer(a, counter()))))};
          exp new_bound_exp = (exp){.type = exp_VAR, .toVar = new_bound};

          // Make the change in the rest of the expression too
          substitute(a, e->toAbs.exp, old_bound, &new_bound_exp);
          e->toAbs.var = new_bound;

          substitute(a, e, target, sub);
        } else {
          substitute(a, e->toAbs.exp, target, sub);
        }
      }
      return;
    case exp_APP:
      substitute(a, e->toApp.exp1, target, sub);
      substitute(a, e->toApp.exp2, target, sub);
      return;
  }
}

void scanner_parser_print_test(void)
{
  Arena input_arena = arena_init();
  Arena token_arena = arena_init();
  Arena exp_arena = arena_init();

  const_string main_inp = const_str(str_readln(input_arena));
  token* main_tokens = scan(token_arena, main_inp);
  exp* main_expr = parse(exp_arena, main_tokens);

  print_exp(main_expr);

  // `printf("%d\n", isFreeVar(expressions, (Variable){.str = inp}));`
  Variable target = (Variable){.str = const_str(str_readln(input_arena))};

  const_string sub_inp = const_str(str_readln(input_arena));
  token* sub_tokens = scan(token_arena, sub_inp);
  exp* substitution = parse(exp_arena, sub_tokens);

  substitute(exp_arena, main_expr, target, substitution);
  print_exp(main_expr);

  arena_free(input_arena);
  arena_free(exp_arena);
  arena_free(token_arena);
}

int main(void)
{
  scanner_parser_print_test();
}
