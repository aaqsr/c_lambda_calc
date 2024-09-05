#include "lambda_calc.h"
#include <assert.h>

bool isFreeVar(Exp* e, Variable v)
{
  assert(e);

  // printf("IsFreeVar Var: ");
  // str_print(v.str);
  // printf(", Exp (Address %p): ", e);
  // print_exp(e);

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

void substitute(Arena a, Exp* ast, Variable target, Exp* subs)
{
  assert(ast);
  switch (ast->type) {
    case exp_VAR:
      if (str_eq(ast->toVar.str, target.str)) {
        *ast = deep_clone_exp(a, subs);
      }
      return;
    case exp_ABS:
      // $(λy.e1)[x<-e2] = λy.e1[x<-e2]$
      // where $x != y$, and $y$ not in $Fv[e2]$

      // Variable is free in the lambda
      if (isFreeVar(ast, target)) {
        // We never want to do a substitution that makes a free variable bound
        // to prevent capture
        if (isFreeVar(subs, ast->toAbs.var)) {
          // We perform alpha_equiv to replace the bound variable and continue
          // with the substitution Add a number in front of the bound variable
          alpha_equiv(a, &(ast->toAbs));
          substitute(a, ast, target, subs);
        } else {
          substitute(a, ast->toAbs.exp, target, subs);
        }
      }
      return;
    case exp_APP:
      substitute(a, ast->toApp.exp1, target, subs);
      substitute(a, ast->toApp.exp2, target, subs);
      return;
  }
}

// $λx.e =_\alpha λy.e[x<-y]$, $y$ not in $Fv[e]$
void alpha_equiv(Arena a, Abstraction* abs)
{
  Variable old_bound = abs->var;
  Variable new_bound =
    (Variable){.str = const_str(str_cat(
                 a, abs->var.str, const_str(str_from_integer(a, counter()))))};
  Exp new_bound_exp = (Exp){.type = exp_VAR, .toVar = new_bound};

  // Make the change in the rest of the expression too
  substitute(a, abs->exp, old_bound, &new_bound_exp);
  abs->var = new_bound;
}

Exp* beta_reduce(Arena a, Exp* abs, Exp* subs)
{
  assert(abs->type == exp_ABS);
  substitute(a, abs->toAbs.exp, abs->toAbs.var, subs);
  return abs->toAbs.exp;
}

void lazy_interp(Arena a, Exp* ast, context_list context)
{
  switch (ast->type) {
    case exp_APP:
      lazy_interp(a, ast->toApp.exp1, context);
      if (ast->toApp.exp1->type == exp_ABS) {
        *ast = *(beta_reduce(a, ast->toApp.exp1, ast->toApp.exp2));
        lazy_interp(a, ast, context);
      } else {
        lazy_interp(a, ast->toApp.exp2, context);
      }
      break;

    case exp_VAR: {
      Exp* sub = NULL;
      if ((sub = context_search(context, ast->toVar))) {
        *ast = deep_clone_exp(a, sub);
      }
    } break;
  }
}

bool isReducible(Exp* e, context_list ctx)
{
  switch (e->type) {
    case exp_VAR: return (context_search(ctx, e->toVar) != NULL);
    case exp_ABS: return (isReducible(e->toAbs.exp, ctx));
    case exp_APP:
      return (e->toApp.exp1->type == exp_ABS ||
              isReducible(e->toApp.exp1, ctx) ||
              isReducible(e->toApp.exp2, ctx));
  }
  return false;
}
