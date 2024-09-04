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
