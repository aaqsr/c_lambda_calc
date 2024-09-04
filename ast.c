#include "ast.h"
#include <assert.h>

#define MAX_RECUR_DEPTH 30

void print_exp_helper(Exp* exp, bool print_addrs, int recur_depth)
{
  assert(recur_depth < MAX_RECUR_DEPTH);
  ++recur_depth;

  if (exp == NULL) {
    printf("NULL\n");
    return;
  }

  if (print_addrs) {
#define BLK "\e[0;30m"
#define WHT "\e[0;37m"
#define RESET "\x1B[0m"
    printf(WHT "(%p)" RESET, exp);
  }

  switch (exp->type) {
    case exp_VAR: str_print(exp->toVar.str); break;
    case exp_ABS:
      printf("λ");
      str_print(exp->toAbs.var.str);
      printf(".");
      print_exp_helper(exp->toAbs.exp, print_addrs, recur_depth);
      break;
    case exp_APP:
      // Naive algorithm:
      // printf("(");
      // print_exp_helper(exp->toApp.exp1);
      // printf(" ");
      // print_exp_helper(exp->toApp.exp2);
      // printf(")");

      // Better algorithm:
      // If first expression is Abstraction, surround with parentheses, else
      // just print
      if (exp->toApp.exp1->type == exp_ABS) {
        printf("(");
        print_exp_helper(exp->toApp.exp1, print_addrs, recur_depth);
        printf(") ");
      } else {
        print_exp_helper(exp->toApp.exp1, print_addrs, recur_depth);
        printf(" ");
      }

      // If second expression is Abstraction or Application, surround with
      // parentheses, else just print it
      if (exp->toApp.exp2->type == exp_ABS || exp->toApp.exp2->type == exp_APP)
      {
        printf("(");
        print_exp_helper(exp->toApp.exp2, print_addrs, recur_depth);
        printf(")");
      } else {
        print_exp_helper(exp->toApp.exp2, print_addrs, recur_depth);
      }
      break;
    default: break;
  }
}

void print_exp(Exp* exp)
{
  print_exp_helper(exp, false, 0);
  printf("\n");
}

Variable deep_clone_var(Arena a, Variable v)
{
  return (Variable){.str = const_str(str_copy(a, v.str))};
}

Abstraction deep_clone_abs(Arena a, Abstraction abs)
{
  Exp* clone = arena_zalloc(a, Exp, 1);
  *clone = deep_clone_exp(a, abs.exp);
  return (Abstraction){.var = deep_clone_var(a, abs.var), .exp = clone};
}

Application deep_clone_app(Arena a, Application app)
{
  Exp* clone1 = arena_zalloc(a, Exp, 1);
  *clone1 = deep_clone_exp(a, app.exp1);
  Exp* clone2 = arena_zalloc(a, Exp, 1);
  *clone2 = deep_clone_exp(a, app.exp2);
  return (Application){.exp1 = clone1, .exp2 = clone2};
}

Exp deep_clone_exp(Arena a, const Exp* e)
{
  assert(e);

  Exp res = {0};
  res.type = e->type;

  switch (e->type) {
    case exp_VAR: res.toVar = deep_clone_var(a, e->toVar); break;
    case exp_ABS: res.toAbs = deep_clone_abs(a, e->toAbs); break;
    case exp_APP: res.toApp = deep_clone_app(a, e->toApp); break;
  }

  return res;
}

bool exp_equal(Exp* e1, Exp* e2)
{
  assert(e1 && e2);

  if (e1->type != e2->type) {
    return false;
  }

  switch (e1->type) {
    case exp_VAR: return str_eq(e1->toVar.str, e2->toVar.str);
    case exp_ABS:
      return (str_eq(e1->toAbs.var.str, e2->toAbs.var.str) &&
              exp_equal(e1->toAbs.exp, e2->toAbs.exp));
    case exp_APP:
      return (exp_equal(e1->toApp.exp1, e2->toApp.exp1) &&
              exp_equal(e1->toApp.exp2, e2->toApp.exp2));
  }

  return false;
}
