#include "ast.h"

void print_exp_helper(exp* exp)
{
  if (exp == NULL) {
    printf("NULL\n");
    return;
  }

  switch (exp->type) {
    case exp_VAR: str_print(exp->toVar.str); break;
    case exp_ABS:
      printf("λ");
      str_print(exp->toAbs.var.str);
      printf(".");
      print_exp_helper(exp->toAbs.exp);
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
        print_exp_helper(exp->toApp.exp1);
        printf(") ");
      } else {
        print_exp_helper(exp->toApp.exp1);
        printf(" ");
      }

      // If second expression is Abstraction or Application, surround with
      // parentheses, else just print it
      if (exp->toApp.exp2->type == exp_ABS || exp->toApp.exp2->type == exp_APP)
      {
        printf("(");
        print_exp_helper(exp->toApp.exp2);
        printf(")");
      } else {
        print_exp_helper(exp->toApp.exp2);
      }
      break;
    default: break;
  }
}

void print_exp(exp* exp)
{
  print_exp_helper(exp);
  printf("\n");
}
