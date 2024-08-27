#ifndef AAQ_AST_H_
#define AAQ_AST_H_

#include "str.h"

typedef struct exp exp;

typedef struct
{
  const_string str;
} Variable;

typedef struct
{
  Variable var;
  exp* exp;
} Abstraction;

typedef struct
{
  exp* exp1;
  exp* exp2;
} Application;

struct exp
{
  enum {
    exp_NULL = 0,
    exp_VAR, // Variable
    exp_ABS, // Abstraction
    exp_APP, // Application
  } type;

  union
  {
    Variable toVar;
    Abstraction toAbs;
    Application toApp;
  };
};

void print_exp_helper(exp* exp);
void print_exp(exp* exp);

#endif
