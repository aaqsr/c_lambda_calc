#ifndef AAQ_AST_H_
#define AAQ_AST_H_

#include "str.h"

typedef struct Exp Exp;

typedef struct
{
  const_string str;
} Variable;

typedef struct
{
  Variable var;
  Exp* exp;
} Abstraction;

typedef struct
{
  Exp* exp1;
  Exp* exp2;
} Application;

struct Exp
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

void print_exp(Exp* exp);

#endif
